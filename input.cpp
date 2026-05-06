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
#include "input.h"

#include <Ticker.h>

/* #define OPTIMIZED */

/* PINS ASSIGNEMENT */
/* Encoder pins: */
#define PIN_ENC_B       2
#define PIN_ENC_A       3
#define PIN_ENC_SW      4
/* Inputs Pins active on low level */
#define PIN_TX          5
#define PIN_MODE_CONFIG 6
#ifdef TRANSVERTER
#define PIN_MODE_TRANS  7
#endif
/* Inputs: Modulation pins (protected by a 10k resistor bridge ) */
/* 0: Active on low level */
/* 1: Active on high level */
#define INPUT_MODULATION_ACTIVE_HIGH 1
#define PIN_LSB         8
#define PIN_USB         9
#define PIN_FM         10
#define PIN_AM         11
#define PIN_CW         12
#define PIN_LED        13 /* builtin LED*/
/* Input: analog */
#define PIN_INCREMENT  A1
#define PIN_CLARIFIER  A2
#define PIN_SMETER     A3
/* IIC is using pins A4 and A5 to connect OLED's bus and si5351's bus */

ClickEncoder Input::encoder_(PIN_ENC_A, PIN_ENC_B, PIN_ENC_SW, 4, LOW);
Ticker timer;

Input::Input():
modulation_(MOD_CW)
{
}

void Input::init()
{
  /* Modulation swith pins */
#if INPUT_MODULATION_ACTIVE_HIGH == 1
  pinMode(PIN_CW, INPUT);
  pinMode(PIN_AM, INPUT);
  pinMode(PIN_FM, INPUT);
  pinMode(PIN_USB, INPUT);
  pinMode(PIN_LSB, INPUT);
#else
  pinMode(PIN_CW, INPUT_PULLUP);
  pinMode(PIN_AM, INPUT_PULLUP);
  pinMode(PIN_FM, INPUT_PULLUP);
  pinMode(PIN_USB, INPUT_PULLUP);
  pinMode(PIN_LSB, INPUT_PULLUP);
#endif
  /* Builtin LED */
  pinMode(PIN_LED, OUTPUT);
  /* TX pin aims to switch FI oscillator upon TX only */
  pinMode(PIN_TX, INPUT_PULLUP);
  /* We want to have 3 menu:
    * - configuration menu
    * - tranceiver menu
    * - receiver menu
    */
  pinMode(PIN_MODE_CONFIG, INPUT_PULLUP);
#ifdef TRANSVERTER
  pinMode(PIN_MODE_TRANS, INPUT_PULLUP);
#endif
  pinMode(PIN_INCREMENT, INPUT_PULLUP);
  /* clarifier analog input*/
  pinMode(PIN_CLARIFIER, INPUT);
  /* smeter analog input */
  pinMode(PIN_SMETER, INPUT);

  encoder_.setAccelerationEnabled(true);
  /* read button every 2ms (used to be 1ms) */
  timer.attach_ms(2, timerIsr);
}

void Input::read()
{
#ifdef OPTIMIZED
  /* read the modulation pins */
  /* This reads D register only once instead of several calls to DigitalRead */
  int val = PINB; /* D8 to D13 */
#if INPUT_MODULATION_ACTIVE_HIGH == 1
  switch (val & B00011111)
  {
  case B00000001:
    modulation_ = MOD_LSB; /*D8*/
    break;
  case B00000010:
    modulation_ = MOD_USB; /*D9*/
    break;
  case B00000100:
    modulation_ = MOD_FM;  /*D10*/
    break;
  case B00001000:
    modulation_ = MOD_AM;  /*D11*/
    break;
  case B00010000:
    modulation_ = MOD_CW;  /*D12*/
    break;
  default:
    /* DEFAULT: UNCHANGED */
    break;
  }
#else
  switch (val & B00011111)
  {
  case B00011110:
    modulation_ = MOD_LSB; /*D8*/
    break;
  case B00011101:
    modulation_ = MOD_USB; /*D9*/
    break;
  case B00011011:
    modulation_ = MOD_FM;  /*D10*/
    break;
  case B00010111:
    modulation_ = MOD_AM;  /*D11*/
    break;
  case B00001111:
    modulation_ = MOD_CW;  /*D12*/
    break;
  default:
    /* DEFAULT: UNCHANGED */
    break;
  }
#endif

  val = PIND; /* D0 to D7 */
  if (0 == (val & B01000000)) /*D6*/
  {
    application_mode_ = CONFIG_MODE;
  }
#ifdef TRANSVERTER
  else if(0 == (val & B10000000)) /*D7*/
  {
    application_mode_ = TRANSVERTER_MODE;
  }
#endif
  else
  {
    application_mode_ = TRANSCEIVER_MODE;
  }
  tx_ = (0 == (val & B00100000) ? true : false); /*D5*/
#else
  /* read the modulation pins */
#if INPUT_MODULATION_ACTIVE_HIGH == 1
  if (HIGH == digitalRead(PIN_USB))
    modulation_ = MOD_USB;
  else if (HIGH == digitalRead(PIN_AM))
    modulation_ = MOD_AM;
  else if (HIGH == digitalRead(PIN_FM))
    modulation_ = MOD_FM;
  else if (HIGH == digitalRead(PIN_LSB))
    modulation_ = MOD_LSB;
  else if (HIGH == digitalRead(PIN_CW))
    modulation_ = MOD_CW;
  /* DEFAULT: UNCHANGED */
#else
if (LOW == digitalRead(PIN_USB))
    modulation_ = MOD_USB;
  else if (LOW == digitalRead(PIN_AM))
    modulation_ = MOD_AM;
  else if (LOW == digitalRead(PIN_FM))
    modulation_ = MOD_FM;
  else if (LOW == digitalRead(PIN_LSB))
    modulation_ = MOD_LSB;
  else if (LOW == digitalRead(PIN_CW))
    modulation_ = MOD_CW;
  /* DEFAULT: UNCHANGED */
#endif

  if(LOW == digitalRead(PIN_MODE_CONFIG))
  {
    application_mode_ = CONFIG_MODE;
  }
#ifdef TRANSVERTER
  else if (LOW == digitalRead(PIN_MODE_TRANS))
  {
    application_mode_ = TRANSVERTER_MODE;
  }
#endif
  else 
  {
    application_mode_ = TRANSCEIVER_MODE;
  }

  tx_ = (LOW == digitalRead(PIN_TX) ? true : false);
#endif
  if(LOW == digitalRead(PIN_INCREMENT))
  {
    increment_mode_ = INCREMENT_STEP;
  }
  else
  {
    increment_mode_ = INCREMENT_MANUAL;
  }
  /*
   * transpose ADC value to frequency offset.
   * value are from 0 to 1023.
   * Ideally if clarifier_ = 511, clarifier offset is 0.
   * Round to 10Hz steps
   */
  clarifier_ = ((analogRead(PIN_CLARIFIER) - 512) / 10) * 10;
  smeter_ = analogRead(PIN_SMETER);
  button_ = encoder_.getButton();
  increment_ = encoder_.getValue();
}

bool Input::buttonLongClicked()
{
  return (button_ == ClickEncoder::Held);
}

void Input::timerIsr()
{
  encoder_.service();
}

/* Getters */
const int Input::getModulation()
{
  return modulation_;
}

const int Input::getApplicationMode()
{
  return application_mode_;
}

const int Input::getIncrementMode()
{
  return increment_mode_;
}

const int16_t Input::getEncoderIncrement()
{
  return increment_;
}

bool Input::buttonClicked()
{
  return (button_ == ClickEncoder::Clicked);
}

bool Input::getTx()
{
  return tx_;
}

const int Input::getSMeter()
{
  /* 
   * 300mV <->  61.44
   * 5V    <->  1024
   */

  return smeter_;
}

const int Input::getClarifier()
{
  return clarifier_;
}

void Input::setBuiltInLED(uint8_t value) const
{
  digitalWrite(PIN_LED, value);
}
