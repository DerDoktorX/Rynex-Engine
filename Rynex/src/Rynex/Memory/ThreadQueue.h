#pragma once
#include <rypch.h>


namespace Rynex {

	namespace Memory {

		class ThreadQueue 
		{
		public:
			using T = int;
		public:
			ThreadQueue()
				: m_Queue()
				, m_Mutex()
			{

			}

			ThreadQueue(const ThreadQueue& threadQueue)
				: m_Queue()
				, m_Mutex()
			{
				std::scoped_lock<std::mutex> lockLocale(m_Mutex);
				std::scoped_lock<std::mutex> lockMove(threadQueue.m_Mutex);
				m_Queue = threadQueue.m_Queue;

			}

			ThreadQueue(ThreadQueue&& threadQueue) noexcept
				: m_Queue()
				, m_Mutex()
			{
				std::scoped_lock<std::mutex> lockLocale(m_Mutex);
				std::scoped_lock<std::mutex> lockMove(threadQueue.m_Mutex);
				
				m_Queue = std::move(threadQueue.m_Queue);

			}

			~ThreadQueue()
			{

			}

			void Push(const T& elment)
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
				m_Queue.push(elment);
			}

			T Front() const
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
				return m_Queue.front();
			}

			T Pop()
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
				T element = m_Queue.front();
				m_Queue.pop();
				return element;
			}

			bool Empty() const
			{
				return m_Queue.empty();
			}

		private:
			std::queue<T> m_Queue;
			mutable std::mutex m_Mutex;
		};

		template<typename ...Args>
		class ThreadQueueVarients
		{
		public:
			ThreadQueueVarients()
				: m_Queue()
				, m_Mutex()
			{

			}

			ThreadQueueVarients(const ThreadQueueVarients& threadQueueVarients)
				: m_Queue()
				, m_Mutex()
			{
				std::scoped_lock<std::mutex> lockLocale(m_Mutex);
				std::scoped_lock<std::mutex> lockMove(threadQueueVarients.m_Mutex);
				m_Queue = threadQueueVarients.m_Queue;

			}

			ThreadQueueVarients(ThreadQueueVarients&& threadQueueVarients) noexcept
				: m_Queue()
				, m_Mutex()
			{
				std::scoped_lock<std::mutex> lockLocale(m_Mutex);
				std::scoped_lock<std::mutex> lockMove(threadQueueVarients.m_Mutex);

				m_Queue = std::move(threadQueueVarients.m_Queue);

			}

			~ThreadQueueVarients()
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
			}

			template<typename T>
			void Push(const T& elment)
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
				m_Queue.push(elment);
			}

			template<typename T>
			void PushNoLock(const T& elment)
			{
				m_Queue.push(elment);
			}

			template<typename Func>
			void Front(Func&& func) const
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
				const std::variant<Args...>& varient = m_Queue.front();

				std::visit(func, varient);
			}


			template<typename Func>
			void Front(Func&& func)
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
				std::variant<Args...>& varient = m_Queue.front();
				std::visit(func, varient);
			}

			
			template<typename Func>
			void Pop(Func&& func)
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
				std::variant<Args...>& varient = m_Queue.front();
				std::visit(func, varient);
				m_Queue.pop();
			}
			
			void Pop()
			{
				std::scoped_lock<std::mutex> lock(m_Mutex);
				m_Queue.pop();
			}

			std::mutex& GetMutex() { return m_Mutex; }

			template<typename Func>
			void FrontNoLock(Func&& func) const
			{
				const std::variant<Args...>& varient = m_Queue.front();
				std::visit(func, varient);
			}

			std::variant<Args...> GetFrontNoLock()
			{
				return m_Queue.front();
			}

			template<typename Func>
			void FrontNoLock(Func&& func)
			{
				std::variant<Args...>& varient = m_Queue.front();
				std::visit(func, varient);
			}

			template<typename Func>
			void PopNoLock(Func&& func)
			{
				std::variant<Args...>& varient = m_Queue.front();
				std::visit(func, varient);
				m_Queue.pop();
			}

			void PopNoLock()
			{
				m_Queue.pop();
			}

			

			bool Empty() const
			{
				return m_Queue.empty();
			}

		private:
			std::queue<std::variant<Args...>> m_Queue;
			mutable std::mutex m_Mutex;
		};
	}
}