#include "device.h"
#include "runtime_error.h"
#include <functional>
#include <array>
#include "globals.h"

namespace asio_mic {

template <class T>
void copy_as_type(void* input_head, void* output_head, long buffer_size)
{
  std::copy(reinterpret_cast<T*>(input_head),
            reinterpret_cast<T*>(input_head) + g_device->buffer_min_size,
            reinterpret_cast<T*>(output_head));
}

void copy_as_24bit(void* input_head, void* output_head, long buffer_size)
{
  std::copy(reinterpret_cast<uint8_t*>(input_head),
            reinterpret_cast<uint8_t*>(input_head) + g_device->buffer_min_size * 3,
            reinterpret_cast<uint8_t*>(output_head));
}

void buffer_switch(long double_buffer_index, ASIOBool direct_process)
{
  for (auto input_channel = 0; input_channel < g_device->input_channels; ++input_channel) {
    for (auto output_channel = 0; output_channel < g_device->output_channels; ++output_channel) {
      auto send_level = g_device->input_send_matrix[input_channel][output_channel];
      if (send_level <= 0.0) {
        continue;
      }
      if (g_device->channel_info[input_channel].type
          != g_device->channel_info[g_device->input_channels + output_channel].type) {
        // type conversion is not currently supported.
        return;
      }

      auto input_head = g_device->buffer_info[input_channel].buffers[double_buffer_index];
      auto output_head = g_device->buffer_info[g_device->input_channels + output_channel].buffers[double_buffer_index];

      switch (g_device->channel_info[input_channel].type) {
      case ASIOSTInt16LSB:
        copy_as_type<int16_t>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTInt24LSB:		// used for 20 bits as well
        copy_as_24bit(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTInt32LSB:
        copy_as_type<int32_t>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTFloat32LSB:		// IEEE 754 32 bit float, as found on Intel x86 architecture
        copy_as_type<float>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTFloat64LSB: 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
        copy_as_type<double>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTInt32LSB16:		// 32 bit data with 18 bit alignment
      case ASIOSTInt32LSB18:		// 32 bit data with 18 bit alignment
      case ASIOSTInt32LSB20:		// 32 bit data with 20 bit alignment
      case ASIOSTInt32LSB24:		// 32 bit data with 24 bit alignment
        copy_as_type<int32_t>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTInt16MSB:
        copy_as_type<int16_t>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTInt24MSB:		// used for 20 bits as well
        copy_as_24bit(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTInt32MSB:
        copy_as_type<int32_t>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTFloat32MSB:		// IEEE 754 32 bit float, as found on Intel x86 architecture
        copy_as_type<float>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTFloat64MSB: 		// IEEE 754 64 bit double float, as found on Intel x86 architecture
        copy_as_type<double>(input_head, output_head, g_device->buffer_min_size);
        break;
      case ASIOSTInt32MSB16:		// 32 bit data with 18 bit alignment
      case ASIOSTInt32MSB18:		// 32 bit data with 18 bit alignment
      case ASIOSTInt32MSB20:		// 32 bit data with 20 bit alignment
      case ASIOSTInt32MSB24:		// 32 bit data with 24 bit alignment
        copy_as_type<int32_t>(input_head, output_head, g_device->buffer_min_size);
        break;
      }
    }
  }

  if (g_device->need_output_ready_call) {
    ASIOOutputReady();
  }
}

void sample_rate_changed(ASIOSampleRate sRate)
{

}

long asio_message(long selector, long value, void* message, double* opt)
{
  long ret = 0;
  ASIOError result;
  switch (selector)
  {
  case kAsioSelectorSupported:
    if (value == kAsioResetRequest
        || value == kAsioEngineVersion
        || value == kAsioLatenciesChanged
        || value == kAsioSupportsTimeInfo
        || value == kAsioSupportsTimeCode)
      ret = 1L;
    break;
  case kAsioResetRequest:
    // restart
    g_device->stop();
    g_device->start();
    ret = 1L;
    break;
  case kAsioLatenciesChanged:
    long input_latency, output_latency;
    result = ASIOGetLatencies(&input_latency, &output_latency);
    if (result == ASE_OK) {
      g_device->input_latency = input_latency;
      g_device->output_latency = output_latency;
    }
    ret = 1L;
    break;
  case kAsioEngineVersion:
    ret = 2L;
    break;
  case kAsioSupportsTimeInfo:
    ret = 0;
    break;
  case kAsioSupportsTimeCode:
    ret = 0;
    break;
  }
  return ret;
}

ASIOTime* buffer_switch_time_info(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
  return nullptr;
}

device::device()
{
  ASIODriverInfo driver_info;
  auto result = ASIOInit(&driver_info);
  if (result != ASE_OK) {
    throw runtime_error(L"ASIOの初期化に失敗しました。");
  }

  result = ASIOGetChannels(&input_channels, &output_channels);
  if (result != ASE_OK) {
    throw runtime_error(L"チャンネル数の取得に失敗しました。");
  }

  result = ASIOGetBufferSize(
    &buffer_min_size,
    &buffer_max_size,
    &buffer_min_size,
    &buffer_granularity
  );
  if (result != ASE_OK) {
    throw runtime_error(L"バッファサイズの取得に失敗しました。");
  }

  result = ASIOGetSampleRate(&sample_rate);
  if (result != ASE_OK) {
    throw runtime_error(L"サンプリングレートの取得に失敗しました。");
  }

  if (sample_rate <= 0.0 || sample_rate > 96000.0) {
    sample_rate = 44100.0;
    result = ASIOSetSampleRate(sample_rate);
    if (result != ASE_OK) {
      throw runtime_error(L"既定のサンプリングレート 44100Hz へのフォールバックに失敗しました。");
    }
  }

  result = ASIOOutputReady();
  if (result == ASE_OK) {
    need_output_ready_call = true;
  } else {
    need_output_ready_call = false;
  }

  ASIOCallbacks callbacks;
  callbacks.bufferSwitch = &buffer_switch;
  callbacks.sampleRateDidChange = &sample_rate_changed;
  callbacks.asioMessage = &asio_message;
  callbacks.bufferSwitchTimeInfo = &buffer_switch_time_info;

  for (auto i = 0; i < input_channels; ++i) {
    buffer_info[i].buffers[0] = buffer_info[i].buffers[1] = nullptr;
    buffer_info[i].channelNum = i;
    buffer_info[i].isInput = ASIOTrue;
  }
  for (auto i = input_channels; i < input_channels + output_channels; ++i) {
    buffer_info[i].buffers[0] = buffer_info[i].buffers[1] = nullptr;
    buffer_info[i].channelNum = i - input_channels;
    buffer_info[i].isInput = ASIOFalse;
  }

  result = ASIOCreateBuffers(
    buffer_info.data(),
    input_channels + output_channels,
    buffer_min_size,
    &callbacks
  );
  if (result != ASE_OK) {
    throw runtime_error(L"ASIOバッファの初期化に失敗しました。");
  }

  for (auto i = 0; i < input_channels + output_channels; ++i) {
    channel_info[i].channel = buffer_info[i].channelNum;
    channel_info[i].isInput = buffer_info[i].isInput;
    result = ASIOGetChannelInfo(&channel_info[i]);
    if (result != ASE_OK) {
      throw runtime_error(L"ASIOチャンネル情報の取得に失敗しました。");
    }
  }

  long input_latency_value, output_latency_value;
  result = ASIOGetLatencies(&input_latency_value, &output_latency_value);
  if (result != ASE_OK) {
    throw runtime_error(L"レイテンシ情報の取得に失敗しました。");
  }
  input_latency = input_latency_value;
  output_latency = output_latency_value;

  for (auto& input_channel : input_send_matrix) {
    for (auto& send_level : input_channel) {
      send_level = 0.0;
    }
  }
}

device::~device()
{
  ASIOExit();
}

void device::start()
{
  auto result = ASIOStart();
  if (result != ASE_OK) {
    throw runtime_error(L"ASIOの開始に失敗しました。");
  }
}

void device::stop() noexcept
{
  ASIOStop();
}

}
