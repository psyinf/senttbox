#pragma once

#include <any>
#include <functional>
#include <string>

#ifdef _WIN32
#define PLUGIN_API __declspec(dllexport)
#elif __linux__
#define PLUGIN_API
#endif

namespace common
{
struct PluginInfo
{
    std::string name;
    std::string version;

    auto operator<=>(const PluginInfo&) const = default;
};

class PluginBase
{
public:
    using DLLHandle = std::any;

    explicit PluginBase(const std::string& path);
    virtual ~PluginBase();

    PluginBase(const PluginBase&)            = delete;
    PluginBase& operator=(const PluginBase&) = delete;

    void getInfo(PluginInfo& info) const;
    void    reportMissingInterface(const std::string& path, const std::string& name) const;

    DLLHandle getHandle() const
    {
        return dllHandle;
    }

protected:
    using Handle = void*;

    
    template <typename R, typename T>
    auto bindFunction(DLLHandle handle, const std::string& name) -> std::function<R(T&)>
    {
        if (auto funcPtr = _getFunction(handle, name))
        {
            return reinterpret_cast<R (*)(T)>(funcPtr);
        }
        reportMissingInterface(path, name);
        return std::function<R(T&)>();
    }

private:
    void* _getFunction(const DLLHandle& handle, std::string_view name);


    DLLHandle                        dllHandle;
    std::string                      path;
    std::function<void(PluginInfo&)> getInfoFunction;
};


} // namespace common