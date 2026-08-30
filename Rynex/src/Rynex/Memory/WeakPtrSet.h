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
			
			using ValueTypesVarientWeak = typename std::variant<Weak<Args> ...>;

			using HashType = uint64_t;
			using SizeType = size_t;
			using DifernzType = int64_t;
		private:
			struct Item
			{
				HashType hash;
				ValueTypesVarientWeak weakPtrVarients;

				Item() = delete;				
				Item(Item&&) = default;
				Item(const Item&) = default;




				template<typename T>
				explicit Item(T* value)
				{					
					static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");

					hash = reinterpret_cast<HashType>(value);
					weakPtrVarients = Asset::GetWeakInPlaceType<T>(value);
				}

				Item& operator=(const Item& item) = default;

				bool operator==(const Item& item) const
				{
					return hash == item.hash;
				}

				bool operator!=(const Item& item) const
				{
					return hash != item.hash;
				}

				bool operator<(const Item& item) const
				{
					return hash < item.hash;
				}

				bool operator<=(const Item& item) const
				{
					return hash <= item.hash;
				}

				bool operator>(const Item& item) const
				{
					return hash > item.hash;
				}

				bool operator>=(const Item& item) const
				{
					return hash >= item.hash;
				}

				
				[[nodiscard]] HashType Hash() const
				{
					return hash;
				}

				[[nodiscard]] bool IsVaild() const
				{
					
					return std::visit([](auto& weakPtr) { return !weakPtr.expired(); }, weakPtrVarients);
				}

				template<typename Func>
				void OnItem(Func&& func) const
				{
					std::visit(func, weakPtrVarients);
				}
				template<typename Func>
				void OnItem(Func&& func)
				{
					std::visit(func, weakPtrVarients);
				}

				template<typename T>
				Ref<T> Get() const
				{
					static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");

					if (Weak<T>* weakobjetPtr = std::get_if<Weak<T>>(&weakPtrVarients))
						return weakobjetPtr->lock();
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
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");

				Item item(valuePtr);
			
				RY_CORE_ASSERT(item.IsVaild(), "value is not vaild");


				const ContainerIt end = m_Container.end();
				const ContainerIt begin = m_Container.begin();

				const ContainerIt pos = std::lower_bound(begin, end, item);

				if (pos != end && pos->Hash() == item.Hash())
				{
					RY_CORE_ERROR("valuePtr is already in container!");
					return std::numeric_limits<SizeType>::max();
				}
				DifernzType index = ContainerIt() != pos ? pos - begin : 0;
				RY_CORE_ASSERT(0 <= index && index <= m_Container.size(), "index is negative and (not to be Positive)!");
				m_Container.insert(pos, item);
				
				SizeType indexSize = static_cast<SizeType>(index);

				
				return indexSize;
			}

			template<typename T>
			ContainerIt Finde(const Item& item)
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");
				return std::lower_bound(m_Container.begin(), m_Container.end(), item);
			}

			template<typename T>
			ContainerIt Finde(T* valuePtr)
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");
				Item item = Item(valuePtr);
				RY_CORE_ASSERT(item.IsVaild(), "tried to set a Nullptr Value!");

				return Finde(item);
			}

			template<typename T>
			bool Has(T* valuePtr) const
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");

				Item hasItem = Item(valuePtr);
				bool result = std::binary_search(m_Container.begin(), m_Container.end(), hasItem);
				return result;
			}

			template<typename Func>
			void OnValue(SizeType index, Func&& func) const
			{
				SizeType count = m_Container.size();
				RY_CORE_ASSERT(index < count, "tried to set a Nullptr Value!");
				const Item& item = m_Container.at(index);
				item.OnItem(func);
			}

			template<typename Func>
			void OnValue(SizeType index, Func&& func)
			{
				SizeType count = m_Container.size();
				RY_CORE_ASSERT(index < count, "tried to set a Nullptr Value!");
				Item& item = m_Container.at(index);
				item.OnItem(func);
			}

			template<typename T>
			Ref<T> Get(SizeType index) const
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");

				SizeType count = m_Container.size();
				RY_CORE_ASSERT(index < count, "tried to set a Nullptr Value!");
				const Item& item = m_Container.at(index);
				Ref<T> valueRef;
				valueRef = item.template Get<T>();
				return valueRef;
			}

			template<typename T>
			Ref<T> Get(SizeType index)
			{
				static_assert(is_one_of_v<T, Args...>, "Typ T need to exist in Template-Typeliste");


				SizeType count = m_Container.size();
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
				if (!hasItem.IsVaild())
				{
					RY_CORE_WARN("we removed a already Destroyd Value from list! is no longer vaild");
				}
				ContainerIt end = m_Container.end();
				ContainerIt pos = std::lower_bound(m_Container.begin(), end, hasItem);
				if (pos != end)
					m_Container.erase(pos);
				else
					RY_CORE_ERROR("we removed a already Destroyd Value from list! not found");
				
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

			void FindeNullptrAndRemove()
			{				
				using RitConst = typename Container::const_reverse_iterator;
				using ItConst = typename Container::const_iterator;

				int i = m_Container.size() - 1;
				ItConst begin = m_Container.begin();
				for (int i = m_Container.size() - 1; 0 <= i ; i--)
				{
					const Item& item = m_Container.at(i);
					if (item.IsVaild())
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

			// in next forech loop we skip exeution for that value if it exist it delet him self automaticly
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
