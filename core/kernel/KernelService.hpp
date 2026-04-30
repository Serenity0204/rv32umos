#pragma once

#include <string>
#include <unordered_map>

class KernelService
{
private:
    static std::unordered_map<std::string, void*> services;

    KernelService() = default;
    ~KernelService() = default;

public:
    static void registerService(const std::string& name, void* serviceInstance);

    template <typename T>
    static T* get(const std::string& name);
    static bool exist(const std::string& name);
    static void clear();
};

template <typename T>
T* KernelService::get(const std::string& name)
{
    if (!KernelService::services.count(name)) return nullptr;
    return static_cast<T*>(KernelService::services.at(name));
}
