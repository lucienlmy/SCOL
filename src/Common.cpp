#include "Common.hpp"

namespace SCOL
{
    std::atomic<bool> g_StartupAvailable = false;
    std::atomic<bool> g_LoadRequested = false;
}