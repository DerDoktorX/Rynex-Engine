#pragma once

namespace Rynex {

	template<typename T>
	class Ref;

	template<typename T>
	class Weak;

	template<typename T>
	class Scope
	{
	public:
		constexpr Scope() noexcept = default;
		constexpr Scope(std::nullptr_t) noexcept {};

		explicit Scope(T* ptr)
			: m_Ptr(ptr)
		{
			s_ScopeCountPtr++;
		}

		explicit Scope(Scope&& other)
			: m_Ptr(other.ptr)
		{
			other.m_Ptr = nullptr;
			
		}

		~Scope() noexcept
		{
			Realese();
		}

		T* Get() const noexcept
		{
			return m_Ptr;
		}

		void Swap(Scope& other) noexcept
		{
			if (&other != this)
			{
				Realese();
				this->m_Ptr = other.m_Ptr;
				other.m_Ptr = nullptr;
			}
		}

		bool Realese() noexcept
		{
			if (m_Ptr != nullptr)
			{
				delete m_Ptr;
				m_Ptr = nullptr;
				s_ScopeCountPtr--;
				return true;
			}

			return false;
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
			Ref(std::move(other)).swap(*this)
				return *this;
		}


		explicit operator bool() const noexcept
		{
			return Get() != nullptr;
		}

		Scope(const Ref<T>&) = delete;
		T& operator= (const T& other) = delete;

		inline static void PrintGlobel()
		{
			RY_CORE_FATAL("Globle Weak Ptr {}", s_ScopeCountPtr);
		}

	private:
		T* m_Ptr = nullptr;
	
		static inline int64_t s_ScopeCountPtr = 0;

		friend Ref;
		friend Weak;
	};

	template <typename T>
	bool operator==(Scope<T>& left, Scope<T>& rigth) noexcept
	{
		return left.Get() == rigth.Get();
	}

	template <typename T>
	bool operator!=(Ref<T>& left, Scope<T>& rigth) noexcept
	{
		return left.Get() != rigth.Get();
	}

	template <typename T>
	bool operator!=(Scope<T>& left, std::nullptr_t) noexcept
	{
		return left.Get() != nullptr;
	}

	template <typename T>
	bool operator!=(std::nullptr_t, Scope<T>& rigth) noexcept
	{
		return nullptr != rigth.Get();
	}

	template <typename T>
	bool operator==(Ref<T>& left, std::nullptr_t) noexcept
	{
		return left.Get() == nullptr;
	}

	template <typename T>
	bool operator==(std::nullptr_t, Scope<T>& rigth) noexcept
	{
		return nullptr == rigth.Get();
	}

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return Scope<T>(new T(std::forward<Args>(args)...));
	}

}
