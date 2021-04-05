
#include "DNSCache_shared_mutex.h"

DNSCache_shared_mutex& DNSCache_shared_mutex::get_instance(size_t max_size) {
    static DNSCache_shared_mutex cache(max_size);
    return cache;
}

void DNSCache_shared_mutex::update(const std::string& name, const std::string& ip) {
    std::unique_lock<std::shared_mutex> ul(shared_mutex);
    auto it = DNSCache::get_it_by_name(name);
    update_it(it, name, ip);
}

std::string DNSCache_shared_mutex::resolve(const std::string& name) {
    std::shared_lock<std::shared_mutex> sl(shared_mutex);
    auto it = DNSCache::get_it_by_name(name);
    if(end(it)) return {};

    sl.unlock();
    std::unique_lock<std::shared_mutex> ul(shared_mutex);
    it = DNSCache::get_it_by_name(name);
    if(end(it)) return {};
    return resolve_it(it);
}

DNSCache_shared_mutex::DNSCache_shared_mutex(size_t max_size): DNSCache(max_size) {}
