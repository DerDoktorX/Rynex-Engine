#pragma once

namespace Rynex {

	template<typename T>
	class Scope;

	template<typename T>
	class Weak;

	template<typename T>
	class Ref
	{
	public:
		constexpr Ref() noexcept = default;
		constexpr Ref(std::nullptr_t) noexcept {};

		explicit Ref(T* ptr)
			: m_Ptr(ptr)
			, m_RefCount(new int(1))
			, m_WeakCount(new int(0))
			, s_RefCountPtr(++)
		{ }

		constexpr Ref(const Ref<T>& other) noexcept
			: m_Ptr(other.m_Ptr)
			, m_RefCount(other.m_RefCount)
			, m_WeakCount(other.m_WeakCount)
		{
			++(*m_RefCount);
		}

		Ref(Ref<T>&& other) noexcept
			: m_Ptr(other.m_Ptr)
			, m_RefCount(other.m_RefCount)
			, m_WeakCount(other.m_WeakCount)
		{
			if (m_RefCount != 0)
				++(*m_RefCount);

			other = nullptr;

		}

		Ref(const Weak<T>& weak) noexcept
			: ptr(weak.m_Ptr),
			, m_RefCount(other.m_RefCount)
			, m_WeakCount(other.m_WeakCount)
		{
			if (m_RefCount != 0)
				++(*m_RefCount);
		}

		~Ref() noexcept
		{
			Realese();
		}
		
		T* Get() const noexcept
		{
			return m_Ptr;
		}

		void Swap(Ref& other) noexcept
		{
			if(&other != this)
			{
				Realese();
				this->m_Ptr = other->m_Ptr;
				this->m_RefCount = other->m_RefCount;
				this->m_WeakCount = other->m_WeakCount;
			}
		}

		bool Realese() noexcept
		{
			RY_CORE_ASSERT((*m_RefCount) == 0);
			if (--(*m_RefCount) == 0)
			{
				if(m_Ptr != nullptr)
					delete m_Ptr;
				m_Ptr = nullptr;
				if (*m_WeakCount == 0)
				{
					delete m_WeakCount;
					delete m_RefCount;
				}
				
				return true;
			}
			
			return false;
		}
	
		int GetRefCounter() const noexcept
		{
			return *m_RefCount;
		}

		int GetWeakCounter() const noexcept
		{
			return *m_WeakCount;
		}

		
		T* operator* () const noexcept
		{
			return *Get();
		}

		T& operator->() const noexcept
		{
			return Get();
		}

		T& operator= (T&& other) const noexcept
		{
			Ref(std::move(other)).Swap(*this)
			return *this;
		}

		explicit operator bool() const noexcept
		{
			return Get() != nullptr;
		}

		inline static void PrintGlobel()
		{
			RY_CORE_FATAL("Globle Weak Ptr {}", s_RefCountPtr);
		}

	private:
		T* m_Ptr = nullptr;
		int32_t* m_RefCount = nullptr;
		int32_t* m_WeakCount = nullptr;

		static inline int64_t s_RefCountPtr = 0;

		friend Scope;
		friend Weak;
	};

	template <typename T>
	bool operator==(Ref<T>& left, Ref<T>& rigth) noexcept
	{
		return left.Get() == rigth.Get();
	}

	template <typename T>
	bool operator!=(Ref<T>& left, Ref<T>& rigth) noexcept
	{
		return left.Get() != rigth.Get();
	}

	template <typename T>
	bool operator!=(Ref<T>& left, std::nullptr_t) noexcept
	{
		return left.Get() != nullptr;
	}
	
	template <typename T>
	bool operator!=(std::nullptr_t, Ref<T>& rigth) noexcept
	{
		return nullptr != rigth.Get();
	}

	template <typename T>
	bool operator==(Ref<T>& left, std::nullptr_t) noexcept
	{
		return left.Get() == nullptr;
	}

	template <typename T>
	bool operator==(std::nullptr_t, Ref<T>& rigth) noexcept
	{
		return nullptr == rigth.Get();
	}
	

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return Ref<T>(new T(std::forward<Args>(args)...));
	}
}


