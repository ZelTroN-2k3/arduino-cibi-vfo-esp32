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

#include "config.h"
#include "utils.h"
#include "vfo.h"

#include <EEPROM.h>

const static uint32_t EEPROM_HEADER         PROGMEM =  0xDEADBEEF;
const static uint32_t DEFAULT_CIBI_FREQ     PROGMEM =  27555000;
const static uint32_t DEFAULT_CIBI_MIN_FREQ PROGMEM =  25000000;
const static uint32_t DEFAULT_CIBI_MAX_FREQ PROGMEM =  30000000;
const static uint32_t DEFAULT_FREQ_STEP_INC PROGMEM =  5000;
/* On superstar 3900 and derivated:
  - reference oscillator is at 10240KHz
  - FI is either 10695KHz(CW,AM,FM), 10692.5KHz(USB), 10697.5KHz(LSB) */
const static uint32_t DEFAULT_FI_FREQ_AM PROGMEM = 10695000;
const static uint32_t DEFAULT_FI_FREQ_USB PROGMEM = 10692500;
const static uint32_t DEFAULT_FI_FREQ_LSB PROGMEM = 10697500;
const static  int32_t DEFAULT_VFO_ADJ PROGMEM = 0; 
const static  int32_t DEFAULT_CLARIFIER_ADJ PROGMEM = 0; 
const static  int32_t DEFAULT_SMETER_ADJ PROGMEM = 0; 
Config::Config(Input &_input, U8G2 &_display, DDS &_dds):
context_({ 
  .eeprom_header = EEPROM_HEADER,
  .cibi_frequency = DEFAULT_CIBI_FREQ,
  .cibi_min_freq = DEFAULT_CIBI_MIN_FREQ,
  .cibi_max_freq = DEFAULT_CIBI_MAX_FREQ,
  .freq_step_inc = DEFAULT_FREQ_STEP_INC,
  .fi_am = DEFAULT_FI_FREQ_AM,
  .fi_usb = DEFAULT_FI_FREQ_USB,
  .fi_lsb = DEFAULT_FI_FREQ_LSB,
  .vfo_adj = DEFAULT_VFO_ADJ,
  .clarifier_adj = DEFAULT_CLARIFIER_ADJ,
  .clarifier_center = 2048,
  .smeter_adj = DEFAULT_SMETER_ADJ
}),
input_(_input),
dds_(_dds),
current_cursor_pos_(CURSOR_POS_NONE),
current_menu_index_(CONFIG_CIBI_FREQ),
current_value_(&context_.cibi_frequency)
{
  view_ = new ConfigDisplay(_display);
}

Config::~Config()
{
  delete view_;
}

void Config::loop(bool _update_display)
{
  bool update = _update_display;
  uint32_t freq_step = 0;
  int increment = 0;
  if(input_.buttonClicked())
  {
    update = true;
    if (current_menu_index_ != CONFIG_ABOUT) {
      current_cursor_pos_++;
      if(current_cursor_pos_ >= CURSOR_POS_MAX)
      {
        current_cursor_pos_ = CURSOR_POS_NONE;
      }
    } else {
      current_cursor_pos_ = CURSOR_POS_NONE;
    }
  }
  freq_step = quick_pow10(current_cursor_pos_-1);
  increment = (int)input_.getEncoderIncrement();
  if(increment) 
  {
    update = true;
    if(CURSOR_POS_NONE == current_cursor_pos_)
    {
      /* change between menu items */
      current_menu_index_ += increment;

      if (current_menu_index_ >= CONFIG_OLED_MAX)
      {
        current_menu_index_ = 0;
      }
      else if (current_menu_index_ < 0)
      {
        current_menu_index_ = CONFIG_OLED_MAX - 1;
      }
    }

    /* always update current_value_ based on current_menu_index_ */
    switch (current_menu_index_)
    {
      case CONFIG_CIBI_FREQ:
        current_value_ = &context_.cibi_frequency;
        break;
      case CONFIG_CIBI_MIN_FREQ:
        current_value_ = &context_.cibi_min_freq;
        break;
      case CONFIG_CIBI_MAX_FREQ:
        current_value_ = &context_.cibi_max_freq;
        break;
      case CONFIG_FREQ_STEP:
        current_value_ = &context_.freq_step_inc;
        break;
      case CONFIG_FI_AM:
        current_value_ = &context_.fi_am;          /* FI AM/FM/CW */
        break;
      case CONFIG_FI_USB:
        current_value_ = &context_.fi_usb;         /* FI USB */
        break;
      case CONFIG_FI_LSB:
        current_value_ = &context_.fi_lsb;         /* FI LSB */
        break;
      case CONFIG_VFO_ADJ:     
        current_value_ = (uint32_t*)&context_.vfo_adj;
        break;
      case CONFIG_CLARIFIER_ADJ:     
        current_value_ = (uint32_t*)&context_.clarifier_adj;
        break;
      case CONFIG_CLARIFIER_CENTER:     
        current_value_ = &context_.clarifier_center;
        break;
      case CONFIG_SMETER_ADJ:
        current_value_ = (uint32_t*)&context_.smeter_adj;
        break;
      case CONFIG_ABOUT:
        current_value_ = nullptr;
        break;
      default:
        return;
    }

    if(CURSOR_POS_NONE != current_cursor_pos_ && current_value_ != nullptr)
    {
      if((current_menu_index_ != CONFIG_VFO_ADJ) && 
         (current_menu_index_ != CONFIG_CLARIFIER_ADJ) && 
         (current_menu_index_ != CONFIG_SMETER_ADJ) && 
         (increment < 0) && 
         (freq_step * abs(increment)) > (*current_value_))
      {
          /* avoid negative values */
          (*current_value_) = 0;
      }
      else
      {
          (*current_value_) += ((uint32_t)increment * freq_step);
      }
    }
  }

  if(update)
  {
    view_->clear();

    if(CURSOR_POS_NONE == current_cursor_pos_)
    {
      view_->highlightTitle(WHITE);
      view_->setTitle(current_menu_index_, BLACK);
    }
    else
    {
      view_->setTitle(current_menu_index_, WHITE);
    }
    
    if (current_menu_index_ == CONFIG_ABOUT)
    {
      view_->setAbout(WHITE);
    }
    else if (current_menu_index_ != CONFIG_VFO_ADJ &&
        current_menu_index_ != CONFIG_CLARIFIER_ADJ &&
        current_menu_index_ != CONFIG_SMETER_ADJ)
    {
      view_->setValueUint((*current_value_), WHITE);
    }
    else
    {
      view_->setValueInt((*(int32_t*)current_value_), WHITE);
    }

    if(current_menu_index_ == CONFIG_VFO_ADJ && current_value_ != nullptr)
    {
      /* set si5351 correction here */
      dds_.setOutputCorrection((*(int32_t*)current_value_));
    }
    
    if (current_menu_index_ != CONFIG_ABOUT) {
      view_->setCursorPos(current_cursor_pos_, WHITE);
    }
    view_->display();
  }
}

int Config::open()
{
  EEPROM.get(0, context_);
  if(context_.eeprom_header != EEPROM_HEADER) 
  {
    /* EEPROM first time use */
    context_.eeprom_header = EEPROM_HEADER;
    context_.cibi_frequency = DEFAULT_CIBI_FREQ;
    context_.cibi_min_freq = DEFAULT_CIBI_MIN_FREQ;
    context_.cibi_max_freq = DEFAULT_CIBI_MAX_FREQ;
    context_.freq_step_inc = DEFAULT_FREQ_STEP_INC;
    context_.fi_am = DEFAULT_FI_FREQ_AM;
    context_.fi_usb = DEFAULT_FI_FREQ_USB;
    context_.fi_lsb = DEFAULT_FI_FREQ_LSB;
    context_.vfo_adj = DEFAULT_VFO_ADJ;
    context_.clarifier_adj = DEFAULT_CLARIFIER_ADJ;
    context_.clarifier_center = 2048; // Default for 12-bit ADC
    context_.smeter_adj = DEFAULT_SMETER_ADJ;
    context_.theme_color = 0; // Default Green
    context_.show_smeter = 1; // Default Show
    context_.show_waterfall = 1; // Default Show
    context_.layout_mode = 0; // Default Option 1
    context_.menu_ui_style = 0; // Default Modal
    context_.layout_orientation = 0; // Default Horizontal (Auto)
    for(int i=0; i<5; i++) context_.memories[i] = DEFAULT_CIBI_FREQ;
  }
  return 0;
}

uint32_t Config::getMemory(int index)
{
  if(index >= 0 && index < 5) return context_.memories[index];
  return 0;
}

void Config::setMemory(int index, uint32_t freq)
{
  if(index >= 0 && index < 5) {
    context_.memories[index] = freq;
    save();
  }
}

void Config::resetMemories() {
  context_.memories[0] = 27185000;
  context_.memories[1] = 27065000;
  context_.memories[2] = 27555000;
  context_.memories[3] = 26500000;
  context_.memories[4] = 27405000;
  save();
}

int Config::save()
{
  EEPROM.put(0, context_);
  EEPROM.commit();
  return 0;
}

const uint32_t& Config::getCibiFreq()
{
  return context_.cibi_frequency;
}

const uint32_t& Config::getCibiMinFreq()
{
  return context_.cibi_min_freq;
}

const uint32_t& Config::getCibiMaxFreq()
{
  return context_.cibi_max_freq;
}

const uint32_t& Config::getFreqStepIncrement()
{
  return  context_.freq_step_inc;
}

const uint32_t& Config::getFiAm()
{
  return context_.fi_am;
}

const uint32_t& Config::getFiUsb()
{
  return context_.fi_usb;
}

const uint32_t& Config::getFiLsb()
{
  return context_.fi_lsb;
}

const int32_t& Config::getVFOAdj()
{
  return context_.vfo_adj;
}

const int32_t& Config::getClarifierAdj()
{
  return context_.clarifier_adj;
}

const uint32_t& Config::getClarifierCenter()
{
  return context_.clarifier_center;
}

const int32_t& Config::getSMeterAdj()
{
  return context_.smeter_adj;
}

void Config::setCibiFreq(const uint32_t& _freq)
{
  if(_freq != context_.cibi_frequency)
  {
    context_.cibi_frequency = _freq;
    save();
  }
}

void Config::setCibiMinFreq(uint32_t _freq) {
  context_.cibi_min_freq = _freq;
  save();
}

void Config::setCibiMaxFreq(uint32_t _freq) {
  context_.cibi_max_freq = _freq;
  save();
}

void Config::setFreqStepIncrement(uint32_t _step) {
  context_.freq_step_inc = _step;
  save();
}

void Config::setFiAm(uint32_t _freq) {
  context_.fi_am = _freq;
  save();
}

void Config::setFiUsb(uint32_t _freq) {
  context_.fi_usb = _freq;
  save();
}

void Config::setFiLsb(uint32_t _freq) {
  context_.fi_lsb = _freq;
  save();
}

void Config::setVFOAdj(int32_t _adj) {
  context_.vfo_adj = _adj;
  dds_.setOutputCorrection(_adj);
  save();
}

void Config::setClarifierCenter(uint32_t _center) {
  context_.clarifier_center = _center;
  save();
}

void Config::setSMeterAdj(int32_t _adj) {
  context_.smeter_adj = _adj;
  save();
}

void Config::getCurrentValueFormatted(char* buffer, int len) const {
  if (buffer == nullptr || len <= 0) return;
  buffer[0] = '\0';

  if (current_menu_index_ == CONFIG_ABOUT) {
    strncpy(buffer, "ABOUT", len);
    buffer[len-1] = '\0';
    return;
  }
  
  if (current_value_ == nullptr) {
    strncpy(buffer, "---", len);
    buffer[len-1] = '\0';
    return;
  }
  
  if (current_menu_index_ == CONFIG_VFO_ADJ || 
      current_menu_index_ == CONFIG_CLARIFIER_ADJ || 
      current_menu_index_ == CONFIG_SMETER_ADJ) {
    snprintf(buffer, len, "%ld", *(int32_t*)current_value_);
  } else {
    snprintf(buffer, len, "%lu", *current_value_);
  }
}
