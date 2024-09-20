#pragma once

#ifndef REQUESTS_H
#define REQUESTS_H
#include "common.h"

namespace Requests {
	std::string GetRequest(const char* url, const char* useragent, const char* path = "", const char* ContentType = "");
}

#endif