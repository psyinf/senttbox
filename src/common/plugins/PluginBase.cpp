#include "PluginBase.h"

#include <array>
#include <fmt/core.h>
#include <functional>
#include <stdexcept>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#elif __linux__
#include <dlfcn.h>
#endif

using namespace common;
#ifdef _WIN32
std::string FormatErrorMessage(const DWORD errorCode)
{
    std::array<char, 512> message;

    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        message.data(),
        message.size(),
        nullptr);

    return std::string(message.begin(), message.end());
}
#endif

//#TODO: explore modes and try to match flags to unify interface here

PluginBase::DLLHandle LoadSharedLibrary(std::string_view libraryPath, [[maybe_unused]] int iMode = 2)
{
#if defined(_MSC_VER) // Microsoft compiler
    return static_cast<HMODULE>(LoadLibraryEx(libraryPath.data(), nullptr, 0x0));
#elif defined(__linux__)
    return dlopen(libraryPath.data(), iMode);
#endif
}


std::string getLastError()
{
#if defined(_MSC_VER) // Microsoft compiler
    DWORD error        = ::GetLastError();
    auto  last_err_str = FormatErrorMessage(error);
    last_err_str.append(" ( ");
    last_err_str.append(std::to_string(error));
    last_err_str.append(" )");
    return last_err_str;
#elif __linux__
    char* error = dlerror();
    return std::string(error);
#endif
}




PluginBase::PluginBase(const std::string& path)
    : path(path)
{
    auto handle = LoadSharedLibrary(path);

    if (!handle.has_value())
    {
        auto last_err_str = getLastError();
        throw std::invalid_argument(fmt::format("Could not load '{}'.\nError reported: {}", path, last_err_str));
    }
    // TODO:move to generic list or use introspection
    getInfoFunction = bindFunction<void, PluginInfo&>(handle, std::string("getInfo"));
    if (!getInfoFunction)
    {
        reportMissingInterface(path, "getInfo");
    }

    dllHandle = handle;
}

void PluginBase::reportMissingInterface(const std::string& path, const std::string& name) const
{
    throw std::invalid_argument(fmt::format("Plugin at '{}' is not a valid plugin (Missing '{}' interface)", path, name));
}

void PluginBase::getInfo(PluginInfo& info) const
{
    getInfoFunction(info);
}

PluginBase::~PluginBase()
{

    if (dllHandle.has_value())
    {
        PluginInfo info;
        getInfo(info);
#ifdef _WIN32
        ::FreeLibrary(std::any_cast<HMODULE>(dllHandle));
#elif  __linux__
        dlclose(std::any_cast<void*>(dllHandle));
#endif
    }
}

void* PluginBase::_getFunction(const DLLHandle& handle, std::string_view name)
{
#if defined(_MSC_VER) // Microsoft compiler
    return ::GetProcAddress(std::any_cast<HINSTANCE>(handle), name.data());
#elif __linux__
    return dlsym(std::any_cast<void*>(handle), name.data());
#endif
}
