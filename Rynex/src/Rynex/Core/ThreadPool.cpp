#include "rypch.h"
#include "ThreadPool.h"


namespace Rynex {


	ThreadPool::ThreadPool(uint32_t numThreads)
        : m_CV()
	{
         
		uint32_t maxAvible = std::thread::hardware_concurrency();
         
		if (maxAvible < numThreads)
		{
			RY_CORE_FATAL("We use more threads then avible");
		}
#if 0
        for (size_t i = 0; i < numThreads; ++i)
        {
            m_Threads.emplace_back(
#if 0
                [this] {
                    while (true)
                    {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(m_QueueMutex);

                            m_CV.wait(lock, [this]
                                {
                                    return !m_Tasks.empty() || m_Stop;
                                });

                            if (m_Stop && m_Tasks.empty())
                            {
                                return;
                            }

                            // Get the next task from the queue
                            task = std::move(m_Tasks.front());
                            m_Tasks.pop();
                        }

                        task();
                    }
                }
#else
                RY_BIND_MEMBER_FUNC( ThreadPool::WaitForTask )
#endif

            );
        }
#else
        CreateThreads(numThreads);
#endif


	}

	ThreadPool::~ThreadPool()
	{
        DestroyPool();
	}

    void ThreadPool::SubmitEnqueue(const std::function<void()>& task, const std::string& disription)
    { 
        if (!m_Threads.empty() && AddTaskToList(task, disription))
        {
            m_CV.notify_one();
        }
        else
        {
            ExecutInPlace(task, disription);
        }
    }

    std::vector<std::string> ThreadPool::GetWorkingTask() const
    {
        std::unique_lock<std::mutex> lock(m_QueueMutex);
        return m_Working;
    }

    bool ThreadPool::IsStopped() const
    {
        return false;
    }

    void ThreadPool::ExecutInPlace(const std::function<void()>& task, const std::string& disription)
    {
        RY_CORE_TRACE("Need to execute threed task on the spot (main thread)! Task discription {}!", disription);
        task();
        RY_CORE_INFO("Finshed execution threed task on the spot (main thread)! Task discription {}!", disription);
    }

    bool ThreadPool::AddTaskToList(const std::function<void()>& task, const std::string& disription)
    {
        std::unique_lock<std::mutex> lock(m_QueueMutex);
        bool state = !m_Stop;
        if(state)
        {
            m_Tasks.emplace(SubmitTask{ task, disription });
        }
        return state;
    }

    void ThreadPool::DestroyPool()
    {
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Stop = true;
        }

        m_CV.notify_all();

        for (std::thread& thread : m_Threads)
        {
            thread.join();
        }
        m_Threads.clear();
    }

    void ThreadPool::CreateThreads(uint32_t numThreads)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            m_Threads.emplace_back( RY_BIND_MEMBER_FUNC(ThreadPool::WaitForTask) );
        }
    }

    bool ThreadPool::ThreadWaitForTask(std::string& disc, std::string& idStr, std::function<void()>& task)
    {
        std::unique_lock<std::mutex> lock(m_QueueMutex);
        std::thread::id idThread = std::this_thread::get_id();

        RY_CORE_TRACE("Wait Threade {}", idThread);
        m_CV.wait(lock, RY_BIND_MEMBER_FUNC(ThreadPool::HasTask));
        RY_CORE_TRACE("Continue Threade {}", idThread);

        if (m_Stop && m_Tasks.empty())
            return true;

        // Get the next task from the queue
        SubmitTask& subTask = m_Tasks.front();
        task = std::move(subTask.Task);
        disc = subTask.TaskDisc;

        idStr = GetCurentThreadIDstr();
        m_Working.emplace_back(disc);
        RY_THREAD_TASK_SUBMIT(idStr, disc);
        m_Tasks.pop();

        return false;
    }

    void ThreadPool::WaitForTask()
    {
        while (true)
        {

            std::function<void()> task;
            std::string disc = "(Empty)";
            std::string idStr = "(No Thread ID)";

#if 0
            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                std::thread::id idThread = std::this_thread::get_id();
#if 0
                std::thread::id* idThreadPtr = &idThread;
                uint32_t* idThreadNumberPtr = reinterpret_cast<uint32_t*>(idThreadPtr);
                uint32_t idThreadNumber = *idThreadNumberPtr;
#endif
                RY_CORE_TRACE("Wait Threade {}", idThread);
                m_CV.wait(lock, RY_BIND_MEMBER_FUNC(ThreadPool::HasTask) );
                RY_CORE_TRACE("Continue Threade {}", idThread);
                
                if (m_Stop && m_Tasks.empty())
                    return;


                // Get the next task from the queue
                SubmitTask& subTask = m_Tasks.front();
                task = std::move(subTask.Task);
                disc = subTask.TaskDisc;

                idStr = GetCurentThreadIDstr();
                m_Working.emplace_back(disc);
                RY_THREAD_TASK_SUBMIT(idStr, disc);
                m_Tasks.pop();
            }
#else
            if(ThreadWaitForTask(disc, idStr, task))
                return;
#endif
            
            if (nullptr != task)
            {
                task();
                std::thread::id threadID = std::this_thread::get_id();

                idStr = GetCurentThreadIDstr();
                {
                    std::unique_lock<std::mutex> lock(m_QueueMutex);
                    uint32_t index = 0u;
                    for (std::string& workTask : m_Working)
                    {
                        if (workTask == disc)
                            break;
                        index++;
                    }
                    if(index != m_Working.size())
                    {
                        std::vector<std::string>::iterator itBegin = m_Working.begin();
                        m_Working.erase(itBegin + index);
                    }
                    else
                    {
                        RY_CORE_TRACE("Task discritpion ({}) not found! ", disc);
                    }
                }
                RY_THREAD_TASK_FINSHED(idStr, disc);
            }
        }
    }

    bool ThreadPool::HasTask()
    {
        return !m_Tasks.empty() || m_Stop;
    }

    std::string ThreadPool::GetCurentThreadIDstr()
    {
        std::thread::id threadID = std::this_thread::get_id();
        std::stringstream stringStream;
        stringStream << threadID;
        std::string idStr = stringStream.str();

        return idStr;
    }

    bool ThreadPool::ExecuteWorkeOnCurentThread()
    {
        if (!ThreadPool::HasTask())
        {
            RY_CORE_INFO("No task Found to exexute now!");
            return false;
        }        
        std::function<void()> task;
        std::string disc = "(Empty)";
        std::string idStr = "(No Thread ID)";

        {

            std::unique_lock<std::mutex> lock(m_QueueMutex);
            if (ThreadPool::HasTask())
            {
                SubmitTask& subTask = m_Tasks.front();
                task = std::move(subTask.Task);
                disc = subTask.TaskDisc;
            }
            else
            {
                return false;
            }
        }

        if (nullptr != task)
        {
            task();
            std::thread::id threadID = std::this_thread::get_id();

            {
                std::unique_lock<std::mutex> lock(m_QueueMutex);
                uint32_t index = 0u;
                for (std::string& workTask : m_Working)
                {
                    if (workTask == disc)
                        break;
                    index++;
                }
                if (index != m_Working.size())
                {
                    std::vector<std::string>::iterator itBegin = m_Working.begin();
                    m_Working.erase(itBegin + index);
                }
                else
                {
                    RY_CORE_ERROR("Task discritpion ({}) not found! ", disc);
                }
            }
            RY_THREAD_TASK_FINSHED(idStr, disc);
            return true;
        }
        else
        {
            std::thread::id threadID = std::this_thread::get_id();
            RY_CORE_WARN("Thread Task not vaild ID: {}, discrption {}", threadID, disc);
            return false;
        }
        RY_CORE_ASSERT(false);
        return false;
    }

}


