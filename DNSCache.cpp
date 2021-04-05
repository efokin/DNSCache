
#include "DNSCache.h"
#include <cassert>

DNSCache::DNSCache(size_t max_size):
        max_size{max_size} {
    if(max_size <= 0) throw std::runtime_error("DNSCache should have positive size");
    name_ip_mapping.reserve(max_size);
}

void DNSCache::update_it(Name_ip_map::iterator it, const std::string& name, const std::string& ip) {
    if(it != name_ip_mapping.end()) {
        it->second.ip = ip;
        update_links(it);
        return;
    }
    if(!is_filled()) {
        add_new_ip(name, ip);
        return;
    }
    update_last_elem(name, ip);
}

bool DNSCache::is_filled() const {
    return name_ip_mapping.size() >= max_size;
}

std::string DNSCache::resolve_it(DNSCache::Name_ip_map::iterator it) {
    update_links(it);
    return it->second.ip;
}

DNSCache::Name_ip_map::iterator DNSCache::get_it_by_name(const std::string& name) {
    return name_ip_mapping.find(name);
}

void DNSCache::update_links(Name_ip_map::iterator it) {
    auto p_prev_node = it->second.prev;
    auto p_next_node = it->second.next;
    if(p_prev_node) p_prev_node->next = p_next_node;
    if(p_next_node) p_next_node->prev = p_prev_node;
    if(!p_next_node && p_prev_node) tail = p_prev_node;
    it->second.next = head;
    head->prev = &it->second;
    head = &it->second;
    it->second.prev = nullptr;
    assert(head && tail);
    assert(name_ip_mapping.size() <= max_size);
}

void DNSCache::add_new_ip(const std::string& name, const std::string& ip) {
    bool cache_empty = name_ip_mapping.empty();
    auto[it, emplaced] = name_ip_mapping.emplace(std::piecewise_construct,
                                                 std::forward_as_tuple(name),
                                                 std::forward_as_tuple(ip));
    assert(emplaced);
    if(cache_empty) {
        head = &it->second;
        tail = &it->second;
        return;
    }

    it->second.next = head;
    head->prev = &it->second;
    head = &it->second;
}

void DNSCache::update_last_elem(const std::string& name, const std::string& ip) {
    auto* p_value = get_name_ip_value_by_elem(tail);
    assert(name_ip_mapping.find(p_value->first) != name_ip_mapping.end());
    auto node = name_ip_mapping.extract(p_value->first);
    node.key() = name;
    node.mapped().ip = ip;
    auto irt = name_ip_mapping.insert(std::move(node));
    assert(irt.inserted);
    update_links(irt.position);
}


DNSCache::Name_ip_map::value_type* DNSCache::get_name_ip_value_by_elem(DNSCache::Elem* elem_p) const {
    return (typename DNSCache::Name_ip_map::value_type*)
            ((char*) elem_p
             + offsetof(DNSCache::Name_ip_map::value_type, first)
             - offsetof(DNSCache::Name_ip_map::value_type, second));
}

bool DNSCache::end(Name_ip_map::iterator it) {
    return it == name_ip_mapping.end();
}
