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

#include "cibi.h"
#include "utils.h"
#include "vfo.h"

const int8_t Cibi::cibi_channels[45] PROGMEM = {
    1, 2, 3, 3, 4, 5, 6, 7, 7, 8, 9,
    10, 11, 11, 12, 13, 14, 15, 15, 16, 17, 18, 19, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
    30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40};

const bool Cibi::cibi_channels_bis[45] PROGMEM = {
    /* 1- 9*/ false, false, false, true /*3*/, false, false, false, false, true /*7*/, false, false,
    /*10-19*/ false, false, true /*11*/, false, false, false, false, true /*15*/, false, false, false, false, true /*19*/,
    /*20-29*/ false, false, false, false, false, false, false, false, false, false,
    /*30-39*/ false, false, false, false, false, false, false, false, false, false,
    /* 40  */ false};

Cibi::Cibi(Config &_config, Input &_input, U8G2 &_display, DDS &_dds) : config_(_config),
                                                                                    input_(_input),
                                                                                    dds_(_dds),
                                                                                    current_cibi_channel_({.channel = -1,
                                                                                                           .band = CIBI_BAND_UNKNOWN,
                                                                                                           .bis = false}),
                                                                                    current_freq_(0),
                                                                                    freq_change_time_(0),
                                                                                    current_clarifier_(0),
                                                                                    freq_step_(0),
                                                                                    current_cursor_pos_(CURSOR_POS_FREQ_DIG5), // <-- should be a parameter?
                                                                                    current_modulation_(-1),
                                                                                    current_smeter_(0),
                                                                                    current_mem_index_(0),
                                                                                    current_tx_(false),
                                                                                    scanning_(false)
{
  view_ = new CibiDisplay(_display);
  updateFreqStep(current_cursor_pos_);
}

Cibi::Cibi(const Cibi &_cibi) : config_(_cibi.config_),
                                input_(_cibi.input_),
                                dds_(_cibi.dds_),
                                current_cibi_channel_({.channel = _cibi.current_cibi_channel_.channel,
                                                       .band = _cibi.current_cibi_channel_.band,
                                                       .bis = _cibi.current_cibi_channel_.bis}),
                                current_freq_(_cibi.current_freq_),
                                freq_change_time_(_cibi.freq_change_time_),
                                current_clarifier_(_cibi.current_clarifier_),
                                freq_step_(_cibi.freq_step_),
                                current_cursor_pos_(_cibi.current_cursor_pos_),
                                current_modulation_(_cibi.current_modulation_),
                                current_smeter_(_cibi.current_smeter_),
                                current_mem_index_(_cibi.current_mem_index_),
                                current_tx_(_cibi.current_tx_),
                                scanning_(_cibi.scanning_),
                                view_(_cibi.view_)
{
}

Cibi::~Cibi()
{
  delete view_;
}

int Cibi::getCibiChannel(uint32_t _frequency, Cibi::cibi_channel_t *_channel)
{
  /* Cibi channels */
  /* Pay attention that TX over 28Mhz is reserved for HAM use */
  const static uint16_t FREQ_CIBI_LOW_DAKHZ PROGMEM = 2561;
  const static uint16_t FREQ_CIBI_HIGH_DAKHZ PROGMEM = 2875;
  /* A cibi band is 45 channels:
       - 40 channels
       - 5 'bis' channels: 3,7,11,15,19  */
  const uint16_t CIBI_BAND_DAKHZ PROGMEM = 45;
  /* Round to kHz to compute channels
     * This avoids to get no display of channels
     * in case a fine tuning was made to clarify the voice 
     */
  uint16_t freq_dakhz = _frequency / 10000;
  _channel->channel = -1;
  _channel->band = CIBI_BAND_UNKNOWN;
  _channel->bis = false;
  /* Check if we are not out of Cibi band */
  if (freq_dakhz < FREQ_CIBI_LOW_DAKHZ || freq_dakhz > FREQ_CIBI_HIGH_DAKHZ)
    return -1;

  /* move down band to 0 for computation */
  freq_dakhz -= FREQ_CIBI_LOW_DAKHZ;

  _channel->band = freq_dakhz / CIBI_BAND_DAKHZ;
  /* reuse variable freq_dakhz as index in cibi_channel array, */
  freq_dakhz = freq_dakhz % CIBI_BAND_DAKHZ;
  _channel->channel = pgm_read_byte_near(cibi_channels + freq_dakhz);
  _channel->bis = pgm_read_byte_near(cibi_channels_bis + freq_dakhz);
  return 0;
}

void Cibi::init()
{
  if (0 == current_freq_)
  {
    /* startup: retrieve value from config and compute our context */
    current_freq_ = config_.getCibiFreq();
    current_modulation_ = input_.getModulation();
    current_tx_ = input_.getTx();
    /* update DDS according to current_freq_ and current_modulation_*/
    updateDDS(current_freq_, current_modulation_, current_tx_);
  }
  /* init display with newly created context */
  view_->init();
  view_->setFreq(current_freq_, WHITE);
  bool cibiBand = (0 == getCibiChannel(current_freq_, &current_cibi_channel_));
  if(cibiBand)
  {
    view_->setCibiChannel(current_cibi_channel_.channel, WHITE);
    view_->setCibiBand(current_cibi_channel_.band, WHITE);
    view_->setCibiChannelBis(current_cibi_channel_.bis, WHITE);
  }
  else
  {
    /* HAM band */
    view_->setBand(current_freq_, WHITE);
  }
  view_->setCursorPos(current_cursor_pos_, WHITE);
  view_->setModulation(current_modulation_, WHITE);
#ifdef SMETER_DISPLAY
  view_->setSMeter(0);
#endif
}

#if 1
void Cibi::updateDisplay(uint32_t _frequency, int _position, int _modulation, bool _tx, int _smeter)
{
  cibi_channel_t cibi_channel({.channel = -1,
                               .band = CIBI_BAND_UNKNOWN,
                               .bis = false});
  uint32_t clarified_freq = add_int32_to_uint32(current_freq_, current_clarifier_);
  if (_frequency != clarified_freq ||
      current_cibi_channel_.channel != cibi_channel.channel ||
      _position != current_cursor_pos_ ||
      _modulation != current_modulation_ ||
      _tx != current_tx_)
  {
    view_->clear();
    /* TODO: could be optimized to update only the impacted digits */
    view_->setTx(_tx);
    view_->setFreq(_frequency, (_tx? BLACK : WHITE));
    /* if withing the 10-11m band, check for Cibi channels */
    /* Outside this range, set HAM bands */
    bool cibiBand = (0 == getCibiChannel(_frequency, &cibi_channel));
    if(cibiBand)
    {
#if 0 // def DEBUG
      Serial.println(cibi_channel.channel);
#endif // DEBUG
      view_->setCibiChannel(cibi_channel.channel, WHITE);
      current_cibi_channel_.channel = cibi_channel.channel;

      view_->setCibiBand(cibi_channel.band, WHITE);
      current_cibi_channel_.band = cibi_channel.band;

      view_->setCibiChannelBis(cibi_channel.bis, WHITE);
      current_cibi_channel_.bis = cibi_channel.bis;
    }
    else
    {
      view_->setBand(_frequency, WHITE);
      current_cibi_channel_.channel = -1;
      current_cibi_channel_.band = CIBI_BAND_UNKNOWN;
    }

    view_->setCursorPos(_position, WHITE);
    view_->setModulation(_modulation, WHITE);
#ifdef SMETER_DISPLAY
    view_->setSMeter(_smeter);
#else
    view_->display();
#endif
    clarified_freq = _frequency;
    current_cibi_channel_.channel = cibi_channel.channel;
    current_cursor_pos_ = _position;
    current_modulation_ = _modulation;
  }
}
#endif
#if 0
void Cibi::updateDisplay(uint32_t _frequency, int _position, int _modulation, bool _tx, int _smeter)
{
  bool update = false;
  cibi_channel_t cibi_channel({.channel = -1,
                               .band = CIBI_BAND_UNKNOWN,
                               .bis = false});
  uint32_t clarified_freq = add_int32_to_uint32(current_freq_, current_clarifier_);
  if (_frequency != clarified_freq)
  {
    /* TODO: could be optimized to update only the impacted digits */
    view_->setFreq(clarified_freq, BLACK);
    view_->setFreq(_frequency, WHITE);
    update = true;
    /* if withing the 10-11m band, check for Cibi channels */
    /* Outside this range, set HAM bands */
    bool cibiBand = (0 == getCibiChannel(_frequency, &cibi_channel));
    if(cibiBand)
    {
#if 0 // def DEBUG
      Serial.println(cibi_channel.channel);
#endif // DEBUG
      if (current_cibi_channel_.channel != cibi_channel.channel)
      {
        if(-1 == current_cibi_channel_.channel)
        {
          view_->setBand(clarified_freq, BLACK);
        }
        else
        {
          view_->setCibiChannel(current_cibi_channel_.channel, BLACK);
        }
        view_->setCibiChannel(cibi_channel.channel, WHITE);
        current_cibi_channel_.channel = cibi_channel.channel;
      }
      if (current_cibi_channel_.band != cibi_channel.band &&
          CIBI_BAND_UNKNOWN != cibi_channel.band)
      {
        view_->setCibiBand(current_cibi_channel_.band, BLACK);
        view_->setCibiBand(cibi_channel.band, WHITE);
        current_cibi_channel_.band = cibi_channel.band;
      }
      if (current_cibi_channel_.bis != cibi_channel.bis)
      {
        view_->setCibiChannelBis(current_cibi_channel_.bis, BLACK);
        view_->setCibiChannelBis(cibi_channel.bis, WHITE);
        current_cibi_channel_.bis = cibi_channel.bis;
      }
    }
    else
    {
      /* HAM band */
      if(-1 != current_cibi_channel_.channel)
      {
        view_->setCibiChannel(current_cibi_channel_.channel, BLACK);
        view_->setCibiBand(current_cibi_channel_.band, BLACK);
        view_->setCibiChannelBis(current_cibi_channel_.bis, BLACK);
      }
      else
      {
        view_->setBand(clarified_freq, BLACK);
      }
      view_->setBand(_frequency, WHITE);
      current_cibi_channel_.channel = -1;
      current_cibi_channel_.band = CIBI_BAND_UNKNOWN;
    }
  }
  if (_position != current_cursor_pos_)
  {
    view_->setCursorPos(current_cursor_pos_, BLACK);
    view_->setCursorPos(_position, WHITE);
    update = true;
  }
  if (_modulation != current_modulation_)
  {
    view_->setModulation(current_modulation_, BLACK);
    view_->setModulation(_modulation, WHITE);
    update = true;
  }
  if (update)
  {
    view_->display();
  }
}
#endif

void Cibi::updateDDS(uint32_t _frequency, int _modulation, bool _tx)
{
  bool enable_fi = _tx;
  uint32_t vfo_freq = _frequency;
  /* order matters, modulation needs to be updated prior to frequency */
  switch (_modulation)
  {
  case MOD_AM:
  case MOD_FM:
    /* store the frequency currently used */
    dds_.setOutputFreq(DDS::OUTPUT_FI, config_.getFiAm());
    break;
  case MOD_CW:
    /* store the frequency currently used */
    dds_.setOutputFreq(DDS::OUTPUT_FI, config_.getFiAm());
    enable_fi = true;
    break;
  case MOD_USB:
    /* store the frequency currently used */
    dds_.setOutputFreq(DDS::OUTPUT_FI, config_.getFiUsb());
    enable_fi = true;
    break;
  case MOD_LSB:
    /* store the frequency currently used */
    dds_.setOutputFreq(DDS::OUTPUT_FI, config_.getFiLsb());
    enable_fi = true;
    break;
  default:
    // don't change the clock for unknown modulation type.
    break;
  }
  /* RX/TX frequency is the VF0 + the FI
    * So if we want to RX/TX on 27.555MHz in AM
    * Set VFO to 27.555MHz - 10.695MHz = 16.860MHz (infradyne)
    */
  vfo_freq -= dds_.getOutputFreq(DDS::OUTPUT_FI);
#if 0
  /* RX/TX frequency is the VF0 - the FI
    * So if we want to RX/TX on 27.555MHz in AM
    * Set VFO to 27.555MHz + 10.695MHz = 38.250MHz (supradyne)
    */
  vfo_freq += dds_.getOutputFreq(DDS::OUTPUT_FI);
#endif
  dds_.setOutputFreq(DDS::OUTPUT_VFO, vfo_freq);
  dds_.enableOutput(DDS::OUTPUT_VFO, true);
  /* enable FI oscillator only while transmitting or in CW/SSB mode */
  dds_.enableOutput(DDS::OUTPUT_FI, enable_fi);
}

void Cibi::updateFreqStep(int cursor_pos)
{
  if (cursor_pos > CURSOR_POS_MAX ||
      cursor_pos == CURSOR_POS_NONE)
  {
    freq_step_ = 0;
  }
  else
  {
    freq_step_ = quick_pow10(cursor_pos - 1);
  }
}

void Cibi::loop(bool _update_display)
{
  uint32_t new_frequency = current_freq_;
  int new_cursor_pos = current_cursor_pos_;
  int new_clarifier = current_clarifier_;
  int16_t increment = input_.getEncoderIncrement();
  bool click = input_.buttonClicked();
  int new_smeter = input_.getSMeter();
  // smeter adjustment
  if(0 != config_.getSMeterAdj())
  {
    if(config_.getSMeterAdj() > 0)
    {
      /* if positive, multiply factor */
      new_smeter = new_smeter * config_.getSMeterAdj();
    }
    else if (config_.getSMeterAdj() < 0)
    {
      /* if negative, division factor */
      new_smeter = new_smeter / abs(config_.getSMeterAdj());
    }
  }
#ifdef DEBUG
    //Serial.print(F("Sig "));Serial.println(smeter);
#endif // DEBUG
  if (_update_display)
  {
    /* initial display */
    init();
    /* waiting for next call to loop() */
    return;
  }
  if(input_.getIncrementMode() == Input::INCREMENT_MANUAL)
  {
    /* Increment mode: manual
       ======================
       Click the encoder to select a digit
       Change the selected digit value by encoder rotation
     */
    if (click)
    {
      new_cursor_pos++;
      if (new_cursor_pos > CURSOR_POS_MAX)
      {
        new_cursor_pos = CURSOR_POS_NONE;
      }
    }
    updateFreqStep(new_cursor_pos);
  }
  else
  {
    /* Increment mode: step
       ====================
       Encoder rotation change frequency value by step
       Click to scan 
     */
    new_cursor_pos = CURSOR_POS_NONE;
    freq_step_ = config_.getFreqStepIncrement();
    if(click && !scanning_) {
      scanning_ = true;
      increment = 1;
    }
    else if ((click || input_.getTx()) && scanning_)
    {
      scanning_ = false;
      increment = 0;
    }
    else if (increment && scanning_)
    {
      scanning_ = false;
    }
    
    if(scanning_) {
      /* TODO: does scan threshold needs be in the config? 
       * Playing with RF gain does allow to play around this level
       */
      if(new_smeter > 10) {
        increment = -1; // seems that measure takes one loop, so return to previous freq
        scanning_ = false;
      } else {
        increment = 1;
      }
    }
  }

  if (input_.buttonLongClicked())
  {
    int start_index = current_mem_index_;
    bool found = false;
    for (int i = 1; i <= 5; i++)
    {
      int next_idx = (start_index + i) % 5;
      uint32_t mem_freq = config_.getMemory(next_idx);
      if (mem_freq >= config_.getCibiMinFreq() && mem_freq <= config_.getCibiMaxFreq())
      {
        new_frequency = mem_freq;
        current_mem_index_ = next_idx;
        found = true;
        break;
      }
    }
    // If no valid memory found, at least cycle the index
    if (!found) current_mem_index_ = (current_mem_index_ + 1) % 5;
  }
  
  if (increment)
  {
    if (increment < 0 && (freq_step_ * abs(increment)) > new_frequency)
    {
      /* avoid negative values */
      new_frequency = 0;
    }
    else
    {
      new_frequency += ((uint32_t)increment * freq_step_);
    }

    /* Limits with rotation */
    if (new_frequency > config_.getCibiMaxFreq())
      new_frequency = config_.getCibiMinFreq();
    else if (new_frequency < config_.getCibiMinFreq())
      new_frequency = config_.getCibiMaxFreq();
  }
  
  /* save new frequency to EEPROM after 30 seconds of usage */
  if (new_frequency != current_freq_)
  {
    freq_change_time_ = millis();
    input_.setBuiltInLED(HIGH);
  }
  else if(freq_change_time_ != 0)
  {
    if(millis() - freq_change_time_ > 30000)
    {
      /* about 30 seconds on the same frequency, save it */
      config_.setCibiFreq(current_freq_);
      freq_change_time_ = 0;
      input_.setBuiltInLED(LOW);
    }
  }

  if (new_frequency != current_freq_ ||
      (input_.getClarifier() + config_.getClarifierAdj()) != current_clarifier_ ||
      input_.getModulation() != current_modulation_ ||
      new_cursor_pos != current_cursor_pos_ ||
      input_.getTx() != current_tx_
#ifdef SMETER_DISPLAY
      || new_smeter != current_smeter_
#endif
      )
  {
    /* Update display and context with newer values */
    if(!input_.getTx()) 
    {
      /* Apply clarifier here if we are receiving */
      new_clarifier = input_.getClarifier() + config_.getClarifierAdj();
    }
    else
    {
      new_clarifier = 0;
    }

    updateDisplay(add_int32_to_uint32(new_frequency, new_clarifier), new_cursor_pos, input_.getModulation(), input_.getTx(), new_smeter);
    updateDDS(add_int32_to_uint32(new_frequency, new_clarifier), input_.getModulation(), input_.getTx());
    current_freq_ = new_frequency;
    current_clarifier_ = new_clarifier;
    current_modulation_ = input_.getModulation();
    current_cursor_pos_ = new_cursor_pos;
    current_tx_ = input_.getTx();
    current_smeter_ = new_smeter;
  }
}

const uint32_t& Cibi::getCurrentFreq()
{
  return current_freq_;
}

void Cibi::setFrequency(uint32_t freq)
{
  if (freq < config_.getCibiMinFreq()) freq = config_.getCibiMinFreq();
  if (freq > config_.getCibiMaxFreq()) freq = config_.getCibiMaxFreq();
  
  if (freq != current_freq_) {
    /* Update display and DDS */
    uint32_t clarified_freq = add_int32_to_uint32(freq, current_clarifier_);
    updateDisplay(clarified_freq, current_cursor_pos_, current_modulation_, current_tx_, current_smeter_);
    updateDDS(clarified_freq, current_modulation_, current_tx_);
    
    current_freq_ = freq;
    freq_change_time_ = millis();
    input_.setBuiltInLED(HIGH);
  }
}

#if 0
int Cibi::getBand(uint32_t _frequency)
{
    /*
     * Display band names according to
     * https://en.wikipedia.org/wiki/Amateur_radio_frequency_allocations
     */
    const static uint32_t BAND_2200_LOW PROGMEM =    135700;
    const static uint32_t BAND_2200_HI  PROGMEM =    135800;
    const static uint32_t BAND_1750_LOW PROGMEM =    160000;
    const static uint32_t BAND_1750_HI  PROGMEM =    190000;
    const static uint32_t BAND_630_LOW  PROGMEM =    472000;
    const static uint32_t BAND_630_HI   PROGMEM =    479000;
    const static uint32_t BAND_160_LOW  PROGMEM =   1800000;
    const static uint32_t BAND_160_HI   PROGMEM =   2000000;
    const static uint32_t BAND_80_LOW   PROGMEM =   3500000;
    const static uint32_t BAND_80_HI    PROGMEM =   4000000;
    const static uint32_t BAND_60_LOW   PROGMEM =   5332000;
    const static uint32_t BAND_60_HI    PROGMEM =   5450000;
    const static uint32_t BAND_40_LOW   PROGMEM =   7000000;
    const static uint32_t BAND_40_HI    PROGMEM =   7300000;
    const static uint32_t BAND_30_LOW   PROGMEM =  10100000;
    const static uint32_t BAND_30_HI    PROGMEM =  10150000;
    const static uint32_t BAND_20_LOW   PROGMEM =  14000000;
    const static uint32_t BAND_20_HI    PROGMEM =  14350000;
    const static uint32_t BAND_17_LOW   PROGMEM =  18068000;
    const static uint32_t BAND_17_HI    PROGMEM =  18168000;
    const static uint32_t BAND_15_LOW   PROGMEM =  21000000;
    const static uint32_t BAND_15_HI    PROGMEM =  21450000;
    const static uint32_t BAND_12_LOW   PROGMEM =  24890000;
    const static uint32_t BAND_12_HI    PROGMEM =  24990000;
    const static uint32_t BAND_11_LOW   PROGMEM =  25000000;
    const static uint32_t BAND_11_HI    PROGMEM =  28000000;
    const static uint32_t BAND_10_LOW   PROGMEM =  28000000;
    const static uint32_t BAND_10_HI    PROGMEM =  29700000;
    const static uint32_t BAND_6_LOW    PROGMEM =  50000000;
    const static uint32_t BAND_6_HI     PROGMEM =  54000000;
    const static uint32_t BAND_2_LOW    PROGMEM = 144000000;
    const static uint32_t BAND_2_HI     PROGMEM = 148000000;
    if      (_frequency >   BAND_10_LOW && _frequency <=   BAND_10_HI) return   BAND_10;
    else if (_frequency >   BAND_11_LOW && _frequency <=   BAND_11_HI) return   BAND_11;
    else if (_frequency >   BAND_12_LOW && _frequency <=   BAND_12_HI) return   BAND_12;
    else if (_frequency >   BAND_15_LOW && _frequency <=   BAND_15_HI) return   BAND_15;
    else if (_frequency >   BAND_17_LOW && _frequency <=   BAND_17_HI) return   BAND_17;
    else if (_frequency >   BAND_20_LOW && _frequency <=   BAND_20_HI) return   BAND_20;
    else if (_frequency >   BAND_30_LOW && _frequency <=   BAND_30_HI) return   BAND_30;
    else if (_frequency > BAND_2200_LOW && _frequency <= BAND_2200_HI) return BAND_2200;
    else if (_frequency > BAND_1750_LOW && _frequency <= BAND_1750_HI) return BAND_1750;
    else if (_frequency >  BAND_630_LOW && _frequency <=  BAND_630_HI) return  BAND_630;
    else if (_frequency >  BAND_160_LOW && _frequency <=  BAND_160_HI) return  BAND_160;
    else if (_frequency >   BAND_80_LOW && _frequency <=   BAND_80_HI) return   BAND_80;
    else if (_frequency >   BAND_60_LOW && _frequency <=   BAND_60_HI) return   BAND_60;
    else if (_frequency >   BAND_40_LOW && _frequency <=   BAND_40_HI) return   BAND_40;
    else if (_frequency >    BAND_6_LOW && _frequency <=    BAND_6_HI) return    BAND_6;
    else if (_frequency >    BAND_2_LOW && _frequency <=    BAND_2_HI) return    BAND_2;
    else return BAND_UNKNOWN;
}
#endif /* 0 */
