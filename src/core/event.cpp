#include "modloader/event.hpp"
#include "modloader/modloader.hpp"
#include "modloader/logger.hpp"

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace modloader {
namespace {

struct Subscription {
    EventSubscriptionId id;
    std::string event_name;
    std::string owner_id;
    EventCallback callback;
    void* user_data;
};

std::mutex event_mutex;
std::unordered_map<EventSubscriptionId, Subscription> subscriptions;
std::atomic<EventSubscriptionId> next_subscription_id{1};

bool valid_event_arguments(std::string_view event_name, std::string_view owner_id) {
    return !event_name.empty() && !owner_id.empty();
}

} // namespace

EventSubscriptionId subscribe_event(
    std::string_view event_name,
    std::string_view owner_id,
    EventCallback callback,
    void* user_data) {
    if (!is_initialized() || !valid_event_arguments(event_name, owner_id) || !callback) {
        return 0;
    }

    const EventSubscriptionId id = next_subscription_id.fetch_add(1);
    if (id == 0) {
        log(LogLevel::error, "Event subscription ID space exhausted.");
        return 0;
    }

    Subscription subscription{
        id,
        std::string(event_name),
        std::string(owner_id),
        callback,
        user_data
    };

    std::lock_guard<std::mutex> lock(event_mutex);
    subscriptions.emplace(id, std::move(subscription));
    return id;
}

bool unsubscribe_event(EventSubscriptionId subscription_id) {
    if (subscription_id == 0) {
        return false;
    }
    std::lock_guard<std::mutex> lock(event_mutex);
    return subscriptions.erase(subscription_id) != 0;
}

std::size_t unsubscribe_events_by_owner(std::string_view owner_id) {
    if (owner_id.empty()) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(event_mutex);
    std::size_t removed = 0;
    for (auto it = subscriptions.begin(); it != subscriptions.end();) {
        if (it->second.owner_id == owner_id) {
            it = subscriptions.erase(it);
            ++removed;
        } else {
            ++it;
        }
    }
    return removed;
}

std::size_t emit_event(
    std::string_view event_name,
    const void* data,
    std::size_t data_size) {
    if (!is_initialized() || event_name.empty() || (!data && data_size != 0)) {
        return 0;
    }

    std::vector<Subscription> snapshot;
    {
        std::lock_guard<std::mutex> lock(event_mutex);
        snapshot.reserve(subscriptions.size());
        for (const auto& [id, subscription] : subscriptions) {
            (void)id;
            if (subscription.event_name == event_name) {
                snapshot.push_back(subscription);
            }
        }
    }

    std::size_t invoked = 0;
    for (const auto& subscription : snapshot) {
        const Event event{
            subscription.event_name,
            data,
            data_size,
            subscription.user_data
        };
        try {
            subscription.callback(event);
            ++invoked;
        } catch (...) {
            log(LogLevel::error, "Event callback threw an exception.");
        }
    }
    return invoked;
}

} // namespace modloader
