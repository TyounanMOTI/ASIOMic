#pragma once
#include <memory>
#include <asiodrivers.h>
#include "debug.h"
#include "device.h"

bool loadAsioDriver(char *name); // defined in asiodrivers.cpp
extern AsioDrivers* asioDrivers;

namespace asio_mic {

extern std::unique_ptr<debug> g_debug;
extern std::unique_ptr<device> g_device;

}
