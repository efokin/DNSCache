#ifndef POSITIVE_TASK_DNSCACHE_H
#define POSITIVE_TASK_DNSCACHE_H

#include <string>
#include <unordered_map>
#include "DNSCache.h"

class DNSCache {
public:
    explicit DNSCache(size_t max_size);

private:
    struct Elem {
        Elem(std::string ip, Elem* prev = nullptr, Elem* next = nullptr):
                ip{ip}, prev{prev}, next{prev} {}

        std::string ip;
        Elem* prev{};
        Elem* next{};
    };

    using Name_ip_map = std::unordered_map<std::string, Elem>;

    bool is_filled() const;
    void update_links(Name_ip_map::iterator it);
    void add_new_ip(const std::string& name, const std::string& ip);
    void update_last_elem(const std::string& name, const std::string& ip);
    Name_ip_map::value_type* get_name_ip_value_by_elem(DNSCache::Elem* elem_p) const;

    const size_t max_size;
    Elem* head{};
    Elem* tail{};
    Name_ip_map name_ip_mapping;

protected:
    bool end(Name_ip_map::iterator it);
    Name_ip_map::iterator get_it_by_name(const std::string& name);
    std::string resolve_it(Name_ip_map::iterator it);
    void update_it(Name_ip_map::iterator it, const std::string& name, const std::string& ip);

    friend class Test_inspector;
};


#endif //POSITIVE_TASK_DNSCACHE_H
