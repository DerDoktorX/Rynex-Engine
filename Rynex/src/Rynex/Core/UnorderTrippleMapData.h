#pragma once
#include <rypch.h>

namespace Rynex {

	template<typename _Key1, typename _Key2, typename _Key3>
	class UnorderTrippleMapData
	{
	public:
		struct DataInfo3
		{
			uint32_t Index;
			_Key2 Parent2;
			uint32_t Parent1Index;
			uint32_t Parent2Index;
		};

		struct DataInfo2
		{
			uint32_t ThisIndex2;
			_Key1 Parent1;
			std::vector<typename _Key3> ChilderenKey3;
			glm::uvec2 RangeChild3;
			void PushBack(const _Key3& key3)
			{
				ChilderenKey3.push_back(key3);
			};



			void Erase(const _Key3& key3)
			{
				uint32_t index = Find(key3);
				if (MaxSize() != index)
					ChilderenKey3.erase(ChilderenKey3.begin() + index);
			};

			uint32_t Find(const _Key3& key3)const
			{
				uint32_t index = 0;
				for (const _Key3& key3Element : ChilderenKey3)
				{
					if (key3 == key3Element)
						return index;
					index++;

				}
				RY_CORE_ASSERT(false);
				return MaxSize();
			};

			bool Has(const _Key3& key3)const
			{
				for (const _Key3& key3Element : ChilderenKey3)
				{
					if (key3 == key3Element)
						return true;
				}
				return false;
			};

			static uint32_t MaxSize()
			{
				return static_cast<uint32_t>(-1);
			};

			void Clear()
			{
				ChilderenKey3.clear();
			};

			uint32_t Size()const
			{
				return ChilderenKey3.size();
			};


		};

		struct DataInfo1
		{
			uint32_t ThisIndex1;
			std::vector<typename _Key2> ChilderenKey2;
			glm::uvec2 RangeChild2;

			void PushBack(const _Key2& key2)
			{
				ChilderenKey2.push_back(key2);
			};

			void Erase(const _Key2& key2)
			{
				uint32_t index = Find(key2);
				if (MaxSize() != index)
					ChilderenKey2.erase(ChilderenKey2.begin() + index);
			};

			uint32_t Find(const _Key2& key2)const
			{
				uint32_t index = 0;
				for (const _Key2& key2Element : ChilderenKey2)
				{
					if (key2 == key2Element)
						return index;
					index++;

				}
				RY_CORE_ASSERT(false);
				return MaxSize();
			};

			bool Has(const _Key2& key2)const
			{
				for (const _Key2& key2Element : ChilderenKey2)
				{
					if (key2 == key2Element)
						return true;
				}
				return false;
			};

			static uint32_t MaxSize()
			{
				return static_cast<uint32_t>(-1);
			};

			void Clear()
			{
				ChilderenKey2.clear();
			};

			uint32_t Size()const
			{
				return ChilderenKey2.size();
			}



		};

		using FirstMapType = typename std::unordered_map<typename _Key1, typename DataInfo1>;
		using SecondMapType = typename std::unordered_map<typename _Key2, typename DataInfo2>;
		using ThiredMapType = typename std::unordered_map<typename _Key3, typename DataInfo3>;

		using FirstPair = typename FirstMapType::value_type;
		using SecondPair = typename SecondMapType::value_type;
		using ThiredPair = typename ThiredMapType::value_type;

		constexpr UnorderTrippleMapData()
			: m_FirstMap()
			, m_SecoundMap()
			, m_ThiredMap()

			, m_OnClearFunc(nullptr)

			, m_OnEraseDestroyInfo1Func(nullptr)
			, m_OnEraseDestroyInfo2Func(nullptr)
			, m_OnEraseDestroyInfo3Func(nullptr)
			, m_OnInserteCreateInfo1Func(nullptr)
			, m_OnInserteCreateInfo2Func(nullptr)
			, m_OnInserteCreateInfo3Func(nullptr)
		{
		};

		constexpr UnorderTrippleMapData(
			std::function<void()> onClearFunc

			, std::function<void(const DataInfo1&, const _Key1&)> onEraseDestroyInfo1Func
			, std::function<void(const DataInfo2&, const _Key2&)> onEraseDestroyInfo2Func
			, std::function<void(const DataInfo3&, const _Key3&)> onEraseDestroyInfo3Func


			, std::function<void(const DataInfo1&, const _Key1&)> onInserteCreateInfo1Func
			, std::function<void(const DataInfo2&, const _Key2&)> onInserteCreateInfo2Func
			, std::function<void(const DataInfo3&, const _Key3&)> onInserteCreateInfo3Func

			, std::function<void(const DataInfo1&, const _Key1&, uint32_t, uint32_t)> onChangeIndexRangeInfo1Func
			, std::function<void(const DataInfo2&, const _Key2&, uint32_t, uint32_t)> onChangeIndexRangeInfo2Func
			, std::function<void(const DataInfo3&, const _Key3&, uint32_t)> onChangeIndexRangeInfo3Func

		)
			: m_FirstMap()
			, m_SecoundMap()
			, m_ThiredMap()

			, m_OnClearFunc(onClearFunc)

			, m_OnEraseDestroyInfo1Func(onEraseDestroyInfo1Func)
			, m_OnEraseDestroyInfo2Func(onEraseDestroyInfo2Func)
			, m_OnEraseDestroyInfo3Func(onEraseDestroyInfo3Func)

			, m_OnInserteCreateInfo1Func(onInserteCreateInfo1Func)
			, m_OnInserteCreateInfo2Func(onInserteCreateInfo2Func)
			, m_OnInserteCreateInfo3Func(onInserteCreateInfo3Func)

			, m_OnChangeIndexRangeInfo1Func(onChangeIndexRangeInfo1Func)
			, m_OnChangeIndexRangeInfo2Func(onChangeIndexRangeInfo2Func)
			, m_OnChangeIndexRangeInfo3Func(onChangeIndexRangeInfo3Func)

		{
		};

		~UnorderTrippleMapData()
		{
			Clear();
			m_OnClearFunc = nullptr;
		};

		bool HasKey1(const _Key1& key) const { return m_FirstMap.find(key) != m_FirstMap.end(); };
		bool HasKey2(const _Key2& key) const { return m_SecoundMap.find(key) != m_SecoundMap.end(); };
		bool HasKey3(const _Key3& key) const { return m_ThiredMap.find(key) != m_ThiredMap.end(); };

		const DataInfo1& GetInfo1(const _Key1& key1) const
		{
			const DataInfo1& info1 = m_FirstMap.at(key1);
			return info1;
		};

		DataInfo1& GetInfo1(const _Key1& key1)
		{
			DataInfo1& info1 = m_FirstMap.at(key1);
			return info1;
		};

		const DataInfo2& GetInfo2(const _Key2& key2) const
		{
			const DataInfo2& info2 = m_SecoundMap.at(key2);
			return info2;
		};

		DataInfo2& GetInfo2(const _Key2& key2)
		{
			DataInfo2& info2 = m_SecoundMap.at(key2);
			return info2;
		};

		const DataInfo3& GetInfo3(const _Key3& key3) const
		{
			const DataInfo3& info3 = m_ThiredMap.at(key3);
			return info3;
		};

		DataInfo3& GetInfo3(const _Key3& key3)
		{
			DataInfo3& info3 = m_ThiredMap.at(key3);
			return info3;
		};


		uint32_t GetSize1(const _Key1& key1) const
		{
			const DataInfo1& info1 = GetInfo1(key1);
			const std::vector<typename _Key2>& data = info1.ChilderenKey;
			uint32_t size = data.size();
			return size;
		};

		uint32_t GetSize2(const _Key2& key2) const
		{
			const DataInfo2& info2 = GetInfo2(key2);
			const std::vector<typename _Key2>& data = info2.ChilderenKey;
			uint32_t size = data.size();
			return size;
		};


		DataInfo3& Inserte3(const _Key1& key1, const _Key2& key2, const _Key3& key3)
		{
			RY_PROFILE_FUNCTION();
			bool hasKey1 = HasKey1(key1);
			bool hasKey2 = HasKey2(key2);
			bool hasKey3 = HasKey3(key3);
			if (hasKey1 && hasKey2 && !hasKey3)
				return OnInsertNewKey3(key1, key2, key3);
			else if (hasKey1 && !hasKey2 && !hasKey3)
				return OnInsertNewKey2Key3(key1, key2, key3);
			else if (!hasKey1 && !hasKey2 && !hasKey3)
				return OnInsertNewKey1Key2Key3(key1, key2, key3);
			else if (hasKey1 && hasKey2 && hasKey3)
			{
				DataInfo3& info3 = GetInfo3(key3);
				RY_CORE_ASSERT(info3.Parent2 == key2, "The Same Key Used on Another Parent!");
				DataInfo2& info2 = GetInfo2(key2);
				RY_CORE_ASSERT(info2.Parent1 == key1, "The Same Key Used on Another Parent!");
				return info3;
			}
			else
			{
				RY_CORE_ASSERT(false, "Not Expexted Error!");
			}
			return DataInfo3{ static_cast<uint32_t>(-1), _Key1() };
		};

		DataInfo2& Inserte2(const _Key1& key1, const _Key2& key2)
		{
			RY_PROFILE_FUNCTION();
			bool hasKey1 = HasKey1(key1);
			bool hasKey2 = HasKey2(key2);
			if (hasKey1 && hasKey2)
			{
				CheckChildrensInfo1(key2);
				DataInfo2& info2 = GetInfo2(key2);
				return info2;
			}
			else if (hasKey1 && !hasKey2)
			{
				uint32_t index2 = m_SecoundMap.size();
				uint32_t index3 = m_ThiredMap.size();

				DataInfo1& info1 = GetInfo1(key1);
				info1.PushBack(key2);

				DataInfo2& info2 = AddCreateInfo2(key1, key2, index2);

				return info2;
			}
			else if (!hasKey1 && !hasKey2)
			{
				uint32_t index1 = m_FirstMap.size();
				uint32_t index2 = m_SecoundMap.size();
				uint32_t index3 = m_ThiredMap.size();

				DataInfo1& info1 = AddCreateInfo1(key1, index1);
				info1.PushBack(key2);

				DataInfo2& info2 = AddCreateInfo2(key1, key2, index2);

				return info2;
			}
			else if (hasKey1 && hasKey2)
			{
				DataInfo3& info3 = GetInfo3(key3);
				RY_CORE_ASSERT(info3.Parent2 == key2, "The Same Key Used on Another Parent!");
				DataInfo2& info2 = GetInfo2(key2);
				RY_CORE_ASSERT(info2.Parent1 == key1, "The Same Key Used on Another Parent!");
				return info3;
			}
			else if (!hasKey1 && hasKey2)
			{
				RY_CORE_ASSERT(false, "The Same Key Used on Some Parent!")
			}
			else
			{
				RY_CORE_ASSERT(false, "Not Expexted Error!");
			}
			return DataInfo3{ static_cast<uint32_t>(-1), _Key1() };
		};

		DataInfo1& Inserte1(const _Key1& key1)
		{
			RY_PROFILE_FUNCTION();
			bool hasKey1 = HasKey1(key1);

			if (!hasKey1)
			{
				uint32_t index1 = m_FirstMap.size();
				DataInfo1& info1 = AddCreateInfo1(key1, index1);

				return info1;
			}
			else (hasKey1)
			{
				DataInfo1& info1 = AddCreateInfo1(key1, index1);
				return info1;
			}
			return DataInfo1{ static_cast<uint32_t>(-1) };
		};

		void Erase3(const _Key3& key3)
		{
			RY_PROFILE_FUNCTION();
			if (!HasKey3(key3))
			{
				RY_CORE_ERROR("ID Not Found Abourt Erase!");
				return;
			}

			DataInfo3& info3 = GetInfo3(key3);
			const uint32_t indexCopy = info3.Index;
			const _Key2& key2 = info3.Parent2;
			DataInfo2& info2 = GetInfo2(key2);

			CheckParentInfo3(key3, key2, info2);

			RemoveDestroyInfo3(key3, info3);

			ChangeIndex3FromList(indexCopy, -1, IsGreaterThen, IsGreaterThen);

			info2.Erase(key3);
			info2.RangeChild3.y--;


		};

		void Erase2(const _Key2& key2)
		{
			RY_PROFILE_FUNCTION();
			if (!HasKey2(key2))
			{
				RY_CORE_ERROR("ID Not Found Abourt Erase!");
				return;
			}
			DataInfo2& info2 = GetInfo2(key2);

			CheckChildrensInfo2(key2);
			CheckParentInfo2(key2, info2);
			const _Key1& key1 = info2.Parent1;
			DataInfo1& info1 = GetInfo1(key1);

			RemoveDestroyInfo2(key2, info2);

			info1.Erase(key2);

		};

		void Erase1(const _Key1& key1)
		{
			RY_PROFILE_FUNCTION();
			if (!HasKey1(key1))
			{
				RY_CORE_ERROR("ID Not Found Abourt Erase!");
				return;
			}
			DataInfo1& info1 = GetInfo1(key1);

			CheckChildrensInfo1(key1, info1);

			RemoveDestroyInfo1(key1, info1);


		};

		void Clear()
		{
			if (m_OnClearFunc)
				m_OnClearFunc();

			m_FirstMap.clear();
			m_SecoundMap.clear();
			m_ThiredMap.clear();
		}

		typename FirstMapType::iterator begin() { return m_FirstMap.begin(); }
		typename FirstMapType::iterator end() { return m_FirstMap.end(); }

		typename FirstMapType::const_iterator begin() const { return m_FirstMap.begin(); }
		typename FirstMapType::const_iterator end() const { return m_FirstMap.end(); }

	private:
		DataInfo1& AddCreateInfo1(const _Key1& key1, const uint32_t index)
		{

			DataInfo1& info1 = m_FirstMap[key1];
			info1.ThisIndex1 = index;
			// info1.RangeChildeChilde3 = MinMaxRange();
			if (m_OnInserteCreateInfo1Func)
				m_OnInserteCreateInfo1Func(info1, key1);
			ChangeIndex1FromList(index, 1, IsGreaterThen);
			return info1;
		}

		DataInfo2& AddCreateInfo2(const _Key1& key1, const _Key2& key2, const uint32_t index)
		{
			DataInfo2& info2 = m_SecoundMap[key2];
			info2.ThisIndex2 = index;
			info2.Parent1 = key1;
			info2.RangeChild3 = MinMaxRange();
			if (m_OnInserteCreateInfo2Func)
				m_OnInserteCreateInfo2Func(info2, key2);

			ChangeIndex2FromList(index, 1, IsGreaterThen);

			return info2;
		}

		DataInfo3& AddCreateInfo3(const _Key2& key2, const _Key3& key3, const uint32_t index)
		{
			ChangeIndex3FromList(index, 1, IsGreaterOrEqualThen, IsGreaterThen);

			DataInfo3& info3 = m_ThiredMap[key3];
			info3.Index = index;
			info3.Parent2 = key2;

			if (m_OnInserteCreateInfo3Func)
				m_OnInserteCreateInfo3Func(info3, key3);


			return info3;
		}


		DataInfo3& OnInsertNewKey3(const _Key1& key1, const _Key2& key2, const _Key3& key3)
		{
			uint32_t index3 = m_ThiredMap.size();

			DataInfo1& info1 = GetInfo1(key1);
			RY_CORE_ASSERT(info1.Has(key2), "Whrong Parent!");




			DataInfo2& info2 = GetInfo2(key2);
			info2.PushBack(key3);

			DataInfo3& info3 = AddCreateInfo3(key2, key3, info2.RangeChild3.y);
			info2.RangeChild3.y++;
			return info3;
		}

		DataInfo3& OnInsertNewKey2Key3(const _Key1& key1, const _Key2& key2, const _Key3& key3)
		{
			uint32_t index2 = m_SecoundMap.size();
			uint32_t index3 = m_ThiredMap.size();

			DataInfo1& info1 = GetInfo1(key1);
			info1.PushBack(key2);

			DataInfo2& info2 = AddCreateInfo2(key1, key2, index2);
			info2.PushBack(key3);
			info2.RangeChild3.x = index3;
			info2.RangeChild3.y = index3;

			DataInfo3& info3 = AddCreateInfo3(key2, key3, info2.RangeChild3.y);
			info2.RangeChild3.y++;
			return info3;
		}

		DataInfo3& OnInsertNewKey1Key2Key3(const _Key1& key1, const _Key2& key2, const _Key3& key3)
		{
			uint32_t index1 = m_FirstMap.size();
			uint32_t index2 = m_SecoundMap.size();
			uint32_t index3 = m_ThiredMap.size();

			DataInfo1& info1 = AddCreateInfo1(key1, index1);
			info1.PushBack(key2);

			DataInfo2& info2 = AddCreateInfo2(key1, key2, index2);
			info2.PushBack(key3);
			info2.RangeChild3.x = index3;
			info2.RangeChild3.y = index3;

			DataInfo3& info3 = AddCreateInfo3(key2, key3, index3);
			info2.RangeChild3.y++;
			return info3;
		}

		DataInfo2& OnInsertNewKey2(const _Key1& key1, const _Key2& key2)
		{
			uint32_t index2 = m_SecoundMap.size();
			uint32_t index3 = m_ThiredMap.size();

			DataInfo1& info1 = GetInfo1(key1);
			info1.PushBack(key2);

			DataInfo2& info2 = AddCreateInfo2(key1, key2, index2);

			return info2;
		}

		DataInfo2& OnInsertNewKey1Key2(const _Key1& key1, const _Key2& key2)
		{
			uint32_t index1 = m_FirstMap.size();
			uint32_t index2 = m_SecoundMap.size();

			DataInfo1& info1 = AddCreateInfo1(key1, index1);
			info1.PushBack(key2);

			DataInfo2& info2 = AddCreateInfo2(key1, key2, index2);

			return info2;
		}


		void ChangeIndex1FromList(uint32_t changeIndex, uint32_t changeBy, std::function<bool(const uint32_t&, const uint32_t&)> equalFunc)
		{
			EcheInfo1([changeIndex, changeBy, equalFunc, this](const _Key1& key1, DataInfo1& info1) {
				uint32_t& index = info1.ThisIndex1;
				if (equalFunc(index, changeIndex))
				{
					index += changeBy;

				}
				if (m_OnChangeIndexRangeInfo1Func)
					m_OnChangeIndexRangeInfo1Func(info1, key1, changeBy, 0);
				});
		}
		void ChangeIndex2FromList(uint32_t changeIndex, uint32_t changeBy, std::function<bool(const uint32_t&, const uint32_t&)> equalFunc)
		{
			EcheInfo2([changeIndex, changeBy, equalFunc, this](const _Key2& key2, DataInfo2& info2) {
				uint32_t& index = info2.ThisIndex2;
				if (equalFunc(index, changeIndex))
				{
					index += changeBy;

				}
				if (m_OnChangeIndexRangeInfo2Func)
					m_OnChangeIndexRangeInfo2Func(info2, key2, changeBy, 0);

				});

		}
		void ChangeIndex3FromList(uint32_t changeIndex, uint32_t changeBy, std::function<bool(const uint32_t&, const uint32_t&)> equalFunc3, std::function<bool(const uint32_t&, const uint32_t&)> equalFunc2)
		{


			EcheInfo2([changeIndex, changeBy, equalFunc2, this](const _Key2& key2, DataInfo2& info2) {
				glm::uvec2& rangeChild = info2.RangeChild3;
				if (equalFunc2(rangeChild.x, changeIndex))
				{
					rangeChild.x += changeBy;
					rangeChild.y += changeBy;
				}
				if (m_OnChangeIndexRangeInfo2Func)
					m_OnChangeIndexRangeInfo2Func(info2, key2, 0, changeBy);
				});

			EcheInfo3([changeIndex, changeBy, equalFunc3, this](const _Key3& key3, DataInfo3& info3) {
				uint32_t& index = info3.Index;
				if (equalFunc3(index, changeIndex))
				{
					index += changeBy;

					if (m_OnChangeIndexRangeInfo3Func)
						m_OnChangeIndexRangeInfo3Func(info3, key3, changeBy);
				}
				else
				{
					if (m_OnChangeIndexRangeInfo3Func)
						m_OnChangeIndexRangeInfo3Func(info3, key3, 0);
				}

				});


		}

		static bool IsGreaterThen(const uint32_t& index, const uint32_t& targetIndex)
		{
			return index > targetIndex;
		}
		static bool IsGreaterOrEqualThen(const uint32_t& index, const uint32_t& targetIndex)
		{
			return index >= targetIndex;
		}
		static bool IsSmallerThen(const uint32_t& index, const uint32_t& targetIndex)
		{
			return index < targetIndex;
		}



		uint32_t RemoveDestroyInfo1(const _Key1& key1, DataInfo1& info1)
		{
			if (m_OnEraseDestroyInfo1Func)
				m_OnEraseDestroyInfo1Func(info1, key1);

			for (const _Key2& key2 : info1.ChilderenKey2)
			{
				RemoveDestroyInfo2(key2);
			}
			info1.Clear();
			const uint32_t index1Copy = info1.ThisIndex1;
			m_FirstMap.erase(key1);


			ChangeIndex1FromList(index1Copy, -1, IsGreaterThen);
			return index1Copy;
		};

		uint32_t RemoveDestroyInfo1(const _Key1& key1)
		{
			DataInfo1& info1 = GetInfo1(key1);
			return RemoveDestroyInfo1(key1, info1);
		};


		uint32_t RemoveDestroyInfo2(const _Key2& key2, DataInfo2& info2)
		{
			if (m_OnEraseDestroyInfo2Func)
				m_OnEraseDestroyInfo2Func(info2, key2);

			for (const _Key3& key3 : info2.ChilderenKey3)
			{
				RemoveDestroyInfo3(key3);
			}
			info2.Clear();
			const uint32_t index2Copy = info2.ThisIndex2;
			m_SecoundMap.erase(key2);

			ChangeIndex2FromList(index2Copy, -1, IsGreaterThen);
			return index2Copy;
		};

		uint32_t RemoveDestroyInfo2(const _Key2& key2)
		{
			DataInfo2& info2 = GetInfo2(key2);
			return RemoveDestroyInfo2(key2, info2);
		};


		uint32_t RemoveDestroyInfo3(const _Key3& key3, DataInfo3& info3)
		{
			if (m_OnEraseDestroyInfo3Func)
				m_OnEraseDestroyInfo3Func(info3, key3);

			const uint32_t index3Copy = info3.Index;
			m_ThiredMap.erase(key3);

			ChangeIndex3FromList(index3Copy, -1, IsGreaterThen, IsGreaterThen);
			return index3Copy;
		};

		uint32_t RemoveDestroyInfo3(const _Key3& key3)
		{
			DataInfo3& info3 = GetInfo3(key3);
			return RemoveDestroyInfo3(key3, info3);
		};

#pragma region CheckFromParentToChildren

		void CheckParentInfo2(const _Key2& key2, const DataInfo2& info2)const
		{
			const _Key1& key1 = info2.Parent1;
			RY_CORE_ASSERT(HasKey1(key1), "ID 1 Dont existed!");
			const DataInfo1& info1 = GetInfo1(key1);
			RY_CORE_ASSERT(info1.Has(key2), "Parent -> Info1 don't has Key2 as Children!");
		};

		void CheckParentInfo2(const _Key2& key2)const
		{
			RY_CORE_ASSERT(HasKey2(key2), "ID 2 Dont existed!");
			const DataInfo2& info2 = GetInfo2(key2);
			CheckParentInfo2(key2, info2);
		}

		void CheckParentInfo3(const _Key3& key3, const _Key2& key2, const DataInfo2& info2) const
		{
			bool result = info2.Has(key3);
			RY_CORE_ASSERT(result, "Parent -> Info2 don't has Key3 as Children!");
			CheckParentInfo2(key2, info2);
		};

		void CheckParentInfo3(const _Key3& key3) const
		{
			RY_CORE_ASSERT(HasKey3(key3), "ID 3 Dont existed!");
			const DataInfo3& info3 = GetInfo3(key3);
			CheckParentInfo3(key3, info3);
		};

		void CheckParentInfo3(const _Key3& key3, const DataInfo3& info3) const
		{
			const _Key2& key2 = info3.Parent2;
			RY_CORE_ASSERT(HasKey2(key2), "ID 2 Dont existed!");
			const DataInfo2& info2 = GetInfo2(key3);
			CheckParentInfo3(key3, key2, info2);
		};

#pragma endregion


#pragma region CheckFromChildrensToParent

		void CheckChildrensInfo1(const _Key1& key1, const DataInfo1& info)const
		{
			const DataInfo1& info1 = GetInfo1(key1);
			for (const _Key2& key2 : info1.ChilderenKey2)
			{
				const DataInfo2& info2 = GetInfo2(key2);
				CheckChildrensInfo2(key2, info2);
				RY_CORE_ASSERT(info2.Parent1 == key1, "Children -> Info2 don't has Key1 as Parent!");
			}
		};

		void CheckChildrensInfo1(const _Key1& key1)const
		{
			const DataInfo1& info1 = GetInfo1(key1);
			CheckChildrensInfo1(key1);
		};

		void CheckChildrensInfo2(const _Key2& key2)const
		{
			const DataInfo2& info2 = GetInfo2(key2);
			CheckChildrensInfo2(key2, info2);

		};

		void CheckChildrensInfo2(const _Key2& key2, const DataInfo2& info2)const
		{
			for (const _Key3& key3 : info2.ChilderenKey3)
			{
				const DataInfo3& info3 = GetInfo3(key3);
				RY_CORE_ASSERT(info3.Parent2 == key2, "Children -> Info3 don't has Key2 as Parent!");
			}
		};

#pragma endregion

#pragma region Range

		// x,r = UINT32_MAX / y,g = Uint32_MIN
		static glm::uvec2 MinMaxRange() { return glm::uvec2{ static_cast<uint32_t>(-1), 0 }; }

		// x,r = Min Index / y,g = Max Index
		glm::uvec2 GetMinMaxRange2(const _Key1& key1)const
		{
			const DataInfo1& info1 = GetInfo1(key1);
			return GetMinMaxRange2(key1, info1);
		}

		// x,r = Min Index / y,g = Max Index
		glm::uvec2 GetMinMaxRange2(const _Key1& key1, const DataInfo1& info1)const
		{
			glm::uvec2 range = MinMaxRange();
			std::vector<typename _Key2> child2 = info1.ChilderenKey2;
			for (const _Key2& key2 : child2)
			{
				const DataInfo2& info2 = GetInfo2(key2);
				const uint32_t& index = info2.ThisIndex2;
				if (range.x > index)
					range.x = index;

				if (range.y < index)
					range.y = index;
			}
			return range;
		}

		// x,r = Min Index / y,g = Max Index
		glm::uvec2 GetMinMaxRange3(const _Key2& key2)const
		{
			const DataInfo2& info2 = GetInfo2(key2);
			return GetMinMaxRange3(key2, info2);
		}

		// x,r = Min Index / y,g = Max Index
		glm::uvec2 GetMinMaxRange3(const _Key2& key2, const DataInfo2& info2)const
		{
			glm::uvec2 range = MinMaxRange();
			std::vector<typename _Key3> child3 = info2.ChilderenKey3;
			for (const _Key3& key3 : child3)
			{
				const DataInfo3& info3 = GetInfo3(key3);
				const uint32_t& index = info3.Index;
				if (range.x > index)
					range.x = index;

				if (range.y < index)
					range.y = index;
			}
			range.y++;
			return range;
		}

#pragma endregion

#pragma region Looping

		void EcheInfo1c(std::function<void(const _Key1&, const DataInfo1&)> func)
		{
			for (const FirstPair& pair : m_FirstMap)
			{
				const _Key1& key1 = pair.first;
				const DataInfo1& info1 = pair.second;
				func(key1, info1);
			}
		}

		void EcheInfo1(std::function<void(const _Key1&, DataInfo1&)> func)
		{
			for (FirstPair& pair : m_FirstMap)
			{
				const _Key1& key1 = pair.first;
				DataInfo1& info1 = pair.second;
				func(key1, info1);
			}
		}

		void EcheInfo2c(std::function<void(const _Key2&, const DataInfo2&)> func)
		{
			for (const SecondPair& pair : m_SecoundMap)
			{
				const _Key2& key2 = pair.first;
				const DataInfo2& info2 = pair.second;
				func(key2, info2);
			}
		}

		void EcheInfo2(std::function<void(const _Key2&, DataInfo2&)> func)
		{
			for (SecondPair& pair : m_SecoundMap)
			{
				const _Key2& key2 = pair.first;
				DataInfo2& info2 = pair.second;
				func(key2, info2);
			}
		}

		void EcheInfo3c(std::function<void(const _Key3&, const DataInfo3&)> func)
		{
			for (const ThiredPair& pair : m_ThiredMap)
			{
				const _Key3& key3 = pair.first;
				const DataInfo3& info3 = pair.second;
				func(key3, info3);
			}
		}

		void EcheInfo3(std::function<void(const _Key3&, DataInfo3&)> func)
		{
			for (ThiredPair& pair : m_ThiredMap)
			{
				const _Key3& key3 = pair.first;
				DataInfo3& info3 = pair.second;
				func(key3, info3);
			}
		}

#pragma endregion

	private:
		FirstMapType m_FirstMap;
		SecondMapType m_SecoundMap;
		ThiredMapType m_ThiredMap;

		std::function<void(const DataInfo1&, const _Key1&)> m_OnEraseDestroyInfo1Func;
		std::function<void(const DataInfo2&, const _Key2&)> m_OnEraseDestroyInfo2Func;
		std::function<void(const DataInfo3&, const _Key3&)> m_OnEraseDestroyInfo3Func;

		std::function<void(const DataInfo1&, const _Key1&)> m_OnInserteCreateInfo1Func;
		std::function<void(const DataInfo2&, const _Key2&)> m_OnInserteCreateInfo2Func;
		std::function<void(const DataInfo3&, const _Key3&)> m_OnInserteCreateInfo3Func;

		std::function<void(const DataInfo1&, const _Key1&, uint32_t, uint32_t)> m_OnChangeIndexRangeInfo1Func;
		std::function<void(const DataInfo2&, const _Key2&, uint32_t, uint32_t)> m_OnChangeIndexRangeInfo2Func;
		std::function<void(const DataInfo3&, const _Key3&, uint32_t)> m_OnChangeIndexRangeInfo3Func;

		std::function<void()> m_OnClearFunc;

	};



	

}