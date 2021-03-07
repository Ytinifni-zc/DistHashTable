//
// Created by 赵程 on 2021/3/7.
//

#include <opendht.h>
#include <vector>

#include <utils/Log.h>
#include <utils/StopWatch.h>

#include "gtest/gtest.h"

namespace {
    using namespace std::chrono_literals;
    class OpenDHTTest : public ::testing::Test {
    public:
        Stopwatch w;
        dht::DhtRunner node1, node2;

        OpenDHTTest() {}

        virtual ~OpenDHTTest() {}

        virtual void SetUp() {
            LOG::set_pattern("[%^%l%$] %v");
            LOG::set_level(LOG::level::trace);

            dht::DhtRunner::Config config;
            config.dht_config.node_config.max_peer_req_per_sec = -1;
            config.dht_config.node_config.max_req_per_sec = -1;

            node1.run(42222, config);
            node2.run(42232, config);
            node2.bootstrap(node1.getBound());
        }

        virtual void TearDown() {
            unsigned done{0};
            std::condition_variable cv;
            std::mutex cv_m;
            auto shutdown = [&] {
                std::lock_guard<std::mutex> lk(cv_m);
                done++;
                cv.notify_all();
            };
            node1.shutdown(shutdown);
            node2.shutdown(shutdown);
            std::unique_lock<std::mutex> lk(cv_m);
            ASSERT_TRUE(cv.wait_for(lk, 5s, [&] { return done == 2; }));
            node1.join();
            node2.join();
        }

    };
}

TEST_F(OpenDHTTest, Constrctors) {
    ASSERT_TRUE(node1.getBoundPort() == 42222);
    ASSERT_TRUE(node2.getBoundPort() == 42232);
}

TEST_F(OpenDHTTest, GetPut) {
    w.restart();
    auto key = dht::InfoHash::get("123");
    dht::Value val {"hey"};
    auto val_data = val.data;
    std::promise<bool> p;
    node2.put(key, std::move(val), [&](bool ok){
        p.set_value(ok);
    });
    ASSERT_TRUE(p.get_future().get());
    LOG::info("Set Value: [OK]. ({}ms)", w.elapsedMilliseconds());
    auto vals = node1.get(key).get();
    ASSERT_FALSE(vals.empty());
    ASSERT_TRUE(vals.front()->data == val_data);
    LOG::info("Get Value: [OK]. ({}ms)", w.elapsedMilliseconds());
}

//TEST_F(OpenDHTTest, Listen) {
//    std::mutex mutex;
//    std::condition_variable cv;
//    std::atomic_uint valueCount(0);
//    unsigned putCount(0);
//    unsigned putOkCount(0);
//
//    auto a = dht::InfoHash::get("234");
//    auto b = dht::InfoHash::get("2345");
//    auto c = dht::InfoHash::get("23456");
//    auto d = dht::InfoHash::get("234567");
//    constexpr unsigned N = 256;
//    constexpr unsigned SZ = 56 * 1024;
//
//    auto ftokena = node1.listen(a, [&](const std::shared_ptr<dht::Value>&) {
//        valueCount++;
//        return true;
//    });
//
//    auto ftokenb = node1.listen(b, [&](const std::shared_ptr<dht::Value>&) {
//        valueCount++;
//        return false;
//    });
//
//    auto ftokenc = node1.listen(c, [&](const std::shared_ptr<dht::Value>&) {
//        valueCount++;
//        return true;
//    });
//
//    auto ftokend = node1.listen(d, [&](const std::shared_ptr<dht::Value>&) {
//        valueCount++;
//        return true;
//    });
//
//    std::vector<uint8_t> mtu;
//    mtu.reserve(SZ);
//    for (size_t i = 0; i < SZ; i++)
//        mtu.emplace_back((i % 2) ? 'T' : 'M');
//
//    for (unsigned i=0; i<N; i++) {
//        node2.put(a, dht::Value("v1"), [&](bool ok) {
//            std::lock_guard<std::mutex> lock(mutex);
//            putCount++;
//            if (ok) putOkCount++;
//            cv.notify_all();
//        });
//        node2.put(b, dht::Value("v2"), [&](bool ok) {
//            std::lock_guard<std::mutex> lock(mutex);
//            putCount++;
//            if (ok) putOkCount++;
//            cv.notify_all();
//        });
//        auto bigVal = std::make_shared<dht::Value>();
//        bigVal->data = mtu;
//        node2.put(c, bigVal, [&](bool ok) {
//            std::lock_guard<std::mutex> lock(mutex);
//            putCount++;
//            if (ok) putOkCount++;
//            cv.notify_all();
//        });
//    }
//
//    {
//        std::unique_lock<std::mutex> lk(mutex);
//        CPPUNIT_ASSERT(cv.wait_for(lk, 30s, [&]{ return putCount == N * 3u; }));
//        CPPUNIT_ASSERT_EQUAL(N * 3u, putOkCount);
//    }
//
//    CPPUNIT_ASSERT(ftokena.valid());
//    CPPUNIT_ASSERT(ftokenb.valid());
//    CPPUNIT_ASSERT(ftokenc.valid());
//    CPPUNIT_ASSERT(ftokend.valid());
//
//    auto tokena = ftokena.get();
//    auto tokenc = ftokenc.get();
//    auto tokend = ftokend.get();
//    // tokenb might be 0 since the callback returns false.
//
//    CPPUNIT_ASSERT(tokena);
//    CPPUNIT_ASSERT(tokenc);
//    CPPUNIT_ASSERT(tokend);
//    CPPUNIT_ASSERT_EQUAL(N * 2u + 1u, valueCount.load());
//
//    node1.cancelListen(a, tokena);
//    node1.cancelListen(b, std::move(ftokenb));
//    node1.cancelListen(c, tokenc);
//    node1.cancelListen(d, tokend);
//}

//int _tmain(int argc, char* argv[]) {
//    testing::AddGlobalTestEnvironment(new OpenDHTTest);
//    return 0;
//}