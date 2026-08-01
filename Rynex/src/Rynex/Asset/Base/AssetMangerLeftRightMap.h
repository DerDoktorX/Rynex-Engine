#include <rypch.h>



namespace Rynex {

    template<typename K, typename T>
    class AssetMangerLeftRightMap
    {
       
    public:
        AssetMangerLeftRightMap()
            : m_Maps()
            , m_Instance{ 0 }
            , m_ReadersCount{ 0, 0 }
            , m_WriterMutex()
            , m_Stop(false)
        {
        }

        ~AssetMangerLeftRightMap()
        { 
            std::unique_lock writerLock(m_WriterMutex);
            m_Stop = true;
            WriteAction(
                [this](std::map<K, T>& map)
                {
                    map.clear();
                }
            );
        }

        // Read: no mutex, only atomic Counter
        template<typename Func>
        auto Read(Func&& func) const 
        {
            // Wich instanc is curently aktiv?
            int idx = m_Instance.load(std::memory_order_acquire);

            m_ReadersCount.at(idx).fetch_add(1, std::memory_order_acq_rel);
            // Read from activen copy (never blockt!)
           
            auto result = func(m_Maps.at(idx));

            m_ReadersCount.at(idx).fetch_sub(1, std::memory_order_acq_rel);
            return result;
        }

        template<typename Func>
        auto ReadValue(const K& key, Func&& func) const
        {
            return Read(
                [&key, func](const std::map<K, T>& map)
                {
                    const T& value = map.at(key);
                    return func(value);
                }
            );
        }

        // Write: exklusiv, update both copys
        template<typename Func>
        void Write(Func&& func)
        {
            if (m_Stop)
            {
                RY_CORE_FATAL("no write action posible stop means we deleat this list!");
                return;
            }

            std::unique_lock writerLock(m_WriterMutex);
            WriteAction(func);
        }

        template<typename Func>
        void WriteValue(const K& key, Func&& func)
        {
             Write(
                [&key, func](std::map<K, T>& map)
                {
                    T& value = map.at(key);
                    func(value);
                }
            );
        }

        inline void Change(const K& key, const T& value)
        {
            Write(
                [&key, &value](std::map<K, T>& map)
                {
                    map.at(key) = value;
                }
            );
        }

        inline void Set(const K& key, const T& value)
        {
            Write(
                [&key, &value](std::map<K, T>& map)
                {
                    map.at(key) = value;
                }
            );
        }

        inline void GetRefLemda(std::function<void(T&)> func, const K& key)
        {
            Write(
                [&key, func](std::map<K, T>& map)
                {
                    func(map.at(key));
                }
            );
        };

        // Convenience-Wrapper
        T GetCopy(const K& key) const 
        {
            return Read(
                [&key](const std::map<K, T>& map)
                {
                    return map.at(key);
                }
            );
        }

        bool IsFound(const K& key) const 
        {
            return Read(
                [&key](const std::map<K, T>& map)
                {
                    return map.find(key) != map.end();
                }
            );
        }

        void Add(const K& key, const T& value) 
        {
            Write(
                [&key, &value](std::map<K, T>& map)
                {
                    map[key] = value;
                }
            );
        }

        void Remove(const K& key)
        {
            Write(
                [&key](std::map<K, T>& map)
                {
                    map.erase(key);
                }
            );
        }

        void Clear()
        {
            std::unique_lock writerLock(m_WriterMutex);

            Write(
                [this](std::map<K, T>& map)
                {
                    map.clear();
                }
            );


        }

    private:
        template<typename Func>
        void WriteAction(Func&& func)
        {
            int current = m_Instance.load(std::memory_order_acquire);
            int next = 1 - current; // toggled index

            // 1. Inactive copy updaten
            {
                std::map<K, T>& map = m_Maps.at(next);
                func(map);
            }

            // 2. Instanz wich – new Reader use now 'next'
            m_Instance.store(next, std::memory_order_release);

            // 3. wait until all old reader from 'current' are ready
            while (m_ReadersCount.at(current).load(std::memory_order_acquire) != 0)
                std::this_thread::yield();

            // 4. Now the old (curently inactive) copy updating too
            {
                std::map<K, T>& map = m_Maps.at(current);
                func(map);
            }
        }

        

    // --- private member varibles --------------------------------------------------------------------------------------------
        std::array<std::map<K, T>, 2>           m_Maps;
        std::atomic<int>                        m_Instance;
        mutable std::array<std::atomic<int>, 2> m_ReadersCount;
        std::mutex                              m_WriterMutex;
        bool                                    m_Stop;
    };
}