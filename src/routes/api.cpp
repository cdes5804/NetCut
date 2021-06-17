#include "routes/api.hpp"
#include "routes/ping.hpp"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include <thread>
#include <memory>

using namespace Pistache;

void ApiEndpoint::setup_routes() {
    using namespace Rest;
    Routes::Get(router, "/ping", Routes::bind(&ping));
}

ApiEndpoint::ApiEndpoint(Address addr) : httpEndpoint(std::make_shared<Http::Endpoint>(addr)) {}

void ApiEndpoint::init(size_t num_threads) {
    auto opts = Http::Endpoint::options().threads(static_cast<int>(num_threads));
    httpEndpoint->init(opts);
    setup_routes();
}

void ApiEndpoint::start() {
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
}

void start_server(uint16_t listening_port) {
    Port port(listening_port);

    unsigned int num_threads = std::thread::hardware_concurrency();
    unsigned int server_num_threads = num_threads / 4;

    Address addr(Ipv4::any(), port);

    ApiEndpoint api(addr);

    api.init();
    api.start();
}