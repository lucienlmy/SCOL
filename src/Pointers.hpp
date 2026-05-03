#pragma once
#include "rage/scrProgram.hpp"
#include "rage/scrThread.hpp"

namespace rage
{
    template <typename T>
    class atArray;
}

namespace SCOL
{
    namespace Functions
    {
        using RegisterNativeCommand = void (*)(PVOID table, std::uint64_t hash, rage::scrThread::NativeContext::Handler handler);
        using LoadScriptProgram = std::uint32_t (*)(const char* path, const char* name);
        using AllocateGlobalBlock = void (*)(rage::scrProgram* program);
        using InitNativeTables = void (*)(rage::scrProgram* program);
        using rage_new = void* (*)(std::size_t size, std::size_t align, int subAllocator, int a4);
        using sysVirtualFree = bool (*)(void* ptr);
        using ScriptProgramCtor = void (*)(rage::scrProgram* _this, const char* name, std::uint32_t codeSize, std::uint32_t nativeCount, std::uint32_t staticCount, std::uint32_t globalCount, std::uint32_t globalBlock, std::uint32_t stringsSize, std::uint32_t argCount, std::uint32_t globalVersion);
        using RunScriptThread = rage::scrThread::State (*)(rage::scrValue* stack, rage::scrValue** globals, rage::scrProgram* program, rage::scrThread::Context* context);
        using StartNewGtaThread = std::uint32_t (*)(std::uint32_t programHash, PVOID args, std::uint32_t argCount, std::uint32_t stackSize);
        using RegisterIndividualFile = std::uint32_t* (*)(std::uint32_t* result, const char* file, bool quitOnBadVersion, const char* relativePath, bool quitIfMissing, bool overlayIfExists);
        using InvalidateIndividualFile = void (*)(const char* file);
        using RequestStreamedObject = bool (*)(PVOID info, std::uint32_t index, std::uint32_t flags);
        using LoadAllStreamedObjects = void (*)(PVOID loader, bool a2);
        using ClearRequiredFlag = void (*)(PVOID info, std::uint32_t index, std::uint32_t flags);
        using RemoveStreamedObject = bool (*)(PVOID info, std::uint32_t index, bool a2);
        using UnregisterStreamedObject = bool (*)(PVOID info, std::uint32_t index);
    }

    struct PointerData
    {
        PVOID WndProc;
        rage::scrNativeRegistration* NativeRegistrationTable;
        PVOID UpdateScriptThreads;
        Functions::RegisterNativeCommand RegisterNativeCommand;
        Functions::LoadScriptProgram LoadScriptProgram;
        rage::atArray<rage::scrThread*>* ScriptThreads;
        Functions::AllocateGlobalBlock AllocateGlobalBlock;
        Functions::InitNativeTables InitNativeTables;
        rage::scrValue** ScriptGlobals;
        std::uint32_t* LoadingScreenState;
        Functions::rage_new rage_new;
        Functions::sysVirtualFree sysVirtualFree;
        rage::scrProgramRegistry* ScriptProgramRegistry;
        Functions::ScriptProgramCtor ScriptProgramCtor;
        Functions::RunScriptThread RunScriptThread;
        Functions::StartNewGtaThread StartNewGtaThread;
        Functions::RegisterIndividualFile RegisterIndividualFile;
        Functions::InvalidateIndividualFile InvalidateIndividualFile;
        PVOID StreamingEngineInfo;
        Functions::RequestStreamedObject RequestStreamedObject;
        PVOID StreamingEngineLoader;
        Functions::LoadAllStreamedObjects LoadAllStreamedObjects;
        Functions::ClearRequiredFlag ClearRequiredFlag;
        Functions::RemoveStreamedObject RemoveStreamedObject;
        Functions::UnregisterStreamedObject UnregisterStreamedObject;
    };

    struct Pointers : PointerData
    {
        bool Init();
    };

    inline Pointers g_Pointers;
}