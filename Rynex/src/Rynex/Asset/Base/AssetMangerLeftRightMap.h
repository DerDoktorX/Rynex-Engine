#include <rypch.h>



namespace Rynex {

    template<typename K, typename T>
    class AssetMangerLeftRightMap
    {
    public:
        using Map = OrderedMap<K, T>;
        // using MapIt = typename Map::iterator;
        // using MapConstIt = typename Map::const_iterator;
    // --- public member methode ----------------------------------------------------------------------------------------------
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
            RY_CORE_ASSERT(m_Stop, "Destructor called without prior Shutdown()! Ensure all worker threads are joined before destroying AssetMangerMapMutex.");
        }
        

        void Shoutdown();
        

        // Read: no mutex, only atomic Counter
        template<typename R, typename Func>
        R Read(Func&& func) const
        {
            // Wich instanc is curently aktiv?
            int idx = m_Instance.load(std::memory_order_acquire);

            m_ReadersCount.at(idx).fetch_add(1, std::memory_order_acq_rel);
            // Read from activen copy (never blockt!)
            const Map& value = m_Maps.at(idx);
            R result = func(value);

            m_ReadersCount.at(idx).fetch_sub(1, std::memory_order_acq_rel);
            return result;
        }

        template<typename R, typename Func>
        R FindThenRead(const K& key, Func&& func) const
        {
            using ExpextedFunc = std::function<void(const K& key, const T& value)>;
            static_assert(std::is_same_v<ExpextedFunc, Func>, "not expected Func");
            return Read<R>(
                [&key, func](const Map& map) -> R
                {
                    auto it = map.find(key);
                    if(it == map.end())
                    {
                        RY_CORE_ERROR("Key dosen't exist! FindThenRead");
                        return R{};
                    } 
                    const T& value = it->second;
                    return func(key, value);
                }
            );
        }

        template<typename R, typename Func>
        R ReadValue(const K& key, Func&& func) const
        {
            return Read<R>(
                [&key, func](const Map& map) -> R
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
        void FindThenWrite(const K& key, Func&& func) const
        {
            using ExpextedFunc = std::function<void(const K& key, T& value)>;

            // constexpr const char* funcTypeName = typeid(Func).name();
            static_assert(std::is_same_v<ExpextedFunc, Func>, "not expected Func");
            Write(
                [&key, func](const Map& map) -> void
                {
                    auto it = map.find(key);
                    if (it == map.end())
                    {
                        RY_CORE_ERROR("Key dosen't exist! FindThenWrite");
                        return;
                    }
                    T& value = it->second;
                    func(key, value);
                }
            );
        }

        template<typename Func>
        void WriteValue(const K& key, Func&& func)
        {
            FindThenWrite(key,
                [func](const K& keyLamda, T& value) -> void
                {
                    T& value = map.at(keyLamda);
                    func(keyLamda, value);
                }
            );
        }
        

        void Set(const K& key, const T& value)
        {
            FindThenWrite(key,
                [&value](const K& keyLambda, T& valueLambda) -> void
                {
                    valueLambda = value;
                }
            );
        }

        void GetRefLemda(std::function<void(T&)> func, const K& key)
        {
            FindThenWrite(key,
                [func](const K& keyLambda, T& value) -> void
                {
                    func(value);
                }
            );
        };

        // Convenience-Wrapper
        T GetCopy(const K& key) const 
        {
            return FindThenRead<T>(key,
                [&key](const Map& map) -> T
                {
                    return map.at(key);
                }
            );
        }

        bool IsFound(const K& key) const 
        {
            return Read<bool>(
                [&key](const Map& map) -> bool
                {
                    return map.find(key) != map.end();
                }
            );
        }

        void Add(const K& key, const T& value) 
        {
            Write(
                [&key, &value](Map& map) -> void
                {
                    map[key] = value;
                }
            );
        }

        void Remove(const K& key)
        {
            Write(
                [&key](Map& map) -> void
                {
                    if (map.find(key) == map.end())
                    {
                        RY_CORE_ERROR("Key not found to erase!");
                        return;
                    }
                    map.erase(key);
                }
            );
        }

        void Clear()
        {
            std::unique_lock writerLock(m_WriterMutex);

            Write(
                [this](Map& map)
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
                Map& map = m_Maps.at(next);
                func(map);
            }

            // 2. Instanz wich – new Reader use now 'next'
            m_Instance.store(next, std::memory_order_release);

            // 3. wait until all old reader from 'current' are ready
            while (m_ReadersCount.at(current).load(std::memory_order_acquire) != 0)
                std::this_thread::yield();

            // 4. Now the old (curently inactive) copy updating too
            {
                Map& map = m_Maps.at(current);
                func(map);
            }
        }

        

    // --- private member varibles --------------------------------------------------------------------------------------------
        std::array<Map, 2>                      m_Maps;
        mutable std::array<std::atomic<int>, 2> m_ReadersCount;
        std::mutex                              m_WriterMutex;
        std::atomic<int>                        m_Instance;
        bool                                    m_Stop;
    };



    template<typename K, typename T>
    void AssetMangerLeftRightMap<K, T>::Shoutdown()
    {
        std::unique_lock writerLock(m_WriterMutex);
        m_Stop = true;
        WriteAction(
            [this](Map& map)
            {
                map.clear();
            }
        );
    }
}