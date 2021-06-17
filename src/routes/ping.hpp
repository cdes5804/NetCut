#ifndef _PING_HPP
#define _PING_HPP

#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>

using namespace Pistache;

void ping(const Rest::Request& request, Http::ResponseWriter response);

#endif