
#ifndef POSITIVE_TASK_DNSCACHE_SHARED_MUTEX_H
#define POSITIVE_TASK_DNSCACHE_SHARED_MUTEX_H

#include "DNSCache.h"
#include <shared_mutex>

class DNSCache_shared_mutex: public DNSCache {
public:

    static DNSCache_shared_mutex& get_instance(size_t max_size = 0);

    void update(const std::string& name, const std::string& ip);
    std::string resolve(const std::string& name);

private:
    explicit DNSCache_shared_mutex(size_t max_size);
    std::shared_mutex shared_mutex;
};


#endif //POSITIVE_TASK_DNSCACHE_SHARED_MUTEX_H
