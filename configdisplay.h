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
#ifndef CONFIGDISPLAY_H
#define CONFIGDISPLAY_H

#include "display.h"

class ConfigDisplay
{
  public:
    ConfigDisplay(U8G2&_display);
    /* De-initiliaze and release display */
    ~ConfigDisplay();
    /* Display initialisation */
    void init();
    /* clean screen */
    void clear();
    /* refresh display */
    void display();
    /* erase display */
    void clearDisplay();
    /* Update displayed config menu title */
    void setTitle(int _title_index, uint16_t _color);
    /* Highlight title */
    void highlightTitle(uint16_t _color);
    /* Update displayed config menu value */
    void setValueUint(uint32_t _value, uint16_t _color);
    void setValueInt(int32_t _value, uint16_t _color);
    /* Update displayed cursor position */
    void setCursorPos(int _position, uint16_t _color);
  private:
    // OLED module
    U8G2 &display_;
    // Some common constants
    const static int16_t VALUE_POS_X PROGMEM = 10;
    const static int16_t VALUE_POS_Y PROGMEM = 55;
    const static int16_t MAX_STR_LENGTH PROGMEM = 20;
};



#endif /* CONFIGDISPLAY_H */
