#include "modloader/mod_runtime.hpp"
#include "modloader/event.hpp"
#include "modloader/mod_api.hpp"
#include "modloader/modloader.hpp"
#include "modloader/logger.hpp"
#include "mod_registry_internal.hpp"

#include <dlfcn.h>

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace modloader {
namespace {

struct LoadedModule {
    ModMetadata metadata;
    void* handle = nullptr;
    ModUnloadFunction unload = nullptr;

    ~LoadedModule() {
        if (handle) {
            dlclose(handle);
        }
    }
};

std::mutex modules_mutex;
std::unordered_map<std::string, std::shared_ptr<LoadedModule>> modules;
std::unordered_set<std::string> loading_ids;

template <typename Function>
Function symbol(void* handle, const char* name) {
    dlerror();
    void* address = dlsym(handle, name);
    if (dlerror() != nullptr) {
        return nullptr;
    }
    return reinterpret_cast<Function>(address);
}

} // namespace

bool load_mod_library(const std::string& path) {
    if (!is_initialized() || path.empty()) {
        log(LogLevel::warning, "Cannot load mod: runtime is not initialized or path is empty.");
        return false;
    }

    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        const char* error = dlerror();
        log(LogLevel::error, error ? error : "dlopen failed for mod library.");
        return false;
    }

    const auto info = symbol<ModInfoFunction>(handle, "coolmodloader_mod_info");
    const auto load = symbol<ModLoadFunction>(handle, "coolmodloader_mod_load");
    const auto unload = symbol<ModUnloadFunction>(handle, "coolmodloader_mod_unload");
    if (!info || !load || !unload) {
        log(LogLevel::error, "Mod is missing one or more required CoolModLoader exports.");
        dlclose(handle);
        return false;
    }

    const ModDescriptor* descriptor = info();
    if (!descriptor || descriptor->abi_version != mod_abi_version
        || !descriptor->id || !descriptor->name || !descriptor->version) {
        log(LogLevel::error, "Mod descriptor is null, incomplete, or uses an unsupported ABI.");
        dlclose(handle);
        return false;
    }

    ModMetadata metadata{descriptor->id, descriptor->name, descriptor->version};
    auto module = std::make_shared<LoadedModule>();
    module->metadata = metadata;
    module->handle = handle;
    module->unload = unload;

    {
        std::lock_guard<std::mutex> lock(modules_mutex);
        if (modules.find(metadata.id) != modules.end()
            || loading_ids.find(metadata.id) != loading_ids.end()) {
            log(LogLevel::warning, "Mod is already loaded or currently loading.");
            return false; // module RAII closes handle
        }
        loading_ids.insert(metadata.id);
    }

    if (!register_mod(metadata)) {
        std::lock_guard<std::mutex> lock(modules_mutex);
        loading_ids.erase(metadata.id);
        return false;
    }

    int load_result = 0;
    try {
        load_result = load();
    } catch (...) {
        log(LogLevel::error, "Mod onLoad callback threw an exception.");
    }

    if (load_result == 0) {
        detail::unregister_mod(metadata.id);
        std::lock_guard<std::mutex> lock(modules_mutex);
        loading_ids.erase(metadata.id);
        log(LogLevel::error, "Mod onLoad rejected initialization.");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(modules_mutex);
        loading_ids.erase(metadata.id);
        modules.emplace(metadata.id, std::move(module));
    }

    log(LogLevel::info, "Native mod loaded successfully.");
    return true;
}

bool unload_mod(const std::string& id) {
    std::shared_ptr<LoadedModule> module;
    {
        std::lock_guard<std::mutex> lock(modules_mutex);
        const auto it = modules.find(id);
        if (it == modules.end()) {
            return false;
        }
        module = std::move(it->second);
        modules.erase(it);
    }

    // Remove the mod's event callbacks before its code is unloaded.
    unsubscribe_events_by_owner(id);

    try {
        if (module->unload) {
            module->unload();
        }
    } catch (...) {
        log(LogLevel::error, "Mod onUnload callback threw an exception.");
    }

    detail::unregister_mod(id);
    module.reset(); // closes the shared library after its callback
    log(LogLevel::info, "Native mod unloaded.");
    return true;
}

bool unload_all_mods() {
    std::vector<std::string> ids;
    {
        std::lock_guard<std::mutex> lock(modules_mutex);
        ids.reserve(modules.size());
        for (const auto& entry : modules) {
            ids.push_back(entry.first);
        }
    }

    bool success = true;
    for (const auto& id : ids) {
        if (!unload_mod(id)) {
            success = false;
        }
    }
    return success;
}

std::vector<ModMetadata> loaded_mods() {
    std::lock_guard<std::mutex> lock(modules_mutex);
    std::vector<ModMetadata> result;
    result.reserve(modules.size());
    for (const auto& entry : modules) {
        result.push_back(entry.second->metadata);
    }
    return result;
}

bool is_mod_loaded(std::string_view id) {
    std::lock_guard<std::mutex> lock(modules_mutex);
    return modules.find(std::string(id)) != modules.end();
}

} // namespace modloader
