#pragma once
#include<rypch.h>

namespace Rynex {

	namespace Memory {
		template<typename T>
		using VectorIterator = typename std::vector<T>::iterator;

		template<typename T>
		using VectorConstIterator = typename std::vector<T>::const_iterator;


		template<typename T, typename N>
		constexpr size_t ElementEqualCompareDefaultFunc(const T& aElement, const N& bElement)
		{
			return aElement == bElement;
		}

		template<typename T, typename N>
		constexpr size_t ElementSmalerThenCompareDefaultFunc(const T& aElement, const N& bElement)
		{
			return aElement < bElement;
		}

		template<typename S, typename T, typename N, typename Func>
		constexpr S GetElementIndexVector(std::vector<T>& vec, const N& element, Func func)
		{
			S i = 0;
			for (const T& e : vec)
			{
				if (func(e, element))
				{
					return i;
				}
				i++;
			}
			return std::numeric_limits<S>::max();
		}

		
		template<typename S, typename T, typename N>
		constexpr S GetElementIndexVector(std::vector<T>& vec, const N& element)
		{
			return GetElementIndexVector<S, T, N>(vec, element, [](const T& a, const N& b) { return ElementEqualCompareDefaultFunc<T, N>(a, b);});
		}
		
		template<typename T, typename N, typename Func>
		constexpr size_t GetElementIndexVector(std::vector<T>& vec, const N& element, Func func)
		{
			return GetElementIndexVector<size_t, T, N, Func>(vec, element, func);
		}

		template<typename T, typename N>
		constexpr size_t GetElementIndexVector(std::vector<T>& vec, const N& element)
		{
			return GetElementIndexVector<size_t, T, N>(vec, element, [](const T& a, const N& b) { return ElementEqualCompareDefaultFunc<T, N>(a, b);});
		}

		template<typename T, typename N, typename Func>
		constexpr bool HasElementVector(std::vector<T>& vec, const T& element, Func func)
		{
			size_t count = GetElementIndexVector<T>(vec, element, func);
			constexpr size_t max = std::numeric_limits<size_t>::max();
			return vec != max;
		}

		template<typename T, typename N>
		constexpr bool HasElementVector(std::vector<T>& vec, const T& element)
		{
			size_t index= GetElementIndexVector<T>(vec, element, );
			constexpr size_t max = std::numeric_limits<size_t>::max();
			return index != max;
		}

		template<typename T, typename N, typename Func>
		constexpr bool RemoveElementVector(std::vector<T>& vec, const N& element, Func func)
		{
			size_t index = GetElementIndexVector<T>(vec, element, func);
			constexpr size_t max = std::numeric_limits<size_t>::max();

			if (index == max)
				return false;

			std::vector<T>::iterator it = vec.begin() + index;
			vec.erase(it);
			return true;
		}

		template<typename T, typename N, typename Func>
		constexpr bool AddBackElementVector(std::vector<T>& vec, const N& element, Func func)
		{
			if (HasElementVector(vec, element, func))
				return false;

			vec.emplace_back(element);
			return true;
		}

		template<typename T, typename N, typename Func>
		constexpr VectorIterator<T> GetSortedElementIteratorVector(std::vector<T>& vec, const N& element, Func func)
		{
			using It = VectorIterator<T>;

			It itBegin = vec.begin();
			It itPos = std::lower_bound(itBegin, vec.end(), element, func);
			return itPos;
		}


		template<typename T, typename N, typename Func>
		constexpr bool HasSortedElementIteratorVector(VectorIterator<T> iterator, VectorIterator<T> iteratorEnd, const N& element, Func func)
		{
			using It = VectorIterator<T>;

			bool isNotLast = iterator < iteratorEnd;
			bool isSameElement = isNotLast ? func(*iterator, element) : false;

			return isNotLast && isSameElement;
		}

		template<typename T, typename N, typename FuncSort, typename FuncEqual>
		constexpr bool HasSortedElementIteratorVector(std::vector<T>& vec, const N& element, FuncSort funcSort, FuncEqual funcEqual)
		{
			using It = VectorIterator<T>;
			It itPos = GetSortedElementIteratorVector(vec, element, funcSort);
		
			return HasSortedElementIteratorVector(vec, element, itPos, funcEqual);
		}

		template<typename T, typename N, typename FuncSort, typename FuncEqual>
		constexpr bool AddSortedElementVector(std::vector<T>& vec, const N& insertElement, FuncSort funcSort, FuncEqual funcEqual)
		{
			using It = VectorIterator<T>;
			if (vec.empty())
			{
				vec.insert(vec.begin(), insertElement);
				return true;
			}
			It itPos = GetSortedElementIteratorVector(vec, insertElement, funcSort);
			if (HasSortedElementIteratorVector<T, N, FuncEqual>(itPos, vec.end(), insertElement, funcEqual))
			{
				RY_CORE_WARN("You try to add a element what is alrady added");
				return false;
			}

			vec.insert(itPos, insertElement);
			return true;
		}

		template<typename T, typename N, typename FuncSort, typename FuncEqual>
		constexpr bool RemoveSortedElementVector(std::vector<T>& vec, const N& element, FuncSort funcSort, FuncEqual funcEqual)
		{
			using It = VectorIterator<T>;

			if (vec.empty())
			{
				RY_CORE_WARN("You try to remove form a empty Vector!");
				return false;
			}
			It itPos = GetSortedElementIteratorVector(vec, element, funcSort);
			if (HasSortedElementIteratorVector(itPos, vec.end(), element, funcEqual))
			{
				vec.erase(itPos);
				return true;
			}
			
			RY_CORE_WARN("You try to remove a Element from Vector but it has not found withe your equals Funktion the ellement to Remove!");
			return false;
		}

	}
}