#pragma once
#include <asiosys.h>
#include <asio.h>
#include <array>
#include <atomic>

namespace asio_mic {

class device
{
public:
  device();
  ~device();

  void start();
  void stop() noexcept;

  long input_channels;
  long output_channels;
  long buffer_min_size;
  long buffer_max_size;
  long buffer_preferred_size;
  long buffer_granularity;
  ASIOSampleRate sample_rate;
  bool need_output_ready_call;
  std::atomic<long> input_latency;
  std::atomic<long> output_latency;
  std::array<ASIOBufferInfo, 64> buffer_info;
  std::array<ASIOChannelInfo, 64> channel_info;
  std::array<std::array<double, 32>, 32> input_send_matrix;
};

}
