//
// Created by 赵程 on 2021/3/4.
//

#include <opendht.h>
#include <vector>

#include <utils/Log.h>
#include <utils/StopWatch.h>

int main() {
    Stopwatch w;
    LOG::set_pattern("[%^%l%$] %v");
    LOG::set_level(LOG::level::trace);
    dht::DhtRunner node;

    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.
    node.run(4222, dht::crypto::generateIdentity(), true);
    LOG::trace("Init node. [{}ms]", w.elapsedMilliseconds());
    w.restart();

    // Join the network through any running node,
    // here using a known bootstrap node.
    node.bootstrap("127.0.0.1", "4222");
    LOG::trace("Link bootstrap. [{}ms]", w.elapsedMilliseconds());
    w.restart();

    // put some data on the dht
//    std::vector<uint8_t> some_data(5, 10);
    uint32_t some_data{510};
    node.put("unique_key", some_data);
    LOG::trace("put unique_key. [{}ms]", w.elapsedMilliseconds());
    w.restart();

    // put some data on the dht, signed with our generated private key
    node.putSigned("unique_key_42", some_data);
    LOG::trace("put unique_key_42. [{}ms]", w.elapsedMilliseconds());
    w.restart();

    // get data from the dht
    node.get("unique_key", [](const std::vector<std::shared_ptr<dht::Value>> &values) {
        // Callback called when values are found
        std::cout << "In cb <unique_key> \n";
        for (const auto &value : values) {
            std::cout << "Found value: " << *value << std::endl;
            auto data = value->data;

        }
        return true; // return false to stop the search
    });
    LOG::trace("get unique_key. [{}ms]", w.elapsedMilliseconds());
    w.restart();

    node.get(dht::InfoHash ("unique_key")).get();
    LOG::trace("get unique_key->value. [{}ms]", w.elapsedMilliseconds());
    w.restart();
    node.get("unique_key_42", [](const std::vector<std::shared_ptr<dht::Value>> &values) {
                 // Callback called when values are found
                 std::cout << "In cb <unique_key_42> \n";
                 for (const auto &value : values)
                     std::cout << "Found value: " << *value << std::endl;
                 return true; // return false to stop the search
             },
             [](bool success) {
                 std::cout << "Op went " << (success ? "cool" : "not cool") << std::endl;
             });
    LOG::trace("get unique_key_42. [{}ms]", w.elapsedMilliseconds());
    w.restart();
    node.get(dht::InfoHash("unique_key_42")).get();
    LOG::trace("get unique_key_42->value. [{}ms]", w.elapsedMilliseconds());
    w.restart();

//    dht::Where w;
//    w.id(5);/* the same as Where w("WHERE id=5"); */
//    node.get(dht::InfoHash::get("unique_key"),
//             [](const std::vector<std::shared_ptr<dht::Value>> &values) {
//                 for (const auto &v : values)
//                     std::cout << "This value has passed through the remotes filters " << *v << std::endl;
//                 return true; // keep looking for values
//             },
//             [](bool success) {
//                 std::cout << "Get finished with " << (success ? "success" : "failure") << std::endl;
//             });

    // wait for dht threads to end
    node.join();
    return 0;
}