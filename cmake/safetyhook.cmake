include(FetchContent)

FetchContent_Declare(
    safetyhook
    GIT_REPOSITORY https://github.com/cursey/safetyhook.git
    GIT_TAG 19223663fb8a573253ffb2e82da87cc354bf5c16
    GIT_PROGRESS TRUE	
)
set(SAFETYHOOK_FETCH_ZYDIS ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(safetyhook)