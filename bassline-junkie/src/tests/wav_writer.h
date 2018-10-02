// Copyright 2015 Olivier Gillet.
//
// Author: Olivier Gillet (ol.gillet@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// WAV-file writer.

#ifndef STMLIB_TEST_WAV_WRITER_H_
#define STMLIB_TEST_WAV_WRITER_H_

#include <cstdio>
#include <cstdlib>
#include <iostream>

template<typename T>
void send_char(T *data, size_t size, FILE* fp_) {
    fwrite(data, size, 1, fp_);
}

template<typename T>
void send(T *data, FILE* fp_) {
    fwrite(data, sizeof(*data), 1, fp_);
}

template<typename T>
void send(T data, FILE* fp_) {
    fwrite(&data, sizeof(data), 1, fp_);
}

namespace stmlib {

class WavWriter {
 public:
  WavWriter(size_t num_channels,
      size_t sample_rate,
      size_t duration) : num_channels_(num_channels), sample_rate_(sample_rate), duration_(duration), fp_(NULL) { }

  ~WavWriter() {
    if (fp_) {
      fclose(fp_);
    }
  }
  
  bool Open(const char* file_name) {
    fp_ = fopen(file_name, "wb");
    if (!fp_) {
      return false;
    }
    
    uint32_t l;
    uint16_t s;
  
    fwrite("RIFF", 4, 1, fp_);
    l = 36 + duration_ * sample_rate_ * 2 * num_channels_;
    fwrite(&l, 4, 1, fp_);
    fwrite("WAVE", 4, 1, fp_);
  
    fwrite("fmt ", 4, 1, fp_);
    l = 16;
    fwrite(&l, 4, 1, fp_);
    s = 1;
    fwrite(&s, 2, 1, fp_);
    s = num_channels_;
    fwrite(&s, 2, 1, fp_);
    l = sample_rate_;
    fwrite(&l, 4, 1, fp_);
    l = static_cast<uint32_t>(sample_rate_) * 2 * num_channels_;
    fwrite(&l, 4, 1, fp_);
    s = 2 * num_channels_;
    fwrite(&s, 2, 1, fp_);
    s = 16;
    fwrite(&s, 2, 1, fp_);
  
    fwrite("data", 4, 1, fp_);
    l = duration_ * sample_rate_ * 2 * num_channels_;
    fwrite(&l, 4, 1, fp_);
    return true;
  }

  bool Open2(const char* file_name) {
    fp_ = fopen(file_name, "wb");
    if (!fp_) {
      return false;
    }
    
    uint16_t bits_per_sample = 32;
    uint16_t bytes_per_sample = bits_per_sample / 8;

    // RIFF Header
    char riff_header[4] = {'R', 'I', 'F', 'F'};                             // Contains "RIFF"
    uint32_t wav_size = 36 + duration_ * sample_rate_ * bytes_per_sample * num_channels_;  // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
    char wave_header[4] = {'W', 'A', 'V', 'E'};                             // Contains "WAVE"

    // Format Header
    char fmt_header[4] = {'f', 'm', 't', ' '};                              // Contains "fmt " (includes trailing space)
    uint32_t fmt_chunk_size = 16;                                           // Should be 16 for PCM
    uint16_t audio_format = 1;                                              // Should be 1 for PCM. 3 for IEEE Float
    uint16_t num_channels = num_channels_;
    uint32_t sample_rate = sample_rate_;
    uint32_t byte_rate = sample_rate * bytes_per_sample * num_channels;     // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
    uint16_t sample_alignment = bytes_per_sample * num_channels_;           // num_channels * Bytes Per Sample
    uint16_t bit_depth = bits_per_sample;                                   // Number of bits per sample

    // Data
    char data_header[4] = {'d', 'a', 't', 'a'};                             // Contains "data " (includes trailing space)
    uint32_t data_bytes  = duration_ * sample_rate_ * bytes_per_sample * num_channels_;    // Number of bytes in data. Number of samples * num_channels * sample byte size


    send_char(riff_header, 4, fp_);     // 4
    send(wav_size, fp_);        // 4
    send_char(wave_header, 4, fp_);     // 4

    send_char(fmt_header, 4, fp_);      // 4
    send(fmt_chunk_size, fp_);  // 4

    send(audio_format, fp_);    // 2
    send(num_channels, fp_);    // 2
    send(sample_rate, fp_);     // 4
    send(byte_rate, fp_);       // 4
    send(sample_alignment, fp_);// 2
    send(bit_depth, fp_);       // 2

    send_char(data_header, 4, fp_);     // 4
    send(data_bytes, fp_);      // 4
    return true;
  }
  
  void Write(double* out, size_t size) {
    int32_t* short_buffer = (int32_t*)(calloc(size, sizeof(int32_t)));
    for (size_t i = 0; i < size; ++i) {
      double x = out[i];
      if (x >= 1.0f) x = 1.0f;
      if (x <= -1.0f) x = -1.0f;
      short_buffer[i] = static_cast<int32_t>(x * 2147483647.);
    }
    fwrite(short_buffer, sizeof(int32_t), size, fp_);
  }

 private:
  size_t num_channels_;
  size_t sample_rate_;
  size_t duration_;
  FILE* fp_;
};

}  // namespace stmlib

#endif  // STMLIB_TEST_WAV_WRITER_H_
