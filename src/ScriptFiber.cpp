#include "ScriptFiber.hpp"

namespace SCOL
{
    ScriptFiber::Fiber::Fiber(std::string_view name, std::function<void()> callback)
        : m_Name(name),
          m_Callback(callback),
          m_Done(false),
          m_ChildFiber(0),
          m_MainFiber(0),
          m_WakeTime(std::nullopt)
    {
        m_ChildFiber = CreateFiber(0, [](void* param) {
            auto thisFiber = static_cast<Fiber*>(param);
            thisFiber->m_Callback();
            thisFiber->m_Done = true;
            SwitchToFiber(thisFiber->m_MainFiber); }, this);
    }

    ScriptFiber::Fiber::~Fiber()
    {
        if (m_ChildFiber)
            DeleteFiber(m_ChildFiber);
    }

    void ScriptFiber::Fiber::Tick()
    {
        m_MainFiber = GetCurrentFiber();
        if ((!m_WakeTime.has_value() || m_WakeTime.value() <= std::chrono::high_resolution_clock::now()) && !m_Done)
            SwitchToFiber(m_ChildFiber);
    }

    void ScriptFiber::Fiber::Yield(std::optional<std::chrono::high_resolution_clock::duration> time)
    {
        if (time.has_value())
            m_WakeTime = std::chrono::high_resolution_clock::now() + time.value();
        else
            m_WakeTime = std::nullopt;

        SwitchToFiber(m_MainFiber);
    }

    void ScriptFiber::TickImpl()
    {
        std::lock_guard lock(m_Mutex);
        static bool ensureMainFiber = (ConvertThreadToFiber(nullptr), true);

        for (const auto& script : m_Fibers)
            script->Tick();
    }

    void ScriptFiber::YieldImpl(std::optional<std::chrono::high_resolution_clock::duration> time)
    {
        if (auto script = static_cast<Fiber*>(GetFiberData()))
            script->Yield(time);
    }

    void ScriptFiber::AddImpl(std::string_view name, std::function<void()> callback)
    {
        std::lock_guard lock(m_Mutex);
        m_Fibers.push_back(std::move(std::make_unique<ScriptFiber::Fiber>(name, callback)));
        LOGF(INFO, "Registered script fiber with name '{}'.", name);
    }
}