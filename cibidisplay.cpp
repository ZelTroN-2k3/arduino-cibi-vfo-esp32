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

/***
 *** Display: using an OLED SSD1306
 ***/
#include "cibidisplay.h"
#include "vfo.h"

#include <Arduino.h>
#if defined(__AVR__)
#include <avr/pgmspace.h>
#else
#include <pgmspace.h>
#endif


CibiDisplay::CibiDisplay(U8G2 &_display):
display_(_display)
{

}

CibiDisplay::CibiDisplay(const CibiDisplay &_cibidisplay):
display_(_cibidisplay.display_)
{

}

CibiDisplay::~CibiDisplay()
{
  display_.clearDisplay();
}

void CibiDisplay::init()
{
  const static int16_t SMETER_TOP_X PROGMEM = 5;
  const static int16_t SMETER_TOP_Y PROGMEM = 8;
  const static int16_t FREQ_DOT_TEXT_POS_X PROGMEM = 32;
  const static int16_t MHZ_TEXT_POS_X PROGMEM = 120;
  const static int16_t H_LINE_WIDTH PROGMEM = 118;
  const static int16_t H_LINE_POS_X PROGMEM = 0;
#ifdef SMETER_DISPLAY
  /* S meter frame */
  display_.setFont(u8g2_font_6x10_tr);
  display_.setCursor(SMETER_TOP_X,SMETER_TOP_Y);
  display_.setDrawColor(WHITE);
  display_.print(F("SIG.1.3.5.7.9..+30dB"));
#else
/* S meter frame */
  display_.setFont(u8g2_font_6x10_tr);
  display_.setCursor(SMETER_TOP_X,SMETER_TOP_Y+4);
  display_.setDrawColor(WHITE);
  display_.print(SMETER_REPLACEMENT_TEXT);
#endif
  /* Frequency dot */
  display_.drawDisc(FREQ_DOT_TEXT_POS_X, 35, 2, WHITE);
  /* Mhz sign */
  display_.setCursor(MHZ_TEXT_POS_X,24);
  display_.print(F("M"));
  display_.setCursor(MHZ_TEXT_POS_X,33);
  display_.print(F("H"));
  display_.setCursor(MHZ_TEXT_POS_X,40);
  display_.print(F("z"));
  /* 2 lines separation between informations */
  display_.drawHLine(H_LINE_POS_X, 16, H_LINE_WIDTH);
  display_.drawHLine(H_LINE_POS_X, 40, H_LINE_WIDTH);
}

void CibiDisplay::clear()
{
  display_.clearBuffer();
}

void CibiDisplay::display()
{
  init();
  display_.sendBuffer();
}

void CibiDisplay::setFreq(uint32_t _freq, uint16_t _color) 
{
    const static uint16_t MAX_FREQ_STR_LENGTH PROGMEM = 8; /* 30000000*/
    char str[MAX_FREQ_STR_LENGTH + 1] = {0};
    char big_str[4] = {0};
    snprintf(str, MAX_FREQ_STR_LENGTH + 1, "%08ld", _freq);
    strncpy (big_str, str, 2 * sizeof(char));
    display_.setFont(u8g2_font_crox5hb_tr);
    display_.setCursor(FREQ_DECIMAL_TEXT_POS_X,FREQ_TEXT_POS_Y);
    display_.setDrawColor(_color);
    display_.print(big_str);
    display_.setCursor(FREQ_FRACTIONNAL_TEXT_POS_X,FREQ_TEXT_POS_Y);
    display_.print(str + 2 * sizeof(char));
}

void CibiDisplay::setCursorPos(int _position, uint16_t _color)
{
  const static int16_t CURSOR_TEXT_POS_Y PROGMEM = 37;
  const static int16_t CURSOR_DECIMAL_TEXT_POS_X PROGMEM = 17;
  const static int16_t CURSOR_FRACTIONAL_TEXT_POS_X PROGMEM = FREQ_DECIMAL_TEXT_POS_X + 35;
  const static int16_t CURSOR_WIDTH PROGMEM = 10;
  const static int16_t CURSOR_HEIGHT PROGMEM = 2;
  const static int16_t CURSOR_SHIFT PROGMEM = 14;
  int16_t x;
  if(CURSOR_POS_NONE == _position) return;
  if(CURSOR_POS_FREQ_DIG7 == _position)
  {
    x = CURSOR_DECIMAL_TEXT_POS_X;
  }
  else
  {
    x = CURSOR_FRACTIONAL_TEXT_POS_X +
        (CURSOR_POS_FREQ_DIG6 - _position) * CURSOR_SHIFT;
  }
  display_.setDrawColor(_color);
  display_.drawBox(x, CURSOR_TEXT_POS_Y, CURSOR_WIDTH, CURSOR_HEIGHT);
}

void CibiDisplay::setModulation(int _modulation, uint16_t _color)
{
  const static int16_t MOD_TEXT_POS_X PROGMEM = 70;
  const static int16_t MOD_TEXT_POS_Y PROGMEM = 64;
  display_.setFont(u8g2_font_crox5hb_tr);
  display_.setCursor(MOD_TEXT_POS_X,MOD_TEXT_POS_Y);
  display_.setDrawColor(_color);
  switch (_modulation)
  {
  case MOD_CW:
    display_.print(F(" CW"));
    break;
  case MOD_AM:
    display_.print(F(" AM"));
    break;
  case MOD_FM:
    display_.print(F(" FM"));
    break;
  case MOD_USB:
    display_.print(F("USB"));
    break;
  case MOD_LSB:
  default:
    display_.print(F("LSB"));
    break;
  }
}

void CibiDisplay::setCibiChannel(int _channel, uint16_t _color)
{
  const static int16_t CIBI_CHANNEL_TEXT_POS_X PROGMEM = 16;
  const static int16_t CIBI_CHANNEL_TEXT_POS_Y PROGMEM = 64;
  char str[3] = {0};
  if(-1 != _channel)
  {
    display_.setFont(u8g2_font_crox5hb_tr);
    display_.setDrawColor(_color);
    if(_channel < 10)
      display_.setCursor(CIBI_CHANNEL_TEXT_POS_X + 14, CIBI_CHANNEL_TEXT_POS_Y);
    else
      display_.setCursor(CIBI_CHANNEL_TEXT_POS_X, CIBI_CHANNEL_TEXT_POS_Y);
    snprintf(str, sizeof(str), "%d", _channel);
    display_.print(str);
  }
}

void CibiDisplay::setCibiChannelBis(bool _bis, uint16_t _color)
{
  const static int16_t CIBI_CHANNEL_BIS_TEXT_POS_X PROGMEM = 45;
  const static int16_t CIBI_CHANNEL_BIS_POS_Y PROGMEM = 50;
  if(_bis)
  {
      display_.setFont(u8g2_font_6x10_tr);
      display_.setDrawColor(_color);
      display_.setCursor(CIBI_CHANNEL_BIS_TEXT_POS_X, CIBI_CHANNEL_BIS_POS_Y);
      display_.print(F("bis"));
  }
}

void CibiDisplay::setCibiBand(int _band, uint16_t _color)
{
    const static int CIBI_BAND_NORMAL PROGMEM = 3;
    if(CIBI_BAND_NORMAL == _band) return;
    int deviation = CIBI_BAND_NORMAL - _band;
    display_.setDrawColor(_color);
    for(int i = 0; i < abs(deviation); i++)
    {
        display_.drawHLine(7, 58 - i*6, 5);
    }
    if(deviation < 0)
    {
        /* + signs */
        for(int i = 0; i < abs(deviation); i++)
        {
            display_.drawVLine(9, 56 - i*6, 5);
        }
    }
}

void CibiDisplay::setBand(uint32_t _freq, uint16_t _color)
{
  const static int16_t BAND_TEXT_POS_X PROGMEM = FREQ_DECIMAL_TEXT_POS_X;
  const static int16_t BAND_TEXT_POS_Y PROGMEM = 64; 
  display_.setFont(u8g2_font_crox5hb_tr);
  display_.setCursor(BAND_TEXT_POS_X,BAND_TEXT_POS_Y);
  display_.setDrawColor(_color);
  if(135700 <=_freq && _freq <= 137800)
  {
    /* For this band do not print 'm' to earn on string's lenght */ 
    display_.print(F("2222"));
  }
  else if(472000 <=_freq && _freq <= 479000)
  {
    display_.print(F("630m"));
  }
  else if(1800000 <=_freq && _freq <= 2000000)
  {
    display_.print(F("160m"));
  }
  else if(3500000 <= _freq && _freq <= 4000000)
  {
    display_.print(F("80m"));
  }
  else if(5351500 <= _freq && _freq <= 5366500)
  {
    display_.print(F("60m"));
  }
  else if(7000000 <= _freq && _freq <= 7300000)
  {
    display_.print(F("40m"));
  }
  else if(10100000 <= _freq && _freq <= 10150000)
  {
    display_.print(F("30m"));
  }
  else if(14000000 <= _freq && _freq <= 14350000)
  {
    display_.print(F("20m"));
  }
  else if(18068000 <= _freq && _freq <= 18168000)
  {
    display_.print(F("17m"));
  }
  else if(21000000 <= _freq && _freq <= 21450000)
  {
    display_.print(F("15m"));
  }
  else if(24890000 <= _freq && _freq <= 24990000)
  {
    display_.print(F("12m"));
  }
  else if(28000000 <= _freq && _freq <= 29700000)
  {
    display_.print(F("10m"));
  }
  else
  {
    display_.print(F("OOB"));
  }
}

#ifdef SMETER_DISPLAY
void CibiDisplay::setSMeter(int _signal)
{
  // cursor
  display_.setDrawColor(BLACK);
  display_.drawBox(12, 11, 116, 4);
  display_.setDrawColor(WHITE);
  for(int index = 0; index < _signal && index < 21; index++)
  {
    display_.drawBox(12 + index * 5, 11, 4, 4);
  }
  display();
}
#endif

void CibiDisplay::setTx(bool _tx)
{
  // invert black and white of the displayed frequency in TX
  const static int16_t H_LINE_WIDTH PROGMEM = 118;
  const static int16_t H_LINE_POS_X PROGMEM = 0;
  const static int16_t FREQ_DOT_TEXT_POS_X PROGMEM = 32;
  display_.setDrawColor((_tx? WHITE : BLACK));
  display_.drawBox(H_LINE_POS_X, 17, H_LINE_WIDTH, 23);
}
