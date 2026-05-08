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

#include "configdisplay.h"
#include "vfo.h"
#include <Arduino.h>
#if defined(__AVR__)
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif

ConfigDisplay::ConfigDisplay(U8G2 &_display):
display_(_display)
{

}

ConfigDisplay::~ConfigDisplay()
{
  display_.clearDisplay();
}

void ConfigDisplay::init()
{
  display_.drawHLine(0, 31, SCREEN_WIDTH);
  display_.drawHLine(0, 40, SCREEN_WIDTH);
}

void ConfigDisplay::clear()
{
  display_.clearBuffer();
}

void ConfigDisplay::display()
{
  display_.updateDisplay();
}

void ConfigDisplay::clearDisplay()
{
  display_.clearDisplay();
}

void ConfigDisplay::setTitle(int _title_index, uint16_t _color)
{
    const static int16_t TITLE_POS_X PROGMEM = 6;
    const static int16_t TITLE_POS_Y PROGMEM = 27;
    const static int16_t INDEX_POS_X PROGMEM = 30;
    const static int16_t INDEX_POS_Y PROGMEM = 11;
    const static int16_t MAX_STR_LENGTH PROGMEM = 20;
    const static char title_cibi_default_freq[] PROGMEM = "CIBI DEFAULT FREQ.";
    const static char title_cibi_min_freq[] PROGMEM     = " CIBI FREQ. MIN.";
    const static char title_cibi_max_freq[] PROGMEM     = " CIBI FREQ. MAX.";
    const static char title_freq_step_inc[] PROGMEM     = "FREQ. STEP INCREM.";
    const static char title_fi_am_fm[] PROGMEM          = "    FI AM/FM";
    const static char title_fi_usb[] PROGMEM            = "     FI USB";
    const static char title_fi_lsb[] PROGMEM            = "     FI LSB";
    const static char title_vfo_adj[] PROGMEM           = "  VFO FREQ. ADJ";
    const static char title_clarifier_adj[] PROGMEM     = "CLARIFIER FREQ. ADJ";
    const static char title_clarifier_center[] PROGMEM  = "CLARIFIER ADC CENTER";
    const static char title_smeter_adj[] PROGMEM        = "   S-METER ADJ";
    const static char* const titles[] PROGMEM = {
        title_cibi_default_freq,
        title_cibi_min_freq,
        title_cibi_max_freq,
        title_freq_step_inc,
        title_fi_am_fm,
        title_fi_usb,
        title_fi_lsb,
        title_vfo_adj,
        title_clarifier_adj,
        title_clarifier_center,
        title_smeter_adj
    };
    char str[MAX_STR_LENGTH] = {0};
    int menu_index = _title_index + 1;
    int menu_items = sizeof(titles) / sizeof(char*);
    display_.setFont(u8g2_font_6x10_tr);
    display_.setDrawColor(_color);
    snprintf(str, MAX_STR_LENGTH + 1, "CONFIG %d/%d :", menu_index, menu_items);
    display_.setCursor(INDEX_POS_X, INDEX_POS_Y);
    display_.print(str);
    display_.setCursor(TITLE_POS_X,TITLE_POS_Y);
    strcpy_P(str, (char*)pgm_read_word(&(titles[_title_index])));
    display_.print(str);
    display_.drawHLine(0, 15, SCREEN_WIDTH);
    display_.drawHLine(0, 31, SCREEN_WIDTH);
}

void ConfigDisplay::highlightTitle(uint16_t _color)
{
  display_.setDrawColor(_color);
  display_.drawBox(0, 0, SCREEN_WIDTH, 31);
}

void ConfigDisplay::setValueUint(uint32_t _value, uint16_t _color)
{
    char str[MAX_STR_LENGTH] = {0};
    snprintf(str, MAX_STR_LENGTH + 1, "%08lu", _value);
    display_.setFont(u8g2_font_crox5hb_tr);
    display_.setCursor(VALUE_POS_X,VALUE_POS_Y);
    display_.setDrawColor(_color);
    display_.print(str);
}

void ConfigDisplay::setValueInt(int32_t _value, uint16_t _color)
{
    const static uint16_t SIGN_SHIFT PROGMEM = 9;
    char str[MAX_STR_LENGTH] = {0};
    display_.setDrawColor(_color);
    if(_value < 0) {
      display_.drawHLine(VALUE_POS_X - SIGN_SHIFT, VALUE_POS_Y - 8, 5);
    }
    snprintf(str, MAX_STR_LENGTH + 1, "%08ld", abs(_value));
    display_.setFont(u8g2_font_crox5hb_tr);
    display_.setCursor(VALUE_POS_X,VALUE_POS_Y);
    display_.print(str);
}

void ConfigDisplay::setCursorPos(int _position, uint16_t _color)
{
  const static int16_t CURSOR_POS_X PROGMEM = 11;
  const static int16_t CURSOR_POS_Y PROGMEM = 58;
  const static int16_t CURSOR_BIG_WIDTH PROGMEM = 10;
  const static int16_t CURSOR_SHIFT PROGMEM = 14;
  int16_t x;
  display_.setDrawColor(_color);
  if(_position >= CURSOR_POS_FREQ_DIG1 && _position <= CURSOR_POS_FREQ_DIG7)
  {
      x = CURSOR_POS_X + (8 - _position) * CURSOR_SHIFT;
      display_.drawBox(x, CURSOR_POS_Y, CURSOR_BIG_WIDTH, 2);
  }
}
