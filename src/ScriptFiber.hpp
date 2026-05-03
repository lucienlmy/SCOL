#pragma once

#undef Yield

namespace SCOL
{
    class ScriptFiber
    {
    public:
        static void Tick()
        {
            GetInstance().TickImpl();
        }

        static void Yield(std::optional<std::chrono::high_resolution_clock::duration> time = std::nullopt)
        {
            GetInstance().YieldImpl(time);
        }

        static void Add(std::string_view name, std::function<void()> callback)
        {
            GetInstance().AddImpl(name, callback);
        }

    private:
        class Fiber
        {
        public:
            explicit Fiber(std::string_view name, std::function<void()> callback);
            ~Fiber();

            void Tick();
            void Yield(std::optional<std::chrono::high_resolution_clock::duration> time = std::nullopt);

        private:
            std::string m_Name;
            std::function<void()> m_Callback;
            bool m_Done;
            HANDLE m_ChildFiber;
            HANDLE m_MainFiber;
            std::optional<std::chrono::high_resolution_clock::time_point> m_WakeTime;
        };

        static ScriptFiber& GetInstance()
        {
            static ScriptFiber instance;
            return instance;
        }

        void TickImpl();
        void YieldImpl(std::optional<std::chrono::high_resolution_clock::duration> time = std::nullopt);
        void AddImpl(std::string_view name, std::function<void()> callback);

        std::mutex m_Mutex;
        std::vector<std::unique_ptr<Fiber>> m_Fibers;
    };
}