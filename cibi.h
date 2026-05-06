/*
 * Copyright (c) 2019, Vincent Hervieux vincent.hervieux@gmail.com 
 * https://gitlab.com/croutor/arduino-cibi-vfo
 * Copyright (c) 2026, Patrick Ancher zeltron2k3@gmail.com
 * https://github.com/ZelTroN-2k3/arduino-cibi-vfo
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * Neither the name of the author Vincent Hervieux, nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef CIBI_H
#define CIBI_H

#include "cibidisplay.h"
#include "config.h"
#include "input.h"
#include "dds.h"

class Cibi
{
public:
  /* Cibi application requires: 
     - the config
     - the inputs
     - the display
     - the dds
  */
  Cibi(Config& _config, Input& _input, U8G2& _display, DDS& _dds);
  Cibi(const Cibi &_cibi);
  ~Cibi();
  /* main application loop called when the application has the focus */
  void loop(bool _update_display = false);
  /* getter on current frequency */
  const uint32_t& getCurrentFreq();
  /* setters/getters for web interface */
  void setFrequency(uint32_t freq);
  int getModulation() const { return current_modulation_; }
  int getSMeter() const { return current_smeter_; }
  bool isTx() const { return current_tx_; }
  int getChannel() const { return current_cibi_channel_.channel; }
  bool isBis() const { return current_cibi_channel_.bis; }
  /* @brief get band index 
   * @param: frequency
   * @return RF range index in  -1 if none matching */
  int getBand(uint32_t _frequency);

protected:
  friend class CibiDisplay;
    /* Cibi band enum */
  enum {
          CIBI_BAND_UNKNOWN = -1,
          CIBI_BAND_3_INF   = 0x0,
          CIBI_BAND_2_INF   = 0x1,
          CIBI_BAND_1_INF   = 0x2,
          CIBI_BAND_NORMAL  = 0x3,
          CIBI_BAND_1_SUP   = 0x4,
          CIBI_BAND_2_SUP   = 0x5,
          CIBI_BAND_3_SUP   = 0x6,
          CIBI_BAND_MAX = CIBI_BAND_3_SUP
  };

private:
  /* defining a structure to store names of Cibi channels */
  typedef struct {
          int8_t channel;
          int8_t band;
          bool bis;
  } cibi_channel_t;



  /* make a bis for cibi channels 3,7,11,15,19 */
  const static int8_t cibi_channels[45];
  const static bool cibi_channels_bis[45];

  /* RF ranges enum */
  enum {
          RANGE_RF_UNKNOWN = -1,
          RANGE_RF_LF = 0,
          RANGE_RF_MF,
          RANGE_RF_HF,
          RANGE_RF_VHF,
          RANGE_RF_MAX = RANGE_RF_VHF
  };

  /* HAM band enum */     
  // enum {
  //         BAND_UNKNOWN = -1,
  //         BAND_2200 = 0,
  //         BAND_1750,
  //         BAND_630,
  //         BAND_160,
  //         BAND_80,
  //         BAND_60,
  //         BAND_40,
  //         BAND_30,
  //         BAND_20,
  //         BAND_17,
  //         BAND_15,
  //         BAND_12,
  //         BAND_11,
  //         BAND_10,
  //         BAND_6,
  //         BAND_2,
  //         BAND_MAX = BAND_2
  // };

  /*
   * @brief init Cibi context 
   */
  void init();
  /* @brief get cibi channel 
   * @param: frequency
   * @return -1 if none matching, 0 if successful */
  int getCibiChannel(uint32_t _frequency, cibi_channel_t* _channel);
  /* Update info on display and in our context */
  void updateDisplay(uint32_t _frequency, int _position, int _modulation, bool _tx, int _smeter);
  /* Update DDS values according to modulation and frequency in our context */
  void updateDDS(uint32_t _frequency, int _modulation, bool _tx);
  /* Update the frequency step according to the given cursor position */
  void updateFreqStep(int cursor_pos);
  /* references */
  Config &config_;
  Input &input_;
  DDS &dds_;
  /* context */
  /* current displayed values */
  cibi_channel_t current_cibi_channel_;
  uint32_t current_freq_;
  uint64_t freq_change_time_;
  int current_clarifier_;
  uint32_t freq_step_; /* up/down frequency step */
  int current_cursor_pos_;
  int current_modulation_;
  int current_smeter_;
  int current_mem_index_;
  bool current_tx_;
  bool scanning_;
  CibiDisplay *view_;
};

#endif /* CIBI_H */
