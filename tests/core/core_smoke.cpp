#include "modloader/modloader.hpp"
#include "modloader/event.hpp"
#include "modloader/hook.hpp"
#include "modloader/mod.hpp"
#include "modloader/mod_runtime.hpp"

namespace {
int event_calls = 0;
int received_value = 0;
void* received_user_data = nullptr;

void on_test_event(const modloader::Event& event) {
    ++event_calls;
    received_user_data = event.user_data;
    if (event.data && event.data_size == sizeof(int)) {
        received_value = *static_cast<const int*>(event.data);
    }
}
} // namespace

int main() {
    if (modloader::is_initialized()) {
        return 1;
    }
    if (!modloader::initialize() || !modloader::is_initialized()) {
        return 2;
    }
    if (!modloader::initialize()) {
        return 3;
    }

    // Event dispatch, individual unsubscribe, and owner cleanup.
    int subscriber_data = 42;
    const auto event_a = modloader::subscribe_event(
        "test.ready", "sample.mod", on_test_event, &subscriber_data);
    const auto event_b = modloader::subscribe_event(
        "test.ready", "sample.other", on_test_event);
    if (event_a == 0 || event_b == 0 || event_a == event_b) {
        return 22;
    }
    const int event_value = 1234;
    if (modloader::emit_event("test.ready", &event_value, sizeof(event_value)) != 2
        || event_calls != 2 || received_value != event_value) {
        return 23;
    }
    if (!modloader::unsubscribe_event(event_a)
        || modloader::unsubscribe_event(event_a)) {
        return 24;
    }
    if (modloader::unsubscribe_events_by_owner("sample.other") != 1
        || modloader::emit_event("test.ready") != 0) {
        return 25;
    }
    if (modloader::subscribe_event("", "sample.mod", on_test_event) != 0
        || modloader::subscribe_event("test.ready", "", on_test_event) != 0
        || modloader::subscribe_event("test.ready", "sample.mod", nullptr) != 0) {
        return 26;
    }

    const modloader::ModMetadata sample{"sample.mod", "Sample Mod", "1.0.0"};
    if (!modloader::register_mod(sample)) {
        return 4;
    }
    if (modloader::register_mod(sample)) {
        return 5; // duplicate ID must be rejected
    }
    if (modloader::register_mod({"Bad ID", "Invalid", "1.0"})) {
        return 6; // IDs cannot contain spaces or uppercase characters
    }
    if (modloader::register_mod({"missing.version", "Missing version", ""})) {
        return 7;
    }

    const auto found = modloader::find_mod("sample.mod");
    if (!found || found->name != "Sample Mod" || found->version != "1.0.0") {
        return 8;
    }
    if (modloader::find_mod("not.registered")) {
        return 9;
    }
    const auto mods = modloader::registered_mods();
    if (mods.size() != 1 || mods.front().id != "sample.mod") {
        return 10;
    }

    // Exercise the complete native module lifecycle against a tiny test .so.
    if (!modloader::load_mod_library(SAMPLE_MOD_PATH)) {
        return 11;
    }
    if (!modloader::is_mod_loaded("sample.native")) {
        return 12;
    }
    const auto loaded = modloader::loaded_mods();
    if (loaded.size() != 1 || loaded.front().id != "sample.native") {
        return 13;
    }
    const auto native_metadata = modloader::find_mod("sample.native");
    if (!native_metadata || native_metadata->name != "Sample Native Mod") {
        return 14;
    }
    if (modloader::load_mod_library(SAMPLE_MOD_PATH)) {
        return 15; // duplicate native module must not load twice
    }
    if (!modloader::unload_mod("sample.native")) {
        return 16;
    }
    if (modloader::is_mod_loaded("sample.native")
        || modloader::find_mod("sample.native")) {
        return 17; // unload removes runtime state and metadata
    }
    if (modloader::unload_mod("sample.native")) {
        return 18; // repeated unload is rejected
    }

    // Host builds intentionally expose no native game-hook backend.
    if (modloader::initialize_hook_backend()) {
        return 19;
    }
    if (modloader::hook_function(nullptr, nullptr, nullptr) != nullptr) {
        return 20;
    }
    if (modloader::unhook_function(nullptr)) {
        return 21;
    }
    return 0;
}
