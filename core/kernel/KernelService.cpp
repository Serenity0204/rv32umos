#include "KernelService.hpp"

std::unordered_map<std::string, void*> KernelService::services;

void KernelService::registerService(const std::string& name, void* serviceInstance)
{
    KernelService::services[name] = serviceInstance;
}

bool KernelService::exist(const std::string& name)
{
    return KernelService::services.count(name) > 0;
}

void KernelService::clear()
{
    KernelService::services.clear();
}