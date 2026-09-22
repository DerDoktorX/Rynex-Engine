#pragma once
#include <rypch.h>
#include <Rynex/Asset/Base/Asset.h>

namespace Rynex {


	namespace Memory {
		template<typename T, typename... Args>
		struct is_one_of : std::disjunction<std::is_same<T, Args>...> {};

		template<typename T, typename... Args>
		inline constexpr bool is_one_of_v = is_one_of<T, Args...>::value;

		template<typename... Args>
		class WeakPtrSet
		{
		public:
			// static_assert((std::is_base_of_v<std::enable_shared_from_this<typename Args>, typename Args> && ...)
			// 	,"All Typs need to drive from std::enable_shared_from_this<T>.");
			using ValueTypesVariantWeak = typename std::variant<Weak<Args> ...>;

			using HashType = uint64_t;
			using SizeType = size_t;
			using DifferenceType = int64_t;

		private:
			struct Item
			{
				HashType m_Hash;
				ValueTypesVariantWeak m_WeakPtrVariant;

				Item() = delete;				
				Item(Item&&) = default;
				Item(const Item&) = default;




				template<typename T>
				explicit Item(T* value)
				{					
					static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");

					m_Hash = reinterpret_cast<HashType>(value);
					m_WeakPtrVariant = Asset::GetWeakInPlaceType<T>(value);
				}

				Item& operator=(const Item& item) = default;

				bool operator==(const Item& item) const
				{
					return m_Hash == item.m_Hash;
				}

				bool operator!=(const Item& item) const
				{
					return m_Hash != item.m_Hash;
				}

				bool operator<(const Item& item) const
				{
					return m_Hash < item.m_Hash;
				}

				bool operator<=(const Item& item) const
				{
					return m_Hash <= item.m_Hash;
				}

				bool operator>(const Item& item) const
				{
					return m_Hash > item.m_Hash;
				}

				bool operator>=(const Item& item) const
				{
					return m_Hash >= item.m_Hash;
				}

				
				[[nodiscard]] HashType Hash() const
				{
					return m_Hash;
				}

				[[nodiscard]] bool IsValid() const
				{
					
					return std::visit([](auto& weakPtr) { return !weakPtr.expired(); }, m_WeakPtrVariant);
				}

				template<typename Func>
				void OnItem(Func&& func) const
				{
					std::visit(func, m_WeakPtrVariant);
				}
				template<typename Func>
				void OnItem(Func&& func)
				{
					std::visit(func, m_WeakPtrVariant);
				}

				template<typename T>
				Ref<T> Get() const
				{
					static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Type list");

					if (Weak<T>* weakObjectPtr = std::get_if<Weak<T>>(&m_WeakPtrVariant))
						return weakObjectPtr->lock();
					return nullptr;
				}
			};
		public:
			using Container = typename std::vector<Item>;
			using ContainerIt = typename Container::iterator;
			using ContainerItConst = typename Container::const_iterator;

		public:
			WeakPtrSet()
				: m_LopeHole(0)
			{

			}
			~WeakPtrSet() = default;



			template<typename T>
			SizeType Set(T* valuePtr)
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Type list");

				Item item(valuePtr);
			
				RY_CORE_ASSERT(item.IsValid(), "value is not valid");


				const ContainerIt end = m_Container.end();
				const ContainerIt begin = m_Container.begin();

				const ContainerIt pos = std::lower_bound(begin, end, item);

				if (pos != end && pos->Hash() == item.Hash())
				{
					RY_CORE_ERROR("valuePtr is already in container!");
					return std::numeric_limits<SizeType>::max();
				}
				DifferenceType index = end != pos ? pos - begin : 0;
				RY_CORE_ASSERT(0 <= index && index <= m_Container.size(), "index is negative and (not to be Positive)!");
				m_Container.insert(pos, item);

				const SizeType indexSize = static_cast<SizeType>(index);

				
				return indexSize;
			}

			template<typename T>
			ContainerIt Find(const Item& item)
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Type list");
				return std::lower_bound(m_Container.begin(), m_Container.end(), item);
			}

			template<typename T>
			ContainerIt Find(T* valuePtr)
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Type list");
				Item item = Item(valuePtr);
				RY_CORE_ASSERT(item.IsValid(), "tried to set a Nullptr Value!");

				return Find(item);
			}

			template<typename T>
			bool Has(T* valuePtr) const
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");

				Item hasItem = Item(valuePtr);
				const bool result = std::binary_search(m_Container.begin(), m_Container.end(), hasItem);
				return result;
			}

			template<typename Func>
			void OnValue(SizeType index, Func&& func) const
			{
				const SizeType count = m_Container.size();
				RY_CORE_ASSERT(index < count, "tried to set a Nullptr Value!");
				const Item& item = m_Container.at(index);
				item.OnItem(func);
			}

			template<typename Func>
			void OnValue(SizeType index, Func&& func)
			{
				const SizeType count = m_Container.size();
				RY_CORE_ASSERT(index < count, "tried to set a Nullptr Value!");
				Item& item = m_Container.at(index);
				item.OnItem(func);
			}

			template<typename T>
			Ref<T> Get(SizeType index) const
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Type list");

				const SizeType count = m_Container.size();
				RY_CORE_ASSERT(index < count, "tried to set a Nullptr Value!");
				const Item& item = m_Container.at(index);
				Ref<T> valueRef;
				valueRef = item.template Get<T>();
				return valueRef;
			}

			template<typename T>
			Ref<T> Get(SizeType index)
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Type list");


				const SizeType count = m_Container.size();
				RY_CORE_ASSERT(index < count, "tried to set a Nullptr Value!");
				Item& item = m_Container.at(index);
				Ref<T> valueRef = item.Get();
				return valueRef;
			}

			template<typename T>
			void Remove(T* valuePtr)
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");


				Item hasItem = Item(valuePtr);
				if (!hasItem.IsValid())
				{
					RY_CORE_WARN("we removed a already Destroyed Value from list! is no longer vaild");
				}
				ContainerIt end = m_Container.end();
				ContainerIt pos = std::lower_bound(m_Container.begin(), end, hasItem);
				if (pos != end)
					m_Container.erase(pos);
				else
					RY_CORE_ERROR("we removed a already Destroyed Value from list! not found");
				
			}

			SizeType Size() const
			{
				return m_Container.size();
			}

			template<typename Func>
			void ForEche(Func&& func)
			{
				for (Item& item : m_Container)
				{
					if (m_LopeHole == item.Hash())
						continue;

					item.OnItem(func);
				}
				m_LopeHole = 0;

			}

			template<typename Func>
			void ForEche(Func&& func) const
			{
				for (const Item& item : m_Container)
				{
					if (m_LopeHole == item.Hash())
						continue;

					item.OnItem(func);
				}
				m_LopeHole = 0;
			}

			void FindNullptrAndRemove()
			{				
				using RitConst = typename Container::const_reverse_iterator;
				using ItConst = typename Container::const_iterator;


				ItConst begin = m_Container.begin();
				for (int i = m_Container.size() - 1; 0 <= i ; i--)
				{
					const Item& item = m_Container.at(i);
					if (item.IsValid())
						continue;

					ItConst pos = begin + i;
					m_Container.erase(pos);
				}
				
			}


			void Clear()
			{
				m_Container.clear();
			}

			bool Empty() const
			{
				return m_Container.empty();
			}

			// in next foreach loop we skip execution for that value if it exist it delete him self automaticly
			template<typename T>
			void SetLoopJump(T* valuePtr)const
			{
				Item item(valuePtr);
				m_LopeHole = item.Hash();
			}
		private:
			Container m_Container;
			mutable HashType m_LopeHole;
		};
	}
}
