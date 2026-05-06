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
 *** DDS: Direct Digital Synthesis
 *** The three si5351 clock outputs are square wave
 ***/
#include "dds.h"
#include "vfo.h"

DDS::DDS():
initialized_(false)
{
    /* initialize stored values */
    for(int i = 0; i < OUTPUT_MAX; i++)
        current_freq_[i] = 0;
}

DDS::~DDS()
{
  clockgen_.off();
}

void DDS::init()
{
  if(!initialized_)
  {
    /* This lib has no way to know if I2C communication is effective */
    clockgen_.init(XTALFREQ);
    /* Set drive current to its maximum */
    clockgen_.setPower(OUTPUT_FI, SIOUT_8mA);
    enabled_[OUTPUT_FI] = true;
    /* 2 mA : FM is weak */
    /* 4 mA : FM is distorded */
    /* 6 mA : FM is distorded */
    /* 8 mA : FM is really distorded - adjust VR5*/
    clockgen_.setPower(OUTPUT_VFO, SIOUT_8mA);
    enabled_[OUTPUT_VFO] = true;
    enabled_[OUTPUT_UNUSED] = false;
    initialized_ = true;
  }
}

const uint32_t& DDS::getOutputFreq(output_t _output)
{
  return current_freq_[_output];
}

void DDS::setOutputFreq(output_t _output, uint32_t _frequency) 
{
  clockgen_.setFreq(_output, _frequency);
  /* store the frequency currently used */
  current_freq_[_output] = _frequency;
}

void DDS::setOutputCorrection(int32_t _correction)
{
  /* set quartz correction */
  clockgen_.correction(_correction);
  clockgen_.setFreq(OUTPUT_VFO, current_freq_[OUTPUT_VFO]);
  clockgen_.setFreq(OUTPUT_FI, current_freq_[OUTPUT_FI]);
}

void DDS::enableOutput(output_t _output, bool _enable)
{
  if(_enable && !enabled_[_output])
  {
    clockgen_.enable(_output);
    enabled_[_output] = true;
  }
  else if(!_enable && enabled_[_output])
  {
    clockgen_.disable(_output);
    enabled_[_output] = false;
  }
}
