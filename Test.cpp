#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gmock/gmock.h>
#include <thread>

#include "DNSCache_thread_safe.h"
#include "DNSCache.h"

using namespace testing;

using DNSCache_mutex = DNSCache_thread_safe;

class Test_inspector {
public:
    static DNSCache_mutex create_DNSCache_mutex(size_t max_size) {
        return DNSCache_mutex(max_size);
    }

    DNSCache_mutex& cache;

    void print() {
        for(auto& item : cache.name_ip_mapping)
            std::cout << "key: " << item.first << " value: " << item.second.ip << "\n";
    }

    int get_index(const std::string& name) {
        auto it = cache.name_ip_mapping.find(name);
        if(it == cache.name_ip_mapping.end()) return -1;
        int result{};
        auto* p_value = cache.get_name_ip_value_by_elem(cache.head);
        while(p_value->first != name) {
            ++result;
            p_value = cache.get_name_ip_value_by_elem(p_value->second.next);
        }
        return result;
    }
};


struct DnsCacheSuit: public Test {
    DNSCache_mutex cache = Test_inspector::create_DNSCache_mutex(3);
    Test_inspector ti{cache};
};

TEST_F(DnsCacheSuit, AddOneTwoThreeAndSearch) {
    cache.update("one.org", "192.168.1.0");
    ASSERT_THAT(cache.resolve("one.org"), Eq("192.168.1.0"));
    ASSERT_THAT(ti.get_index("one.org"), Eq(0));
    cache.update("two.org", "192.168.1.1");
    ASSERT_THAT(cache.resolve("two.org"), Eq("192.168.1.1"));
    cache.update("three.org", "192.168.1.2");
    ASSERT_THAT(cache.resolve("three.org"), Eq("192.168.1.2"));
}

TEST_F(DnsCacheSuit, DeletedLast) {
    cache.update("one.org", "192.168.1.0");
    cache.update("two.org", "192.168.1.1");
    cache.update("three.org", "192.168.1.2");
    cache.update("four.org", "192.168.1.3");
    ASSERT_THAT(cache.resolve("one.org"), Eq(""));
    ASSERT_THAT(cache.resolve("two.org"), Eq("192.168.1.1"));
    ASSERT_THAT(cache.resolve("three.org"), Eq("192.168.1.2"));
    ASSERT_THAT(cache.resolve("four.org"), Eq("192.168.1.3"));
}

TEST_F(DnsCacheSuit, DeletedLastResolved) {
    cache.update("one.org", "192.168.1.0");
    cache.update("two.org", "192.168.1.1");
    cache.update("three.org", "192.168.1.2");
    ASSERT_THAT(cache.resolve("one.org"), Eq("192.168.1.0"));
    ASSERT_THAT(cache.resolve("two.org"), Eq("192.168.1.1"));
    cache.update("four.org", "192.168.1.3");
    ASSERT_THAT(cache.resolve("four.org"), Eq("192.168.1.3"));
    ASSERT_THAT(cache.resolve("three.org"), Eq(""));
}

TEST_F(DnsCacheSuit, UpdateFirstMiddleLast) {
    cache.update("one.org", "192.168.1.0");
    cache.update("two.org", "192.168.1.1");
    cache.update("three.org", "192.168.1.2");

    cache.update("one.org", "192.168.1.0");
    cache.update("two.org", "192.168.1.1"); // two.org one.org three.org
    cache.update("three.org", "192.168.1.2");
    ASSERT_THAT(cache.resolve("one.org"), Eq("192.168.1.0"));
    ASSERT_THAT(cache.resolve("two.org"), Eq("192.168.1.1"));
    ASSERT_THAT(cache.resolve("three.org"), Eq("192.168.1.2"));
}

TEST_F(DnsCacheSuit, Singleton) {
    DNSCache_mutex& instance_1 = DNSCache_mutex::get_instance(10);
    DNSCache_mutex& instance_2 = DNSCache_mutex::get_instance();
    ASSERT_THAT(&instance_1, Eq(&instance_2));
}

struct DNSCacheMultithreadSuit: public Test {
    DNSCache_mutex cache = Test_inspector::create_DNSCache_mutex(5);

    void test_resolve_update();
    void work();
};

void DNSCacheMultithreadSuit::test_resolve_update() {
    for(int i = 0; i < 5; ++i) {
        std::string test_ip = std::to_string(i);
        cache.resolve(test_ip);
        cache.update(test_ip, test_ip);
    }
}

void DNSCacheMultithreadSuit::work() {
    for(int i = 0; i < 1'000'000; ++i)
        test_resolve_update();
}

TEST_F(DNSCacheMultithreadSuit, DISABLED_test_1_000_000) {
    std::thread thread_1(&DNSCacheMultithreadSuit::work, this);
    std::thread thread_2(&DNSCacheMultithreadSuit::work, this);
    thread_1.join();
    thread_2.join();
}

#pragma clang diagnostic pop