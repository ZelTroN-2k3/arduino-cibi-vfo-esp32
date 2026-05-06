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
#ifndef VFO_H
#define VFO_H

/* Commonly used program enumerations and structures goes here */
//#define DEBUG
#define WATCHDOG
#define TRANSVERTER
/* Display s-meter or simply SMETER_REPLACEMENT_TEXT if not defined */
#define SMETER_DISPLAY
#ifndef SMETER_DISPLAY
#define SMETER_REPLACEMENT_TEXT F(" PRESIDENT FRANKLIN ")
#endif

/// Enumerations
/* Aim to describe the cursor position on the screen */
enum {
    CURSOR_POS_NONE = 0,
    CURSOR_POS_FREQ_DIG1 = 1,
    CURSOR_POS_FREQ_DIG2,
    CURSOR_POS_FREQ_DIG3,
    CURSOR_POS_FREQ_DIG4,
    CURSOR_POS_FREQ_DIG5,
    CURSOR_POS_FREQ_DIG6,
    CURSOR_POS_FREQ_DIG7,
    CURSOR_POS_MAX = CURSOR_POS_FREQ_DIG7
};

    /* On Superstart 3900 and derivated :
     * FI on CW, AM, FM is 10695kHz
     * FI on LSB is 10697.5kHz
     * FI on USB is 10692.5kHz
     */
enum {
    MOD_CW = 0,
    MOD_AM,
    MOD_FM,
    MOD_USB,
    MOD_LSB
};


#endif /* VFO_H */

