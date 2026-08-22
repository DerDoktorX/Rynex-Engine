#pragma once
#include <Rynex/Renderer/API/Buffer.h>
#include <Rynex/Renderer/Rendering/RenderTarget.h>


namespace Rynex {
	
	class RenderTarget;
	class PiplineRenderBase;

	class PiplineRefBaseVec
	{
	public:
		using Iterator = std::vector<Ref<PiplineRenderBase>>::iterator;
		using ConstIterator = std::vector<Ref<PiplineRenderBase>>::const_iterator;
	public:
#if 0
		virtual ~PiplineRefBaseVec() {};
		virtual void Destroy() = 0;
		virtual void ResetFramePipline() = 0;

		virtual Ref<PiplineRenderBase> GetNextPipline() = 0;
		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i, RenderTarget& target) = 0;
		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i, Ref<RenderTarget>& target) = 0;


		virtual uint32_t GetIndexSize() const = 0;
		virtual uint32_t GetPiplineCount() const = 0;
		

		template<typename N>
		static Ref<PiplineRenderBase> GetPiplineAtType(uint32_t index, std::vector<Ref<PiplineRenderBase>>& vec)
		{
			static_assert(std::is_base_of_v<PiplineRenderBase, N>, "This Func can only be used by types they are Direitif from PiplineRenderBase!");
			Ref<T> pipline = nullptr;
			uint32_t count = vec.size();
			if (index < count)
			{
				pipline = vec.at(index);
			}
			else
			{
				RY_CORE_ASSERT(index == count, "Recusted Index is not next higer Index");
				pipline = CreateRef<N>();
				vec.emplace_back(pipline);
			}
			return pipline;
		}
#else
		PiplineRefBaseVec();

		virtual ~PiplineRefBaseVec() 
		{
			Destroy();
		};


		virtual void Destroy() 
		{
			m_PiplineIndex = 0u;
			m_PiplineVec.clear();
		}

		virtual void ResetFramePipline();

		virtual void ResetExpextedIndex();

		virtual uint32_t GetIndexSize() const { return m_PiplineIndex; }
		virtual uint32_t GetPiplineCount() const { return m_PiplineVec.size(); }

		virtual Ref<PiplineRenderBase> GetNextPipline() { RY_CORE_NOT_IMPL(); return Ref<PiplineRenderBase>(nullptr); }

		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i, RenderTarget& target) { RY_CORE_NOT_IMPL(); return Ref<PiplineRenderBase>(nullptr);}
		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i, Ref<RenderTarget>& target) { RY_CORE_NOT_IMPL(); return Ref<PiplineRenderBase>(nullptr);}
		void SetVaildOutPut(const BufferLayout& layout);
		const BufferLayout& GetVaildOutPut() const;

		template<typename T>
		Ref<PiplineRenderBase> GetPiplineType(uint32_t i, RenderTarget& target)
		{
			uint32_t index;

			Ref<PiplineRenderBase> pipline = nullptr;
			if (i < m_PiplineIndex)
			{
				index = i;
				pipline = GetPiplineAtType<T>(index);
			}
			else
			{
				RY_CORE_ASSERT(i == m_PiplineIndex, "Recusted Index is not next higer Index");
				index = m_PiplineIndex;
				m_PiplineIndex++;

				pipline = GetPiplineAtType<T>(index);
				target.AddPipline(pipline);
			}



			return pipline;
		}

		template<typename T>
		Ref<PiplineRenderBase> GetPiplineType(uint32_t i)
		{
			uint32_t index;
			Ref<PiplineRenderBase> pipline = nullptr;
			if (i < m_PiplineIndex)
			{
				index = i;
				pipline = GetPiplineAtType<T>(index);
			}
			else
			{
				RY_CORE_ASSERT(i == m_PiplineIndex, "Recusted Index is not next higer Index");
				index = m_PiplineIndex;
				m_PiplineIndex++;

				pipline = GetPiplineAtType<T>(index);
			}



			return pipline;
		}

		template<typename T>
		Ref<PiplineRenderBase> GetPiplineType(uint32_t i, Ref<RenderTarget>& target)
		{
			uint32_t index;
			Ref<PiplineRenderBase> pipline = nullptr;
			if (i < m_PiplineIndex)
			{
				index = i;
				pipline = PiplineRefBaseVec::GetPiplineAtType<T>(index);
			}
			else
			{
				RY_CORE_ASSERT(i == m_PiplineIndex, "Recusted Index is not next higer Index");
				index = m_PiplineIndex;
				m_PiplineIndex++;

				pipline = PiplineRefBaseVec::GetPiplineAtType<T>(index);
				target->AddPipline(pipline);
			}

			

			return pipline;
		}
#if 1
		template<typename T>
		Ref<PiplineRenderBase>& GetPiplineTypeRef(uint32_t& i)
		{
			Ref<PiplineRenderBase>& pipline = PiplineRefBaseVec::GetPiplineAtRef(i);
			if(pipline == nullptr)
				pipline = CreateRef<T>();
			return pipline;

		}
		
		Ref<PiplineRenderBase>& GetPiplineBaseRef(uint32_t& i)
		{
			if (m_PiplineIndex <= i)
			{
				i = m_PiplineIndex;
				m_PiplineIndex++;
			}	
			RY_CORE_ASSERT(i < m_PiplineIndex, "Esxpet Index to by smaler then index");
			Ref<PiplineRenderBase>& pipline = PiplineRefBaseVec::GetPiplineAtRef(i);
			return pipline;
		}
#endif

		template<typename T>
		Ref<PiplineRenderBase> GetNextPiplineType()
		{
			uint32_t index = m_PiplineIndex;
			Ref<PiplineRenderBase> pipline = PiplineRefBaseVec::GetPiplineAtType<T>(index);
			m_PiplineIndex++;

			return pipline;
		}

		Iterator begin() noexcept
		{
			return m_PiplineVec.begin();
		}

		Iterator end() noexcept
		{
			Iterator itBegin = m_PiplineVec.begin();
			itBegin = itBegin + m_PiplineIndex;
			return itBegin;
		}


		ConstIterator begin() const noexcept
		{
			return m_PiplineVec.begin();
		}

		ConstIterator end() const noexcept
		{
			ConstIterator itBegin = m_PiplineVec.begin();
			itBegin = itBegin + m_PiplineIndex;
			return itBegin;
		}
	private:
		template<typename T>
		Ref<PiplineRenderBase> GetPiplineAtType(uint32_t index)
		{
			Ref<PiplineRenderBase> pipline = nullptr;
			uint32_t count = m_PiplineVec.size();
			if (index < count)
			{
				pipline = m_PiplineVec.at(index);
			}
			else
			{
				RY_CORE_ASSERT(index == count, "Recusted Index is not next higer Index");
				pipline = CreateRef<T>();
				m_PiplineVec.emplace_back(pipline);
			}
			return pipline;
		}

		Ref<PiplineRenderBase>& GetPiplineAtRef(uint32_t index)
		{
			uint32_t count = m_PiplineVec.size();
			if (count <= index)
			{
				RY_CORE_ASSERT(index == count, "Recusted Index is not next higer Index");
				m_PiplineVec.emplace_back(nullptr);
			}
			Ref<PiplineRenderBase>& pipline = m_PiplineVec.at(index);
			return pipline;
		}

		virtual Ref<PiplineRenderBase> GetPiplineRef(uint32_t i) { RY_CORE_NOT_IMPL(); return Ref<PiplineRenderBase>(nullptr); }
		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i) { RY_CORE_NOT_IMPL(); return Ref<PiplineRenderBase>(nullptr); }


	protected:
		BufferLayout m_OutPutLayout;
	protected:
		uint32_t m_PiplineIndex;
		std::vector<Ref<PiplineRenderBase>> m_PiplineVec;
#endif
	};

	template<typename T>
	class PiplineRefVec : public PiplineRefBaseVec
	{
	public:
		PiplineRefVec()
			: PiplineRefBaseVec()
		{
		}

		~PiplineRefVec()
		{
		}

		

		
		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i, RenderTarget& target) override
		{
			uint32_t index;
			Ref<PiplineRenderBase> pipline = nullptr;
			if (i < m_PiplineIndex)
			{
				index = i;
				pipline = GetPiplineAt(index);
			}
			else
			{
				RY_CORE_ASSERT(i == m_PiplineIndex, "Recusted Index is not next higer Index");
				index = m_PiplineIndex;
				m_PiplineIndex++;

				pipline = GetPiplineAt(index);
				target.AddPipline(pipline);
			}
			

			return pipline;
		}

		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i, Ref<RenderTarget>& target) override
		{
			uint32_t index;
			Ref<PiplineRenderBase> pipline = nullptr;
			if (i < m_PiplineIndex)
			{
				index = i;
				pipline = GetPiplineAt(index);
			}
			else
			{
				RY_CORE_ASSERT(i == m_PiplineIndex, "Recusted Index is not next higer Index");
				index = m_PiplineIndex;
				m_PiplineIndex++;

				pipline = GetPiplineAt(index);
				target->AddPipline(pipline);
			}



			return pipline;
		}

		

		virtual Ref<PiplineRenderBase> GetNextPipline() override
		{
			Ref<PiplineRenderBase> pipline = GetPiplineAt(m_PiplineIndex);
			m_PiplineIndex++;

			return pipline;
		}
	private:
		Ref<PiplineRenderBase> GetPiplineAt(uint32_t index)
		{
			Ref<PiplineRenderBase> pipline = nullptr;
			uint32_t count = m_PiplineVec.size();
			if (index < count)
			{
				pipline = m_PiplineVec.at(index);
			}
			else
			{
				RY_CORE_ASSERT(index == count, "Recusted Index is not next higer Index");
				pipline = CreateRef<T>();
				m_PiplineVec.emplace_back(pipline);
			}
			return pipline;
		}
	};

#if 0
	template<typename ...Args>
	class PiplineDataPackedBaseVec 
	{
	public:
		using Piplines = typename std::variant<typename ...Args>;
		using Iterator = typename std::vector<typename Piplines>::iterator;
		using ConstIterator = typename std::vector<typename Piplines>::const_iterator;
	public:
#if 0
		virtual ~PiplineRefBaseVec() {};
		virtual void Destroy() = 0;
		virtual void ResetFramePipline() = 0;

		virtual Ref<PiplineRenderBase> GetNextPipline() = 0;
		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i, RenderTarget& target) = 0;
		virtual Ref<PiplineRenderBase> GetPipline(uint32_t i, Ref<RenderTarget>& target) = 0;


		virtual uint32_t GetIndexSize() const = 0;
		virtual uint32_t GetPiplineCount() const = 0;


		template<typename N>
		static Ref<PiplineRenderBase> GetPiplineAtType(uint32_t index, std::vector<Ref<PiplineRenderBase>>& vec)
		{
			static_assert(std::is_base_of_v<PiplineRenderBase, N>, "This Func can only be used by types they are Direitif from PiplineRenderBase!");
			Ref<T> pipline = nullptr;
			uint32_t count = vec.size();
			if (index < count)
			{
				pipline = vec.at(index);
			}
			else
			{
				RY_CORE_ASSERT(index == count, "Recusted Index is not next higer Index");
				pipline = CreateRef<N>();
				vec.emplace_back(pipline);
			}
			return pipline;
		}
#else
		PiplineDataPackedBaseVec()
			: m_PiplineIndex(0u)
			, m_PiplineVec()
			, m_OutPutLayout()
		{
		}

		~PiplineDataPackedBaseVec()
		{
			Destroy();
		};


		void Destroy()
		{
			m_PiplineIndex = 0u;
			m_PiplineVec.clear();
		}

		void ResetFramePipline()
		{
			uint32_t count = m_PiplineVec.size();
			RY_CORE_ASSERT(m_PiplineIndex <= count, "PilineIndex shoud be not be greater then the stack at max only equel!");

			for (uint32_t i = 0; i < m_PiplineIndex; i++)
			{
				auto& piplineBase = m_PiplineVec.at(i);
				piplineBase->Clear();
			}
			m_PiplineIndex = 0u;
		}

		void ResetExpextedIndex()
		{
			uint32_t count = m_PiplineVec.size();
			RY_CORE_ASSERT(m_PiplineIndex <= count, "PilineIndex shoud be not be greater then the stack at max only equel but equal is also not expeted!");

			m_PiplineIndex = 0u;
		}

		uint32_t GetIndexSize() const { return m_PiplineIndex; }
		uint32_t GetPiplineCount() const { return m_PiplineVec.size(); }


		void SetVaildOutPut(const BufferLayout& layout);
		const BufferLayout& GetVaildOutPut() const;

		
		Piplines& GetPipline(uint32_t index)
		{
			if (index < m_PiplineVec.size())
			{
				m_PiplineIndex = m_PiplineVec.size();
				return m_PiplineVec.emplace_back<T>(T());
			}
			else
			{
				return m_PiplineVec.at(index);
			}
			
		}

		Iterator begin() noexcept
		{
			return m_PiplineVec.begin();
		}

		Iterator end() noexcept
		{
			Iterator itBegin = m_PiplineVec.begin();
			itBegin = itBegin + m_PiplineIndex;
			return itBegin;
		}


		ConstIterator begin() const noexcept
		{
			return m_PiplineVec.begin();
		}

		ConstIterator end() const noexcept
		{
			ConstIterator itBegin = m_PiplineVec.begin();
			itBegin = itBegin + m_PiplineIndex;
			return itBegin;
		}
	
	protected:
		BufferLayout m_OutPutLayout;
	protected:
		uint32_t m_PiplineIndex;
		std::vector<Piplines> m_PiplineVec;
#endif
	};

	
#endif
}