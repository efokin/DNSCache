#include "DNSCache_thread_safe.h"
#include "iostream"

DNSCache_thread_safe& DNSCache_thread_safe::get_instance(size_t max_size) {
    static DNSCache_thread_safe cache(max_size);
    return cache;
}

DNSCache_thread_safe::DNSCache_thread_safe(size_t max_size):
        DNSCache(max_size) {

}

void DNSCache_thread_safe::update(const std::string& name, const std::string& ip) {
    std::unique_lock ul(mutex);
    auto it = get_it_by_name(name);
    DNSCache::update_it(it, name, ip);
}

std::string DNSCache_thread_safe::resolve(const std::string& name) {
    std::unique_lock ul(mutex);
    auto it = get_it_by_name(name);
    if(end(it)) return {};
    return resolve_it(it);
}

