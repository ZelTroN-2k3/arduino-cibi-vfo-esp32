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
#include "utils.h"

uint32_t quick_pow10(int n)
{
    const static uint32_t pow10[7] PROGMEM = {
        1, 10, 100, 1000, 10000, 
        100000, 1000000
    };
    return (uint32_t) pgm_read_dword_near(pow10 + n); 
}

uint32_t add_int32_to_uint32(uint32_t a, int32_t b)
{
  if(b < 0) 
  {
    return a - abs(b);
  }
  else 
  {
    return a + abs(b);
  }
}

const char* getBandName(uint32_t _freq)
{
    if      (135700   <= _freq && _freq <= 137800)    return "2222";
    else if (472000   <= _freq && _freq <= 479000)    return "630m";
    else if (1800000  <= _freq && _freq <= 2000000)   return "160m";
    else if (3500000  <= _freq && _freq <= 4000000)   return "80m";
    else if (5351500  <= _freq && _freq <= 5366500)   return "60m";
    else if (7000000  <= _freq && _freq <= 7300000)   return "40m";
    else if (10100000 <= _freq && _freq <= 10150000)  return "30m";
    else if (14000000 <= _freq && _freq <= 14350000)  return "20m";
    else if (18068000 <= _freq && _freq <= 18168000)  return "17m";
    else if (21000000 <= _freq && _freq <= 21450000)  return "15m";
    else if (24890000 <= _freq && _freq <= 24990000)  return "12m";
    else if (26000000 <= _freq && _freq <= 28000000)  return "11m";
    else if (28000000 <= _freq && _freq <= 29700000)  return "10m";
    else return "OOB";
}
