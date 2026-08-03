#pragma once
#include <rypch.h>
namespace Rynex {

	template<typename _Key, typename T, typename _Unit = uint32_t>
	class MapVectorElementPtr;

	template<typename T, size_t _Size, typename _Unit = uint32_t>
	class ArrayElementPtr;

	template<typename T, typename _Unit = uint32_t, typename _StoreStruct = std::vector<typename T>>
	class ElementVecPtr
	{
	public:
		
		using ChangeFunc = typename std::function<void(typename _Unit)>;

		using StoreStruct = typename std::vector<typename T>;
		using RefStoreStruct = typename Ref<typename StoreStruct>;

	public:
		ElementVecPtr() = delete;
		ElementVecPtr(RefStoreStruct vecPtr, _Unit index, const std::function<void(_Unit index)>& destroyFunc)
			: m_StoreStructPtr(vecPtr)
			, m_Index(index)
			, m_DestroyFunc(destroyFunc)
			, m_ChangeValueFunc()
		{
		}

		~ElementVecPtr()
		{
			if (nullptr != m_DestroyFunc)
			{
				m_DestroyFunc(m_Index);
			}
			Clear();
		}

		template<typename Func>
		void AddChangeFunc(const Func& changeFunc)
		{
			m_ChangeValueFunc.push_back(changeFunc);
		}

		_Unit GetIndex() const 
		{ 
			RY_CORE_ASSERT(nullptr != m_StoreStructPtr && -1 != m_Index);
			return m_Index; 
		}

		T& GetValueS() 
		{ 
			RY_CORE_ASSERT(nullptr != m_StoreStructPtr && -1 != m_Index);
			if (nullptr != m_ChangeValueFunc)
			{

				for (const std::function<void(_Unit index)>& func : m_ChangeValueFunc)
				{
					func(m_Index);
				}

			}
			return m_StoreStructPtr->at(m_Index); 
		}

		void SetValueS(const T& v) 
		{ 
			RY_CORE_ASSERT(nullptr != m_StoreStructPtr && -1 != m_Index); 
			if (nullptr != m_ChangeValueFunc)
			{
				for (const std::function<void(_Unit index)>& func : m_ChangeValueFunc)
				{
					func(m_Index);
				}

			}
			m_StoreStructPtr->at(m_Index) = v; 
		}

		T& GetValue()
		{
			RY_CORE_ASSERT(nullptr != m_StoreStructPtr && -1 != m_Index);

			for (const std::function<void(_Unit index)>& func : m_ChangeValueFunc)
			{
				func(m_Index);
			}

			return m_StoreStructPtr->at(m_Index);
		}

		void SetValue(const T& v)
		{
			RY_CORE_ASSERT(nullptr != m_StoreStructPtr && -1 != m_Index);

			for (const std::function<void(_Unit index)>& func : m_ChangeValueFunc)
			{
				func(m_Index);
			}

			m_StoreStructPtr->at(m_Index) = v;
		}

		const T& GetValue() const 
		{ 
			RY_CORE_ASSERT(nullptr != m_StoreStructPtr && -1 != m_Index); 
			return m_StoreStructPtr->at(m_Index); 
		}

		
	private:
		void SetIndex(_Unit index) 
		{ 
			m_Index = index;
		}
		void SetVecPtr(Ref<StoreStruct> vecPtr)
		{
			m_StoreStructPtr = vecPtr;
		}

		void Clear()
		{
			RY_DESTROY_REF(m_StoreStructPtr);
			m_Index = -1;

			m_DestroyFunc = nullptr;
			for (std::function<void(_Unit index)>& func : m_ChangeValueFunc)
			{
				func = nullptr;
			}
			m_ChangeValueFunc.clear();
		}

		void SetDestoryFunc(const std::function<void(_Unit index)>& destroyFunc)
		{
			m_DestroyFunc = destroyFunc;
		}
	private:
		Ref<StoreStruct> m_StoreStructPtr;
		_Unit m_Index;

		std::vector<std::function<void(_Unit index)>> m_ChangeValueFunc;
		std::function<void(_Unit index)> m_DestroyFunc;

	private:
		friend MapVectorElementPtr;
		friend ArrayElementPtr;
	};

	template<typename _Key, typename T, typename _Unit = uint32_t>
	class MapVectorElementPtr
	{
	public:
		 
		using MapValue = typename Weak<ElementVecPtr<typename T , typename _Unit>>;
		using MapValueRef = typename Ref<ElementVecPtr<typename T, typename _Unit>>;

		using MapKeyType = typename std::unordered_map<typename _Key, typename MapValue>;
		using MapKeyTypeIterartor = typename MapKeyType::iterator;
		using MapKeyTypeIterartor_Const = typename MapKeyType::const_iterator;
		using PairMapKeyType = typename MapKeyType::value_type;
		using _PairMapKeyType = typename std::pair<const typename _Key, typename MapValue>;

		using VectorValueType = typename std::vector<T>;
		using RefVectorValueType = typename Ref<VectorValueType>;
		using VectorValueTypeIterartor = typename VectorValueType::iterator;
		using VectorValueTypeIterartor_Const = typename VectorValueType::const_iterator;
	public:
		MapVectorElementPtr()
			: m_Vector(nullptr)
		{
			m_Vector = CreateRef<std::vector<T>>();
		}

		MapVectorElementPtr(const MapVectorElementPtr&) = delete;

		MapVectorElementPtr(MapVectorElementPtr&& moveMap)
			: m_Map(std::move(moveMap.m_Map))
			, m_Vector(std::move(moveMap.m_Vector))
		{
			std::function<void(_Unit index)> desrtoyFunc = std::bind(&MapVectorElementPtr::RemoveIndexAndEmptyKey, this, std::placeholders::_1);
			for (std::pair<const _Key, Weak<ElementVecPtr< T, _Unit>>> pair : m_Map)
			{
				Weak<ElementVecPtr< T, _Unit>>& element = pair.second;
				Ref<ElementVecPtr< T, _Unit>> elementRef = element.lock();
				RY_CORE_ASSERT(false);
				elementRef->SetDestoryFunc(desrtoyFunc);
			}
		}

		~MapVectorElementPtr()
		{
			Clear();
			m_Vector.reset();
		}


		typename MapKeyTypeIterartor_Const FindKey(const _Key& key) const { return m_Map.find(key); }

		typename MapKeyTypeIterartor BeginKey() { return m_Map.begin(); }
		typename MapKeyTypeIterartor EndeKey() { return m_Map.end(); }

		typename MapKeyTypeIterartor_Const BeginKey() const { return m_Map.begin(); }
		typename MapKeyTypeIterartor_Const EndeKey() const { return m_Map.end(); }

		bool HasKey(const _Key& key) const { return m_Map.find(key) != m_Map.end(); }

		_Unit Size() const { CheckeSize(); return m_Vector->size(); }

		_Unit ByteSize() const { return Size() * sizeof(T); }


		MapValueRef GetElementRef(const _Key& key) const
		{
			RY_CORE_ASSERT(HasKey(key));
			const MapValue& value = m_Map.at(key);
			MapValueRef valueRef = value.lock();
			return valueRef;
		}

		_Unit GetIndex(const _Key& key)const
		{
			MapValueRef valueRef = GetElementRef(key);
			RY_CORE_ASSERT(nullptr != valueRef);

			_Unit index = valueRef->GetIndex();
			return index;
		}



		void Clear()
		{
			for (const _PairMapKeyType& pairValue : m_Map)
			{
				const MapValue& value = pairValue.second;
				MapValueRef valueRef = value.lock();
				valueRef->Clear();
			}
			m_Map.clear();
			m_Vector->clear();
		}

		const _Key& GetKey(_Unit index) const
		{
			RY_CORE_ASSERT(index < Size(), "Index is equl or higer then Size");
			for (const PairMapKeyType& pair : m_Map)
			{
				const MapValue& mapValue = pair.second;
				MapValueRef valueRef = mapValue.lock();
				RY_CORE_ASSERT(nullptr != valueRef);

				const _Unit& indexVec = valueRef->GetIndex();
				if (index == indexVec)
					return pair.first;
			}
			RY_CORE_ASSERT(false, "Dont Found The Key For Index!");
			return _Key();
		}

		T& GetDataFromIndex(_Unit index) { return m_Vector->at(index); }

		const T& GetDataFromIndex(_Unit index) const { return m_Vector->at(index); }

		T& GetDataFromKey(const _Key& key)
		{
			_Unit index = GetIndex(key);
			T& dataVec = GetDataFromIndex(index);
			return dataVec;
		}

		const T& GetDataFromKey(const _Key& key) const
		{
			_Unit index = GetIndex(key);
			const T& dataVec = GetDataFromIndex(index);
			return dataVec;
		}

#if 0
		MapValueRef AddData(const _Key& key, T& data)
		{
			RY_CORE_ASSERT(!HasKey(key));
			_Unit index = Size();
			m_Vector->emplace_back(data);
			std::function<void(_Unit index)> desrtoyFunc = std::bind(&MapVectorElementWeak::RemoveIndexAndEmptyKey, this, std::placeholders::_1);
			MapValueRef e = CreateRef<ElementMapVectorElementWeak<T, _Unit>>(m_Vector, index, desrtoyFunc);
			m_Map[key] = e;
			
			CheckeSize();
			return e;
		}
#endif
		MapValueRef AddData(const _Key& key, const T& data)
		{
			RY_CORE_ASSERT(!HasKey(key));
			_Unit index = Size();
			m_Vector->emplace_back(data);
			std::function<void(_Unit index)> desrtoyFunc = std::bind(&MapVectorElementPtr::RemoveIndexAndEmptyKey, this, std::placeholders::_1);

			MapValueRef e = CreateRef<ElementVecPtr<T, _Unit>>(m_Vector, index, desrtoyFunc);
			m_Map[key] = e;

			CheckeSize();
			return e;
		}

		void ChangeKey(const _Key& fromKey, const _Key& toKey)
		{
			RY_CORE_ASSERT(!HasKey(toKey) && HasKey(fromKey));
			MapValue e = m_Map.at(fromKey);

			m_Map.erase(fromKey);
			m_Map[toKey] = e;
			CheckeSize();
		}

		MapValueRef SetData(const _Key& key, const T& data)
		{
			if (HasKey(key))
			{
				MapValueRef value = GetElementRef(key);
				_Unit index = value->GetIndex();
				T& dataVec = m_Vector->at(index);
				dataVec = data;
				return value;
			}
			else
			{
				MapValueRef value = AddData(key, data);
				return value;
			}
		}


		MapValueRef GetKeyMapValue(const _Key& key)
		{
			if (HasKey(key))
			{
				MapValueRef value = GetElementRef(key);
				return value;
			}
			else
			{
				T data = T();
				MapValueRef value = AddData(key, data);
				return value;
			}
		}

		template<typename ... Args>
		T& GetKeyMapValueInit(const _Key& key, Args&& ... args)
		{
			if (HasKey(key))
			{
				MapValueRef value = GetElementRef(key);
				return value;
			}
			else
			{
				T data = T(std::forward<Args>(args)...);
				MapValueRef value = AddData(key, data);
				return value;
			}
		}


		void EraseFromKey(_Key key)
		{
			RY_CORE_ASSERT(HasKey(key));
			MapValueRef value = GetElementRef(key);

			RY_CORE_ASSERT(nullptr != value);
			_Unit index = value->GetIndex();
			value->Clear();
			RY_DESTROY_REF(value);

			typename VectorValueTypeIterartor_Const it = m_Vector->begin() + index;
			m_Map.erase(key);
			m_Vector->erase(it);
			for (PairMapKeyType& pair : m_Map)
			{
				MapValue& value = pair.second;
				MapValueRef valueRef = value.lock();
				_Unit indexVec = valueRef->GetIndex();
				if (index < indexVec)
				{
					indexVec--;
					valueRef->SetIndex(indexVec);
				}
			}
			RY_CORE_ASSERT(!HasKey(key), "Key Exist, shoud by destoyed!");
		}

		

		void EraseFromIndex(_Unit index)
		{
			const _Key& key = GetKey(index);
			EraseFromIndexKey(index, key);
		}

		typename VectorValueTypeIterartor begin()
		{
			return m_Vector->begin();
		}

		typename VectorValueTypeIterartor end()
		{
			return m_Vector->end();
		}

		typename VectorValueTypeIterartor_Const begin() const
		{
			return m_Vector->begin();
		}

		typename VectorValueTypeIterartor_Const end()const
		{
			return m_Vector->end();
		}

		const MapKeyType& GetMap()const { return m_Map; }
		MapKeyType& GetMap() { return m_Map; }


		const RefVectorValueType& GetRefVector() const { return m_Vector; }
		RefVectorValueType& GetRefVector() { return m_Vector; }

		const VectorValueType& GetVector() const { return *m_Vector; }
		VectorValueType& GetVector() { return *m_Vector; }


		const T* Data() const { return m_Vector->data(); }
		T* Data() { return  m_Vector->data(); }

		bool Empty() const { return m_Map.empty() && m_Vector->empty(); }
	private:
		void EraseFromIndexKey(_Unit index, _Key key)
		{
			RY_CORE_ASSERT(HasKey(key));
			MapValueRef value = GetElementRef(key);

			if (nullptr != value)
			{
				value->Clear();
			}
			typename VectorValueTypeIterartor_Const it = m_Vector->begin() + index;
			m_Map.erase(key);
			m_Vector->erase(it);
			for (PairMapKeyType& pair : m_Map)
			{
				MapValue& value = pair.second;
				MapValueRef valueRef = value.lock();
				_Unit indexVec = valueRef->GetIndex();
				if (index < indexVec)
				{
					indexVec--;
					valueRef->SetIndex(indexVec);
				}
			}
			RY_CORE_ASSERT(!HasKey(key), "Key Exist, shoud by destoyed!");
		}

		void RemoveIndexAndEmptyKey(_Unit index)
		{

			typename VectorValueTypeIterartor_Const it = m_Vector->begin() + index;
			m_Vector->erase(it);

			RemoveEmptyKeys();

			for (PairMapKeyType& pair : m_Map)
			{
				MapValue& value = pair.second;
				MapValueRef valueRef = value.lock();
				_Unit indexVec = valueRef->GetIndex();
				if (index < indexVec)
				{
					indexVec--;
					valueRef->SetIndex(indexVec);
				}
			}

			CheckeSize();
		}

		void RemoveEmptyKeys()
		{
			std::vector<_Key> remvoeKeysVec;
			for (const PairMapKeyType& pair : m_Map)
			{
				const MapValue& mapValue = pair.second;
				const _Key& key = pair.first;
				MapValueRef valueRef = mapValue.lock();
				if (valueRef == nullptr)
				{
					remvoeKeysVec.push_back(key);
				}
			}

			for (const _Key& key : remvoeKeysVec)
			{
				m_Map.erase(key);
			}

		}
	
		void CheckeSize() const
		{
			uint32_t sizeMap = m_Map.size();
			uint32_t sizeVec = m_Vector->size();
			RY_CORE_ASSERT(sizeVec == sizeMap, "Not Equal Size");
		}
	private:
		MapKeyType m_Map;
		RefVectorValueType m_Vector;
	};

	
}