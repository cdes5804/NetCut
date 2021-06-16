#include "routes/ping.hpp"

using namespace Pistache;

void ping(const Rest::Request& request, Http::ResponseWriter response) {
    response.send(Http::Code::Ok, "1");
}