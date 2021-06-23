#ifndef _API_HPP
#define _API_HPP

#include "models/controller.hpp"

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

#include <cstdint>
#include <memory>

using namespace Pistache;

class ApiEndpoint {
  private:
    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
    Controller controller;

    // Setup the APIs.
    void setup_routes();

  public:
    explicit ApiEndpoint(Address addr, const Controller &controller);
    // Initialize the server, setting threads and routes.
    void init(size_t num_threads=2);

    // Start serving the application.
    void start();

    // Route for GET /ping, used to test if the application is alive.
    void ping(const Rest::Request& request, Http::ResponseWriter response);

    // Route for POST /quit, used to clean up the resources and quit.
    void quit(const Rest::Request& request, Http::ResponseWriter response);

    // Route for GET /get_targets, used to retrive available targets in the network.
    void get_targets(const Rest::Request& request, Http::ResponseWriter response);

    // Route for POST /action/:ip, used to take action against the specified target.
    void action(const Rest::Request& request, Http::ResponseWriter response);

    // Route for POST /get_status/:ip, used to retrieve the status, either attacked or normal, of the specified target.
    void get_status(const Rest::Request& request, Http::ResponseWriter response);
};


// Create a ApiEndpoint instance, initialize it with the provided arguments, and start the server.
// We should use this function to start the server instead of creating a ApiEndpoint object dircectly.
void start_server(uint16_t listening_port, const Controller &controller, const unsigned int num_threads=2);

#endif