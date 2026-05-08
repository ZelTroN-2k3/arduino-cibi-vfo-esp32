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
#ifndef CIBIDISPLAY_H
#define CIBIDISPLAY_H

#include "display.h"

class CibiDisplay
{
    public:
        /* Constructor and initiliaze Cibi display */
        CibiDisplay(U8G2&_display);
        CibiDisplay(const CibiDisplay &_cibidisplay);
        /* De-initiliaze and release display */
        ~CibiDisplay();
        /* Display initialisation */
        void init();
        /* clean screen */
        void clear();
        /* refresh display */
        void display();
        /* partial refresh */
        void clearZone(int _zone);
        void displayZone(int _zone);
        /* Update displayed frequency */
        void setFreq(uint32_t _frequency, uint16_t _color);
        /* Update displayed cursor position */
        void setCursorPos(int _position, uint16_t _color);
        /* Update displayed RF band */
        void setBand(uint32_t _freq, uint16_t _color);
        /* Update displayed modulation */
        void setModulation(int _modulation, uint16_t _color);
        /* update cibi channel display */
        void setCibiChannel(int _channel, uint16_t _color);
        void setCibiChannelBis(bool _bis, uint16_t _color);
        void setCibiBand(int _band, uint16_t _color);
        void setTx(bool _tx);
#ifdef SMETER_DISPLAY
        /* Update s-meter over a 1024 scale (analog input)*/
        void setSMeter(int _signal);
#endif
    private:
        /**
         * CONNEXIONS
         * GND    GND
         * VDD    +5V
         * SCLK   pin A5
         * SDA    pin A4 
         */
    
        /* definning a structure to store RF ranges and bands */
        typedef struct {
                uint32_t low_bound; /* low boundary frequency range */
                uint32_t high_bound; /* high boundary frequency range */
                int index; /* range or band index */
        } range_t;

        const static int16_t FREQ_DECIMAL_TEXT_POS_X PROGMEM = 2;
        const static int16_t FREQ_FRACTIONNAL_TEXT_POS_X PROGMEM = 35;
        const static int16_t FREQ_TEXT_POS_Y PROGMEM = 36;
        // OLED module
        U8G2 &display_;
};

#endif /* CIBIDISPLAY_H */
