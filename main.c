#define MA_DEBUG_OUTPUT
#define MINIAUDIO_IMPLEMENTATION
#define CHANNEL_COUNT 8
#define NUM_THREADS 4
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
//#include <rnnoise.h>
#include <miniaudio.h>
//DenoiseState* st_main;
ma_device device;
ma_device_config deviceConfig;
ma_context_config contextConfig;
ma_context context;
float* tempOut;
float* tempOut_8channels;
int target_channel = 0;
void log_callback(ma_context* pContext, ma_device* pDevice, ma_uint32 logLevel, const char* message) {
  (void)pContext;
  (void)pDevice;
  (void)logLevel;
  printf("%s\n", message);
}
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
  float* pOutputF32 = (float*)pOutput;
  const float* pInputF32 = (const float*)pInput;
  const ma_uint32 channels = pDevice->playback.channels;
  const ma_uint32 input_channels = pDevice->capture.channels;
#pragma omp parallel for simd num_threads(NUM_THREADS)
  for (ma_uint32 iFrame = 0; iFrame < frameCount; iFrame++) {
    float sumLeft = 0;
    float sumRight = 0;
    for (ma_uint32 iChannel = 0; iChannel < input_channels; iChannel++) {
      float sample = pInputF32[iFrame * input_channels + iChannel];
      // Copy input data from each channel to the corresponding output channel
      tempOut_8channels[iFrame * channels + iChannel] = sample;
      // Sum input data from left and right channels (1-6)
      if (iChannel < 6) {
        if (iChannel % 2 == 0) {
          sumLeft += sample; // left-channel
        } else {
          sumRight += sample; // right-channel
        }
      }
    }
    // Add the sum of left channels to output channel 7 and sum of right channels to output channel 8
    tempOut_8channels[iFrame * channels + 6] += sumLeft;
    tempOut_8channels[iFrame * channels + 7] += sumRight;
  }
  ma_convert_pcm_frames_format(pOutput, ma_format_f32, tempOut_8channels, ma_format_f32, (frameCount), CHANNEL_COUNT, ma_dither_mode_none);
}
int main() {
  tempOut = (float*)calloc(1024, sizeof(float));
  tempOut_8channels = (float*)calloc((1024 * CHANNEL_COUNT), sizeof(float));
  //st_main = rnnoise_create(NULL);
  ma_backend backend = ma_backend_jack;
  contextConfig = ma_context_config_init();
  contextConfig.threadPriority = ma_thread_priority_realtime;
  ma_result result = ma_context_init(&backend, 1, &contextConfig, &context);
  if (result != MA_SUCCESS) {
    printf("Failed to initialize context.\n");
    printf("DEBUG: %s\n", ma_result_description(result));
    return result;
  }
  deviceConfig = ma_device_config_init(ma_device_type_duplex);
  deviceConfig.capture.format = ma_format_f32;
  deviceConfig.capture.channels = CHANNEL_COUNT;
  deviceConfig.capture.shareMode = ma_share_mode_shared;
  deviceConfig.playback.format = ma_format_f32;
  deviceConfig.playback.channels = CHANNEL_COUNT;
  deviceConfig.playback.shareMode = ma_share_mode_shared;
  deviceConfig.sampleRate = 48000;
  deviceConfig.dataCallback = data_callback;
  ma_result resultDevEnum = ma_device_init(&context, &deviceConfig, &device);
  if (resultDevEnum != MA_SUCCESS) {
    printf("FATAL: Error initializing device..\n");
    printf("DEBUG: %s\n", ma_result_description(resultDevEnum));
    return -1;
  }
  ma_result devStartRes = ma_device_start(&device);
  if (devStartRes != MA_SUCCESS) {
    printf("FATAL: Error starting engine..\n");
    return -1;
  }
  while (true);
  ma_device_uninit(&device);
  //rnnoise_destroy(st_main);
}
