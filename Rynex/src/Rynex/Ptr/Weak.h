#pragma once

namespace Rynex {


	template<typename T>
	class Scope;

	template<typename T>
	class Ref;

	template<typename T>
	class Weak
	{
	public:
		constexpr Weak() noexcept = default;
		constexpr Weak(std::nullptr_t) noexcept {};

		
		constexpr Weak(const Weak<T>& other) noexcept
			: m_Ptr(other.m_Ptr)
			, m_RefCount(other.m_RefCount)
			, m_WeakCount(other.m_WeakCount)
		{
			++(*m_WeakCount);
			--s_WeakCountPtr;
		}

		Weak(Weak<T>&& other) noexcept
			: m_Ptr(other.m_Ptr)
			, m_RefCount(other.m_RefCount)
			, m_WeakCount(other.m_WeakCount)
		{
			--s_WeakCountPtr;
			other.m_Ptr = nullptr;
		}

		Weak(const Ref<T>& weak) noexcept
			: ptr(weak.m_Ptr),
			, m_RefCount(other.m_RefCount)
			, m_WeakCount(other.m_WeakCount)
		{
			if (m_RefCount != 0)
				++(*m_WeakCount);
		}

		~Weak() noexcept
		{
			Realese();
		}

		Ref<T>* Get() const noexcept
		{
			if(*m_RefCount != 0)
				return m_Ptr;
		}

		void Swap(Weak& other) noexcept
		{
			if (&other != this)
			{
				Realese();
				this->m_Ptr = other->m_Ptr;
				this->m_RefCount = other->m_RefCount;
				this->m_WeakCount = other->m_WeakCount;
			}
		}

		Ref<T> Lock() const noexcept
		{
			if(*m_RefCount != 0)
				return Ref<T>(*this);
			return nullptr;
		}

		int GetWeakCounter() const noexcept
		{
			return *m_WeakCount;
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

		inline static void PrintGlobel()
		{
			RY_CORE_FATAL("Globle Weak Ptr {}", s_WeakCountPtr);
		}

	private:
		bool Realese() noexcept
		{
			RY_CORE_ASSERT((*m_WeakCount) == 0);
			if ((*m_RefCount) == 0 && --(*m_WeakCount) == 0)
			{
				--s_WeakCountPtr;
				delete m_WeakCount;
				delete m_RefCount;
				return true;
			}
			return false;
		}
	private:
		T* m_Ptr = nullptr;
		int32_t* m_RefCount = nullptr;
		int32_t* m_WeakCount = nullptr;

		inline static int64_t s_WeakCountPtr = 0;

		friend Scope;
		friend Ref;
	};

	template <typename T>
	bool operator==(Weak<T>& left, Weak<T>& rigth) noexcept
	{
		return left.Get() == rigth.Get();
	}

	template <typename T>
	bool operator!=(Weak<T>& left, Weak<T>& rigth) noexcept
	{
		return left.Get() != rigth.Get();
	}

	template <typename T>
	bool operator!=(Weak<T>& left, std::nullptr_t) noexcept
	{
		return left.Get() != nullptr;
	}

	template <typename T>
	bool operator!=(std::nullptr_t, Weak<T>& rigth) noexcept
	{
		return nullptr != rigth.Get();
	}

	template <typename T>
	bool operator==(Weak<T>& left, std::nullptr_t) noexcept
	{
		return left.Get() == nullptr;
	}

	template <typename T>
	bool operator==(std::nullptr_t, Weak<T>& rigth) noexcept
	{
		return nullptr == rigth.Get();
	}


}