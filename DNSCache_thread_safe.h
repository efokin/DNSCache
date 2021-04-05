#ifndef DNSCACHE_THREAD_SAFE_H
#define DNSCACHE_THREAD_SAFE_H

#include "DNSCache.h"
#include <mutex>

class DNSCache_thread_safe: public DNSCache {
public:
    static DNSCache_thread_safe& get_instance(size_t max_size = 0);

    void update(const std::string& name, const std::string& ip);
    std::string resolve(const std::string& name);

private:
    explicit DNSCache_thread_safe(size_t max_size);
    std::mutex mutex;

    friend class Test_inspector;
};

#endif //DNSCACHE_THREAD_SAFE_H
