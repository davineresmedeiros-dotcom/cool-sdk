#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace modloader {

using EventSubscriptionId = std::uint64_t;

// Event data is borrowed and valid only for the duration of emit_event().
struct Event {
    std::string_view name;
    const void* data = nullptr;
    std::size_t data_size = 0;
    void* user_data = nullptr;
};

using EventCallback = void (*)(const Event&);

// Subscribe to a named event. owner_id should be the subscribing mod's ID.
// Returns 0 if arguments are invalid or the runtime is not initialized.
EventSubscriptionId subscribe_event(
    std::string_view event_name,
    std::string_view owner_id,
    EventCallback callback,
    void* user_data = nullptr);

// Remove one subscription. Returns false if its ID is unknown.
bool unsubscribe_event(EventSubscriptionId subscription_id);

// Remove all subscriptions belonging to a mod owner.
std::size_t unsubscribe_events_by_owner(std::string_view owner_id);

// Synchronously dispatch an event to a snapshot of current subscribers.
// Returns the number of callbacks successfully invoked.
std::size_t emit_event(
    std::string_view event_name,
    const void* data = nullptr,
    std::size_t data_size = 0);

} // namespace modloader
