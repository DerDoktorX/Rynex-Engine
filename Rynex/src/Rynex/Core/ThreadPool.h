#pragma once
#include <queue>

namespace Rynex {
	class Application;


	class ThreadPool
	{
	public:
		struct SubmitTask
		{
			std::function<void()> Task;
			std::string TaskDisc;
		};

		struct ThreadTask
		{
			std::thread Thread;
			std::string Disc;
		};
	public:
		ThreadPool(uint32_t numThreads = std::thread::hardware_concurrency());
		~ThreadPool();
	
		void SubmitEnqueue(const std::function<void()>& task, const std::string& disription);
		std::vector<std::string> GetWorkingTask() const;

		uint32_t GetAktiveThreads() const { return m_Threads.size(); }
		static std::string GetCurentThreadIDstr();

		bool ExecuteWorkeOnCurentThread();
	private:
		bool IsStopped() const;
		void ExecutInPlace(const std::function<void()>& task, const std::string& disription);
		bool AddTaskToList(const std::function<void()>& task, const std::string& disription);

		void DestroyPool();
		void CreateThreads(uint32_t numThreads);
		
		bool ThreadWaitForTask(std::string& disc, std::string& idStr, std::function<void()>& task);
		void WaitForTask();
		bool HasTask();


	private:
		mutable std::mutex m_QueueMutex;

		std::queue<SubmitTask> m_Tasks;

		std::vector<std::thread> m_Threads;
		std::vector<std::string> m_Working;
	
		std::condition_variable m_CV;
		bool m_Stop = false;
	private:
		friend Application;
	};

}

