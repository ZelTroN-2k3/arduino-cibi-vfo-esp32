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
#ifndef DDS_H
#define DDS_H

#include "si5351mcu.h"

class DDS
{
  public:
      /* valid output list */
      typedef enum {
              OUTPUT_FI        = 0,
              OUTPUT_UNUSED,  // 1
              OUTPUT_VFO,     // 2
              OUTPUT_MAX
      } output_t;

      /* Constructor */
      DDS();
      /* De-initiliaze and release DDS */
      ~DDS();
      /* Update generator output frequency */
      void setOutputFreq(output_t _output, uint32_t _frequency);
      /* Set output frequency correction */
      void setOutputCorrection(int32_t _correction);
      /* get current generator output frequency */
      const uint32_t& getOutputFreq(output_t _output);
      /* enable/disable FI, FI is required while transmitting */
      void enableOutput(output_t _output, bool _enable);
      /* initiliaze DDS */
      void init();
      
  private:
      /**
       * CONNEXIONS
       * GND    GND
       * VDD    +5V
       * SCLK   pin A5
       * SDA    pin A4 
       */
      /* xtal on my si5351 is 25Mhz */
      const static uint32_t XTALFREQ PROGMEM = 25000000L;
      /* delayed initialisation */
      boolean initialized_;
      /* internal storage for frequencies outputs */
      uint32_t current_freq_[OUTPUT_MAX];
      /* store output state */
      bool enabled_[OUTPUT_MAX];
      /* clock module */
      Si5351mcu clockgen_;
  

};

#endif /* DDS_H */
