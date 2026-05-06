/*
 * Copyright (c) 2019, Vincent Hervieux vincent.hervieux@gmail.com 
 * https://gitlab.com/croutor/arduino-cibi-vfo
 * Copyright (c) 2026, Patrick Ancher zeltron2k3@gmail.com
 * https://github.com/ZelTroN-2k3/arduino-cibi-vfo-esp32
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
#ifndef CONFIG_H
#define CONFIG_H

#include "configdisplay.h"
#include "input.h"
#include "dds.h"

class Config
{
  public:
    /* Config application requires: 
     - the inputs
     - the display
     - the DDS
    */
    Config(Input& _input, U8G2& _display, DDS& _dds);
    Config(const Config& _config);
    ~Config();
    /* main application loop called when the application has the focus */
    void loop(bool _update_display = false);
    /* retrieve config from EEPROM */
    int open();
    /* save config to EEPROM */
    int save();
    /* getters */
    /* cibi frequency on power on */
    const uint32_t& getCibiFreq();
    /* cibi allowed freq min */
    const uint32_t& getCibiMinFreq();
    /* cibi allowed freq max */
    const uint32_t& getCibiMaxFreq();
    /* freq step increment */
    const uint32_t& getFreqStepIncrement();
    /* intermediate frequency mixer AM/FM/CW */
    const uint32_t& getFiAm();
    /* intermediate frequency mixer USB */
    const uint32_t& getFiUsb();
    /* intermediate frequency mixer LSB */
    const uint32_t& getFiLsb();
    /* VFO adjustment (replace trim pot) */
    const int32_t& getVFOAdj();
    /* Clarifier adjustment (replace trim pot) */
    const int32_t& getClarifierAdj();
    /* S-meter adjustment */
    const int32_t& getSMeterAdj();
    /* Memory management */
    uint32_t getMemory(int index);
    void setMemory(int index, uint32_t freq);
    void resetMemories();
    /* setters */
    /* cibi frequency on power on */
    void setCibiFreq(const uint32_t& _freq);
    void setCibiMinFreq(uint32_t _freq);
    void setCibiMaxFreq(uint32_t _freq);
    void setFreqStepIncrement(uint32_t _step);
    void setFiAm(uint32_t _freq);
    void setFiUsb(uint32_t _freq);
    void setFiLsb(uint32_t _freq);
    void setVFOAdj(int32_t _adj);
    void setSMeterAdj(int32_t _adj);
    uint32_t getTheme() { return context_.theme_color; }
    void setTheme(uint32_t _theme) { context_.theme_color = _theme; save(); }
  private:
    enum
    {
      CONFIG_CIBI_FREQ       = 0,
      CONFIG_CIBI_MIN_FREQ, // 1
      CONFIG_CIBI_MAX_FREQ, // 2
      CONFIG_FREQ_STEP,     // 3
      CONFIG_FI_AM,         // 4
      CONFIG_FI_USB,        // 5
      CONFIG_FI_LSB,        // 6
      CONFIG_VFO_ADJ,       // 7
      CONFIG_CLARIFIER_ADJ, // 8
      CONFIG_SMETER_ADJ,    // 9
      CONFIG_THEME,         // 10
      CONFIG_MAX            // 11
    };
    /* context stored in EEPROM */
    struct context_t {
      uint32_t eeprom_header;  /* EEPROM flag to detect if we've already set values */
      uint32_t cibi_frequency; /* cibi default frequency on startup */
      uint32_t cibi_min_freq;  /* cibi minimum frequency */
      uint32_t cibi_max_freq;  /* cibi maximum frequency */
      uint32_t freq_step_inc;  /* frequency step, in increment mode STEP */
      uint32_t fi_am;          /* FI AM/FM/CW */
      uint32_t fi_usb;         /* FI USB */
      uint32_t fi_lsb;         /* FI LSB */
      int32_t  vfo_adj;        /* VFO frequency adjustment */
      int32_t  clarifier_adj;  /* Clarifier frequency adjustment */
      int32_t  smeter_adj;     /* S-Meter adjustment to fill S-meter */
      uint32_t memories[5];    /* 5 memory slots */
      uint32_t theme_color;    /* 0: Green, 1: Amber, 2: Blue, 3: White, 4: Red */
    } context_;
      /* references */
    Input& input_;
    DDS& dds_;
    ConfigDisplay *view_;
    /* context */
    int current_cursor_pos_;
    int current_menu_index_;
    uint32_t* current_value_;
};



#endif /* CONFIG_H */
