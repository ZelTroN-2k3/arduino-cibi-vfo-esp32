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
#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>
#define ENC_DECODER (1 << 2)
#include "ClickEncoder.h"

class Input 
{
  public:
    Input();
    /* init inputs, called during setup */
    void init();
    /* read inputs, call within loop, as much as you can */
    void read();

    /* Enumerations */
    /* modulation selector */
    enum {
        MOD_CW  = 0x0,
        MOD_AM  = 0x1,
        MOD_FM  = 0x2,
        MOD_USB = 0x3,
        MOD_LSB = 0x4
    };

    /* Application has the 3 following modes
    * - configuration mode to setup offset, default frequencies...
    * - transceiver mode to RX/TX on the Cibi band
    * - transverter mode to RX on other bands (requires an expansion card) 
    */
    enum {
      CONFIG_MODE      = 0x0,
      TRANSCEIVER_MODE = 0x1,
      TRANSVERTER_MODE = 0x2
    };

    /* increment can be :
       - manual, selecting a digit and changing it by ratating the encoder.
       - a step, according to configuration
       */
    enum {
      INCREMENT_MANUAL = 0x0,
      INCREMENT_STEP   = 0x1
    };

    /* getters */
    const int getModulation();
    const int getApplicationMode();
    const int getIncrementMode();
    const int16_t getEncoderIncrement();
    bool buttonClicked();
    bool buttonLongClicked();
    bool getTx();
    const int getSMeter();
    const int getClarifier();
    /* setters */
    void setBuiltInLED(uint8_t value) const;

  private:
    static void timerIsr();
    /* store the current modulation */
    uint8_t modulation_ : 3;
    /* store the current application mode */
    uint8_t application_mode_ : 2;
    /* store the current increment mode */
    uint8_t increment_mode_ : 1;
    /* store the RX/TX mode */
    uint8_t tx_ : 1; /* 1: transmitting, 0: receiving */
    /* store clarifier input */
    int clarifier_;
    /* store SMeter input */
    int smeter_;
    /* smeter sample period */
    int smeter_sampling_;
    /* button state */
    ClickEncoder::Button button_;
    /* encoder increment */
    int16_t increment_;
    /* rotary encoder with button */
    static ClickEncoder encoder_;

};

#endif /* INPUT_H */
