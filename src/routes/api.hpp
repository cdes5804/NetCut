#ifndef _API_HPP
#define _API_HPP

#include "models/controller.hpp"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include <cstdint>
#include <cstddef>
#include <memory>

using namespace Pistache;

class ApiEndpoint {
  private:
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
    void setup_routes();
    Controller controller;

  public:
    explicit ApiEndpoint(Address addr, const Controller &controller);
    void init(size_t num_threads=2);
    void start();
    void ping(const Rest::Request& request, Http::ResponseWriter response);
    void quit(const Rest::Request& request, Http::ResponseWriter response);
    void get_targets(const Rest::Request& request, Http::ResponseWriter response);
    void action(const Rest::Request& request, Http::ResponseWriter response);
    void get_status(const Rest::Request& request, Http::ResponseWriter response);
};

void start_server(uint16_t listening_port, const Controller &controller);

#endif