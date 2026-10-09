#pragma once
// Span created in Project Rynex-Rendering on 06/10/2026.
// Ai generated class as working implementation of std::span from cpp 20.

namespace Rynex {

    inline constexpr std::size_t DynamicExtent = std::numeric_limits<std::size_t>::max();

	template<typename T, std::size_t Extent = DynamicExtent>
	class Span;

	namespace SpanDetail {

		// Byte extent of a span after reinterpreting its elements as bytes (same rule as std::as_bytes).
		constexpr std::size_t ByteExtent(std::size_t elementSize, std::size_t extent)
		{
			if (DynamicExtent == extent)
			{
				return DynamicExtent;
			}

			std::size_t byteExtent = elementSize * extent;
			return byteExtent;
		}

		template<typename T>
		struct IsSpan : std::false_type {};

		template<typename T, std::size_t Extent>
		struct IsSpan<Span<T, Extent>> : std::true_type {};

		// True if Container offers std::data / std::size and its element pointer converts to Element* safely.
		template<typename Container, typename Element, typename = void>
		struct IsSpanContainer : std::false_type {};

		template<typename Container, typename Element>
		struct IsSpanContainer<Container, Element,
			std::void_t<decltype(std::data(std::declval<Container&>())), decltype(std::size(std::declval<Container&>()))>>
			: std::is_convertible<std::remove_pointer_t<decltype(std::data(std::declval<Container&>()))>(*)[], Element(*)[]>
		{
		};

		// Static extent: the size is part of the type, only the pointer is stored.
		template<typename T, std::size_t Extent>
		class SpanStorage
		{
		public:
			constexpr SpanStorage(T* data, std::size_t size) noexcept
				: m_Data(data)
			{
				RY_CORE_ASSERT(Extent == size, "Size does not match the static extent of the Span");
				static_cast<void>(size);
			}

			constexpr T* Data() const noexcept
			{
				return m_Data;
			}

			constexpr std::size_t Size() const noexcept
			{
				return Extent;
			}

		private:
			T* m_Data;
		};

		// Dynamic extent: pointer and size are stored.
		template<typename T>
		class SpanStorage<T, DynamicExtent>
		{
		public:
			constexpr SpanStorage(T* data, std::size_t size) noexcept
				: m_Data(data)
				, m_Size(size)
			{
				RY_CORE_ASSERT((nullptr != data) || (0ull == size), "A Span with elements needs a valid data pointer");
			}

			constexpr T* Data() const noexcept
			{
				return m_Data;
			}

			constexpr std::size_t Size() const noexcept
			{
				return m_Size;
			}

		private:
			T* m_Data;
			std::size_t m_Size;
		};

	}

	template<typename T, std::size_t Extent>
	class Span
	{
	public:
		using element_type = T;
		using value_type = std::remove_cv_t<T>;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using iterator = T*;
		using reverse_iterator = std::reverse_iterator<iterator>;

	// --- public static variables -------------------------------------------------------------------
		static constexpr size_type extent = Extent;

	// --- public constructors -----------------------------------------------------------------------
		template<std::size_t E = Extent, std::enable_if_t<(DynamicExtent == E) || (0ull == E), int> = 0>
		constexpr Span() noexcept
			: m_Storage(nullptr, 0ull)
		{
		}

		constexpr Span(pointer data, size_type count)
			: m_Storage(data, count)
		{
		}

		// Arrays, std::array, std::vector, std::string, ... (anything with std::data and std::size).
		template<typename Container, std::enable_if_t<
			(!SpanDetail::IsSpan<std::remove_cv_t<Container>>::value)
			&& SpanDetail::IsSpanContainer<Container, element_type>::value, int> = 0>
		constexpr Span(Container& container)
			: m_Storage(std::data(container), std::size(container))
		{
		}

		// Span<U, N> -> Span<T, Extent>, for example Span<int> -> Span<const int>.
		template<typename U, std::size_t N, std::enable_if_t<
			    (
			        (DynamicExtent == Extent) || (DynamicExtent == N) || (Extent == N)
			    )
			    && std::is_convertible_v<U(*)[], element_type(*)[]>, int> = 0>
		constexpr Span(const Span<U, N>& other) noexcept
			: m_Storage(other.data(), other.size())
		{
		}

	// --- public observers --------------------------------------------------------------------------
		constexpr pointer data() const noexcept
		{
			return m_Storage.Data();
		}

		constexpr size_type size() const noexcept
		{
			return m_Storage.Size();
		}

		constexpr size_type size_bytes() const noexcept
		{
			size_type bytes = size() * sizeof(element_type);
			return bytes;
		}

		constexpr bool empty() const noexcept
		{
			return 0ull == size();
		}

	// --- public element access ---------------------------------------------------------------------
		constexpr reference operator[](size_type index) const
		{
			RY_CORE_ASSERT(index < size(), "Span index out of range");
			return data()[index];
		}

		constexpr reference front() const
		{
			RY_CORE_ASSERT(!empty(), "front() on an empty Span");
			return data()[0ull];
		}

		constexpr reference back() const
		{
			RY_CORE_ASSERT(!empty(), "back() on an empty Span");
			size_type lastIndex = size() - 1ull;
			return data()[lastIndex];
		}

	// --- public iterators --------------------------------------------------------------------------
		constexpr iterator begin() const noexcept
		{
			return data();
		}

		constexpr iterator end() const noexcept
		{
			return data() + size();
		}

		constexpr reverse_iterator rbegin() const noexcept
		{
			return reverse_iterator(end());
		}

		constexpr reverse_iterator rend() const noexcept
		{
			return reverse_iterator(begin());
		}

	// --- public sub views --------------------------------------------------------------------------
		constexpr Span<element_type> first(size_type count) const
		{
			RY_CORE_ASSERT(count <= size(), "first() count is bigger than the Span");
			return Span<element_type>(data(), count);
		}

		constexpr Span<element_type> last(size_type count) const
		{
			RY_CORE_ASSERT(count <= size(), "last() count is bigger than the Span");
			size_type offset = size() - count;
			return Span<element_type>(data() + offset, count);
		}

		constexpr Span<element_type> subspan(size_type offset, size_type count = DynamicExtent) const
		{
			RY_CORE_ASSERT(offset <= size(), "subspan() offset is behind the end of the Span");
			size_type available = size() - offset;
			size_type resultCount = count;
			if (DynamicExtent == count)
			{
				resultCount = available;
			}

			RY_CORE_ASSERT(resultCount <= available, "subspan() range is bigger than the Span");
			return Span<element_type>(data() + offset, resultCount);
		}

	private:
		SpanDetail::SpanStorage<element_type, Extent> m_Storage;
	};

}