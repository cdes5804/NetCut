#ifndef _API_HPP
#define _API_HPP

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

  public:
    explicit ApiEndpoint(Address addr);
    void init(size_t num_threads=2);
    void start();
};

void start_server(uint16_t listening_port);

#endif