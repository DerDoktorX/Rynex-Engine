#pragma once
#include <rypch.h>

namespace Rynex {

	template<typename _Key, typename T, typename _Unit = uint32_t>
	class MapVector
	{
	public:
		using MapKeyType = std::unordered_map< _Key, _Unit>;
		using MapKeyTypeIterartor = typename MapKeyType::iterator;
		using MapKeyTypeIterartor_Const = typename MapKeyType::const_iterator;
		using PairMapKeyType = typename MapKeyType::value_type;
		using _PairMapKeyType = std::pair<_Key, _Unit>;

		using VectorValueType = std::vector<T>;
		using VectorValueTypeIterartor = typename VectorValueType::iterator;
		using VectorValueTypeIterartor_Const = typename VectorValueType::const_iterator;
	// --- public member funktion ---------------------------------------------------------------------------------------------
		MapVector()
		{
		}

		~MapVector()
		{
			Clear();
		}


		MapKeyTypeIterartor_Const FindKey(const _Key& key) const { return m_Map.find(key); }

		MapKeyTypeIterartor BeginKey() { return m_Map.begin(); }
		MapKeyTypeIterartor EndeKey() { return m_Map.end(); }

		MapKeyTypeIterartor_Const BeginKey() const { return m_Map.begin(); }
		MapKeyTypeIterartor_Const EndeKey() const { return m_Map.end(); }
		
		bool HasKey(const _Key& key) const { return m_Map.find(key) != m_Map.end(); }

		_Unit Size() const { CheckeSize(); return m_Vector.size(); }

		_Unit ByteSize() const { return Size() * sizeof(T); }

		_Unit GetIndex(const _Key& key) const
		{
			// const PairMapKeyType& pair = m_Map.at(key); 
			const _Unit& index = m_Map.at(key);
			return index;
		}

		void Clear()
		{
			m_Map.clear();
			m_Vector.clear();
		}

		const _Key& GetKey(_Unit index) const
		{
			RY_CORE_ASSERT(index < Size(), "Index is equl or higer then Size");
			for (const PairMapKeyType& pair : m_Map)
			{
				const _Unit& indexVec = pair.second;
				if (index == indexVec)
					return pair.first;
			}
			RY_CORE_ASSERT(false, "Dont Found The Key For Index!");
			return _Key();
		}

		T& GetDataFromIndex(_Unit index) { return m_Vector.at(index); }

		const T& GetDataFromIndex(_Unit index) const { return m_Vector.at(index); }

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



		T& AddData(const _Key& key, T& data)
		{
			_Unit index = Size();
			T& dataVec = m_Vector.emplace_back( data );
			m_Map[key] = index;
			CheckeSize();
			return dataVec;
		}

		

		T& AddData(const _Key& key, const T& data)
		{
			_Unit index = Size();
			T& dataVec = m_Vector.emplace_back<T>(T{ data });
			m_Map[key] = index;
			CheckeSize();
			return dataVec;
		}
		
		_Unit AddDataIndex(const _Key& key, T& data)
		{
			_Unit index = Size();
			m_Vector.emplace_back(data);
			m_Map[key] = index;
			CheckeSize();
			return index;
		}

		_Unit AddDataIndex(const _Key& key, const T& data)
		{
			_Unit index = Size();
			m_Vector.emplace_back(data);
			m_Map[key] = index;
			CheckeSize();
			return index;
		}

		void ChangeKey(const _Key& fromKey, const _Key& toKey)
		{
			RY_CORE_ASSERT(!HasKey(toKey) && HasKey(fromKey));
			_Unit index = m_Map.at(fromKey);
			m_Map.erase(fromKey);
			m_Map[toKey] = index;
			CheckeSize();
		}

		T& SetData(const _Key& key, T& data)
		{
			if (HasKey(key))
			{
				T& dataVec = GetDataFromKey(key);
				dataVec = data;
				return dataVec;
			}
			else
			{				
				T& dataVec = AddData(key, data);
				return dataVec;
			}
		}

		T& SetData(const _Key& key, const T& data)
		{
			if (HasKey(key))
			{
				T& dataVec = GetDataFromKey(key);
				dataVec = data;
				return dataVec;
			}
			else
			{
				T& dataVec = AddData(key, data);
				return dataVec;
			}
		}

		_Unit SetDataIndex(const _Key& key, T& data)
		{
			if (HasKey(key))
			{
				_Unit index = GetIndex(key);
				T& dataVec = GetDataFromIndex(index);
				dataVec = data;
				return index;
			}
			else
			{
				MapKeyType index = AddDataIndex(key, data);
				return index;
			}
		}

		_Unit SetDataIndex(const _Key& key, const T& data)
		{
			if (HasKey(key))
			{
				_Unit index = GetIndex(key);
				T& dataVec = GetDataFromIndex(index);
				dataVec = data;
				return index;
			}
			else
			{
				_Unit index = AddDataIndex(key, data);
				return index;
			}
		}

		T& GetKeyData(const _Key& key)
		{
			if (HasKey(key))
			{
				T& dataVec = GetDataFromKey(key);
				return dataVec;
			}
			else
			{
				T data = T();
				T& dataVec = AddData(key, data);
				return dataVec;
			}
		}

		template<typename ... Args>
		T& GetKeyDataInit(const _Key& key, Args&& ... args)
		{
			if (HasKey(key))
			{
				T& dataVec = GetDataFromKey(key);
				return dataVec;
			}
			else
			{
				T data = T(std::forward<Args>(args)...);
				T& dataVec = AddData(key, data);
				return dataVec;
			}
		}

		void AddNewKey(const _Key& key, T& data)
		{
			if (!HasKey(key))
			{
				AddData(key, data);
			}
		}

		void AddNewKey(const _Key& key, const T& data)
		{
			if (!HasKey(key))
			{
				AddData(key, data);
			}
		}

		void EraseFromKey(_Key key)
		{
			RY_CORE_ASSERT(HasKey(key));
			_Unit index = GetIndex(key);
			VectorValueTypeIterartor_Const it = begin() + index;
			m_Vector.erase(it);
			m_Map.erase(key);
			for (PairMapKeyType& pair : m_Map)
			{
				uint32_t& indexVec = pair.second;
				if (index < indexVec)
					indexVec--;
			}
			RY_CORE_ASSERT(!HasKey(key), "Key Exíst, shoud by destoyed!");
		}

		void EraseFromIndex(_Unit index)
		{
			const _Key& key = GetKey(index);
			EraseFromKey(key);
		}

		VectorValueTypeIterartor begin()
		{
			return m_Vector.begin();
		}

		VectorValueTypeIterartor end()
		{
			return m_Vector.end();
		}

		VectorValueTypeIterartor_Const begin() const
		{
			return m_Vector.begin();
		}

		VectorValueTypeIterartor_Const end()const
		{
			return m_Vector.end();
		}

		const MapKeyType& GetMap()const { return m_Map; }
		MapKeyType& GetMap() { return m_Map; }


		const VectorValueType& GetVector() const { return m_Vector; }
		VectorValueType& GetVector() { return m_Vector; }

		const T* Data() const { return m_Vector.data(); }
		T* Data() { return  m_Vector.data(); }

		bool Empty() const { return m_Map.empty() && m_Vector.empty(); }
	private:
		void CheckeSize() const
		{
			uint32_t sizeMap = m_Map.size();
			uint32_t sizeVec = m_Vector.size();
			RY_CORE_ASSERT(sizeVec == sizeMap, "Not Equal Size");
		}
	// --- private member varibles --------------------------------------------------------------------------------------------
		MapKeyType m_Map;
		VectorValueType m_Vector;
	};


	

#if 0

#define RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY(obj, typeResult, func)							\
	int64_t key = m_GetKeyFromPtrFunc(obj);														\
	typeResult result = _MapVectorRef::func(key);												\
	return result

#define RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_VOID(obj, func)									\
	int64_t key = m_GetKeyFromPtrFunc(obj);														\
	_MapVectorRef::func(key)


#define RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(obj, typeResult, func)					\
	int64_t key = m_GetKeyFromPtrFunc(obj);														\
	typeResult result = _MapVectorRef::func(key, obj);											\
	return result


	template<typename T>																					\
	class MapVectorRef : public MapVector<int64_t, typename Ref<typename T>>
	{																										\
	private:																								\
		using _MapVectorRef = MapVector<int64_t, typename Ref<typename T>>;									\
		using _T = typename Ref<typename T>;																\
	public:																									\
		MapVectorRef()																						
		{																								
		}
			
		~MapVectorRef()
		{																								
		}																								

		bool HasObject(const _T& object) const														
		{																								
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY(object, bool, HasKey);											
		}																								
			
		uint32_t GetIndex(const _T& object)															
		{																							
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY(object, uint32_t, GetIndex);									
		}																								
		_T& GetObjectDataFromObject(const _T& object)												
		{																								
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY(object, _T&, GetDataFromKey);									
		}																								
			
		_T& AddObject(_T& object)
		{																								
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, _T&, AddData);									
		}																				
			
		_T& AddObject(const _T& object)																
		{																								
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, _T&, AddData);									
		}																								
			
		_T& SetObject(_T& object)																	
		{																								
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, _T&, SetData);
		}																								
			
		_T& SetObject(const _T& object)																
		{																								
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, _T&, SetData);
		}																								
			
		void EraseFromObject(const T& object)														
		{																								
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_VOID(object, EraseFromKey);									
		}																								
	private:																						
		static int64_t GetKeyFromPtr(const _T& object)													
		{			
			T* objectPtr = object.get();																
			int64_t objectKey = (int64_t)objectPtr;														
			return objectKey;																			
		}	
	};


#else


#define RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY(obj, typeResult, func)							\
	int64_t key = m_GetKeyFromPtrFunc(obj);														\
	typeResult result = _MapVectorObjectPtr::func(key);											\
	return result

#define RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_VOID(obj, func)									\
	int64_t key = m_GetKeyFromPtrFunc(obj);														\
	_MapVectorObjectPtr::func(key)


#define RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(obj, typeResult, func)					\
	int64_t key = m_GetKeyFromPtrFunc(obj);														\
	typeResult result = _MapVectorObjectPtr::func(key, obj);									\
	return result


	template<typename _T>
	class MapVectorObjectPtr : public MapVector<int64_t, _T>
	{
	private:
		using _MapVectorObjectPtr = typename MapVector<int64_t, typename _T>;
	public:		

		MapVectorObjectPtr(std::function<int64_t(const _T&)> getKeyFromPtrFunc)
			: m_GetKeyFromPtrFunc(getKeyFromPtrFunc)
		{
		}

		~MapVectorObjectPtr()
		{
		}

		bool HasObject(const _T& object) const
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY(object, bool, HasKey);
		}

		uint32_t GetObjectIndex(const _T& object) const
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY(object, uint32_t, GetIndex);
		}
		_T& GetObjectDataFromObject(const _T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY(object, _T&, GetDataFromKey);
		}


		_T& AddObject(_T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, _T&, AddData);
		}

		_T& AddObject(const _T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, _T&, AddData);
		}

		uint32_t AddObjectIndex(_T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, uint32_t, AddDataIndex);
		}

		uint32_t AddObjectIndex(const _T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, uint32_t, AddDataIndex);
		}


		_T& SetObject(_T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, _T&, SetData);
		}

		_T& SetObject(const _T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, _T&, SetData);
		}

		uint32_t SetObjectIndex(_T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, uint32_t, SetDataIndex);
		}

		uint32_t SetObjectIndex(const _T& object)
		{
			RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT(object, uint32_t, SetDataIndex);
		}

		void EraseFromObject(const _T& object)
		{
			int64_t key = m_GetKeyFromPtrFunc(object); _MapVectorObjectPtr::EraseFromKey(key);
		}

		void ChangeKeyObject(const _T& fromObject, const _T& toObject)
		{
			int64_t fromKey = m_GetKeyFromPtrFunc(fromObject);
			int64_t toKey = m_GetKeyFromPtrFunc(toObject);
			_MapVectorObjectPtr::ChangeKey(fromKey, toKey);
		}
	private:
		std::function<int64_t(const _T&)> m_GetKeyFromPtrFunc;
	};


#undef RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY
#undef RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_VOID
#undef RY_INTERNALE_MAP_VECTOR_OBJECT_PTR_KEY_OBJECT

	template<typename T>
	class MapVectorRef : public MapVectorObjectPtr<typename Ref<typename T>>
	{
	private:
		using _ObjectPtr = typename Ref<typename T>;
		using _MapVectorObjectPtr = typename MapVectorObjectPtr<typename _ObjectPtr>;
	public:
		inline MapVectorRef()
			: _MapVectorObjectPtr(MapVectorRef::GetKeyFromPtr)
		{
		}

		~MapVectorRef()
		{
		}

	// --- public static funktion ---------------------------------------------------------------------------------------------
		static int64_t GetKeyFromPtr(const _ObjectPtr& object)
		{
			T* objectPtr = object.get();
			int64_t objectKey = (int64_t)objectPtr;
			return objectKey;
		}

		
	};


	template<typename T>
	class MapVectorWeak : public MapVectorObjectPtr<typename Weak<typename T>>
	{
	private:
		using _ObjectPtr= typename Weak<typename T>;
		using _MapVectorObjectPtr = typename MapVectorObjectPtr<typename _ObjectPtr>;
	public:
		MapVectorWeak()
			: _MapVectorObjectPtr(GetKeyFromPtr)
		{
		}

		~MapVectorWeak()
		{
		}

	// --- public static funktion ---------------------------------------------------------------------------------------------
		static int64_t GetKeyFromPtr(const _ObjectPtr& object)
		{
			if(Ref<T> objectRef = object.lock())
			{
				T* objectPtr = objectRef.get();
				int64_t objectKey = reinterpret_cast<int64_t>(objectPtr);
				return objectKey;
			}
			RY_CORE_FATAL("We have a not vaild Nullptr! In This MapVectorWeak!");
			return 0;
		}

		
	};

	
	template<typename T>
	class MapVectorScope : public MapVectorObjectPtr<typename Scope<typename T>>
	{
	private:
		using _ObjectPtr = typename Scope<typename T>;
		using _MapVectorObjectPtr = typename MapVectorObjectPtr<typename _ObjectPtr>;
	public:
		MapVectorScope()
			: _MapVectorObjectPtr(GetKeyFromPtr)
		{
		}

		~MapVectorScope()
		{
		}

	// --- public static funktion ---------------------------------------------------------------------------------------------
		static int64_t GetKeyFromPtr(const _ObjectPtr& object)
		{
			T* objectPtr = object.get();
			int64_t objectKey = static_cast<int64_t>(objectPtr);
			return objectKey;
		}
	};
#endif
}