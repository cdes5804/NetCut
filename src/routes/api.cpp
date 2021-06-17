#include "routes/api.hpp"
#include "models/controller.hpp"
#include "models/host.hpp"
#include "utils/json.hpp"
#include "utils/socket_utils.hpp"
#include "utils/thread_utils.hpp"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/http_headers.h>

#include <thread>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>

using namespace Pistache;
using json = nlohmann::json;

void start_server(uint16_t listening_port, const Controller &controller) {
    Port port(listening_port);

    unsigned int num_threads = std::thread::hardware_concurrency();
    unsigned int server_num_threads = std::max(1U, num_threads / 4);

    Address addr(Ipv4::any(), port);

    ApiEndpoint api(addr, controller);

    api.init(server_num_threads);
    api.start();
}

void ApiEndpoint::setup_routes() {
    using namespace Rest;
    Routes::Get(router, "/ping", Routes::bind(&ApiEndpoint::ping, this));
    Routes::Get(router, "/get_targets", Routes::bind(&ApiEndpoint::get_targets, this));
    Routes::Get(router, "/get_status/:ip", Routes::bind(&ApiEndpoint::get_status, this));
    Routes::Post(router, "/action/:ip", Routes::bind(&ApiEndpoint::action, this));
    Routes::Post(router, "/quit", Routes::bind(&ApiEndpoint::quit, this));
}

ApiEndpoint::ApiEndpoint(Address addr, const Controller &controller)
            : httpEndpoint(std::make_shared<Http::Endpoint>(addr)), controller(controller) {}

void ApiEndpoint::init(size_t num_threads) {
    auto opts = Http::Endpoint::options().threads(static_cast<int>(num_threads));
    httpEndpoint->init(opts);
    setup_routes();
}

void ApiEndpoint::start() {
    httpEndpoint->setHandler(router.handler());
    httpEndpoint->serve();
}

// routes
void ApiEndpoint::ping(const Rest::Request& request, Http::ResponseWriter response) {
    response.send(Http::Code::Ok, "OK");
}

void ApiEndpoint::quit(const Rest::Request& request, Http::ResponseWriter response) {
    controller.recover_all_hosts(); // being nice by recovering the network for targets before exiting
    Socket::close_sockets();
    Thread::stop_all_threads();
    auto res = response.send(Http::Code::Ok, "OK");
    res.then(
        [](ssize_t bytes) { exit(EXIT_SUCCESS); },
        Async::NoExcept
    );
}

void ApiEndpoint::get_targets(const Rest::Request& request, Http::ResponseWriter response) {
    controller.scan_targets();
    std::vector<Host> hosts = controller.get_targets();

    std::vector<std::map<std::string, std::string>> v;

    for (const Host &host : hosts) {
        v.push_back({
            { "IP Address", host.get_ip() },
            { "MAC Address", host.get_mac() },
            { "Status", host.is_cut() ? "Cut" : "Normal" },
        });
    }

    json j_vec(v);

    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(Http::Code::Ok, j_vec.dump());
}

void ApiEndpoint::action(const Rest::Request& request, Http::ResponseWriter response) {
    std::string target_ip = request.param(":ip").as<std::string>();

    ACTION_STATUS status = controller.action(target_ip);
    std::map<std::string, std::string> res;
    auto http_return_code = Http::Code::Ok; // default

    res["Target"] = target_ip;
    
    switch (status) {
      case ACTION_STATUS::TARGET_NOT_FOUND:
        res["Status"] = "Target Not Found";
        http_return_code = Http::Code::Not_Found;
        break;
      case ACTION_STATUS::CUT_SUCCESS:
        res["Status"] = "Cut";
        break;
      case ACTION_STATUS::RECOVER_SUCCESS:
        res["Status"] = "Recovered";
        break;
    }

    json j_map(res);

    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(http_return_code, j_map.dump());
}

void ApiEndpoint::get_status(const Rest::Request& request, Http::ResponseWriter response) {
    std::string target_ip = request.param(":ip").as<std::string>();

    controller.scan_targets();
    Host host = controller.get_host(target_ip);
    std::map<std::string, std::string> res;
    auto http_return_code = Http::Code::Ok; // default

    res["Target"] = target_ip;

    switch (host.get_status()) {
      case Status::NOT_EXIST:
        res["Status"] = "Target Not Found";
        http_return_code = Http::Code::Not_Found;
        break;
      case Status::CUT:
        res["Status"] = "Cut";
        break;
      case Status::NORMAL:
        res["Status"] = "Normal";
        break;
    }

    json j_map(res);

    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
    response.send(http_return_code, j_map.dump());
}