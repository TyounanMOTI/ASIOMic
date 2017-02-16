#include <memory>
#include <array>
#include <IUnityInterface.h>
#include <asiosys.h>
#include <asio.h>
#include <algorithm>
#include "runtime_error.h"
#include "globals.h"
#include "asiodrivers.h"

using namespace asio_mic;

namespace {

const size_t asio_driver_name_length = 32;

}

extern "C" {

  int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetDriverNameMaxLength()
  {
    return asio_driver_name_length;
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetDeviceList(char** driver_names, long max_num_drivers)
  {
    if (!asioDrivers) {
      asioDrivers = new AsioDrivers();
    }
    asioDrivers->getDriverNames(driver_names, max_num_drivers);
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetDebugLogFunc(debug::log_func func)
  {
    g_debug = std::make_unique<debug>(func);
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API Initialize(const char* driver_name)
  {
    if (!asioDrivers) {
      auto succeed = loadAsioDriver(const_cast<char*>(driver_name));
      if (!succeed) {
        if (g_debug) {
          g_debug->log(L"ASIOドライバのロードに失敗しました。");
          return;
        }
      }
    }

    try {
      g_device = std::make_unique<device>();
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
      asioDrivers->removeCurrentDriver();
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API StartLoopback()
  {
    if (!g_device) {
      if (g_debug) {
        g_debug->log(L"ASIOが初期化されていません。");
      }
      return;
    }
    try {
      g_device->start();
    } catch (const runtime_error& e) {
      ASIODisposeBuffers();
      ASIOExit();
      asioDrivers->removeCurrentDriver();
      if (g_debug) {
        g_debug->log(e.get_message());
      }
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API StopLoopback()
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      g_device->stop();
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
    }
  }

  long UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetInputLatency()
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      return g_device->input_latency.load();
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
      return -1;
    }
  }

  long UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetOutputLatency()
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      return g_device->output_latency.load();
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
      return -1;
    }
  }

  double UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetSampleRate()
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      return g_device->sample_rate;
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
      return 0;
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetInputChannelNames(char** names)
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      for (auto i = 0; i < g_device->input_channels; ++i) {
        auto channel_name = g_device->channel_info[i].name;
        std::copy(channel_name, channel_name + 32, names[i]);
      }
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetOutputChannelNames(char** names)
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      for (auto i = g_device->input_channels; i < g_device->input_channels + g_device->output_channels; ++i) {
        auto channel_name = g_device->channel_info[i].name;
        std::copy(channel_name, channel_name + 32, names[i - g_device->input_channels]);
      }
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
    }
  }

  long UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetNumInputChannels()
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      return g_device->input_channels;
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
      return 0;
    }
  }

  long UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetNumOutputChannels()
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      return g_device->output_channels;
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
      return 0;
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetInputSendLevel(int input_channel, int output_channel, double level)
  {
    try {
      if (!g_device) {
        throw runtime_error(L"ASIOが初期化されていません。");
      }
      g_device->input_send_matrix[input_channel][output_channel] = level;
    } catch (const runtime_error& e) {
      if (g_debug) {
        g_debug->log(e.get_message());
      }
    }
  }
}
