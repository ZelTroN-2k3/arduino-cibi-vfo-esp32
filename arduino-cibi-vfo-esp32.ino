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

/***
 *** Main entry point
 ***/
#include "cibi.h"
#include "config.h"
#include "dds.h"
#include "display.h"
#include "input.h"
#include "vfo.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include "web_interface.h"
#ifdef WATCHDOG
/* #include "watchdog.h" */
#endif

/* WIFI CONFIGURATION */
const char* ssid = "Freebox-ZelTroN2k3"; // YOUR_SSID
const char* password = "7AC429D2DEZ2"; // YOUR_PASSWORD

/// Globals:
//U8G2_SSD1309_128X64_NONAME0_F_HW_I2C display(U8G2_R0, OLED_RESET, SCL /* A5 */ , SDA /* A4 */); // working with 2.42" only
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, OLED_RESET, SCL /* A5 */ , SDA /* A4 */); // working with 2.42" and 0.96" displays
DDS dds;
Input input;
Config config(input, display, dds);
Cibi cibi(config, input, display, dds);
WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", WEB_PAGE);
}

void handleStatus() {
  String json = "{";
  json += "\"freq\":" + String(cibi.getCurrentFreq()) + ",";
  
  String modeStr = "UNKNOWN";
  switch(cibi.getModulation()) {
    case Input::MOD_AM: modeStr = "AM"; break;
    case Input::MOD_FM: modeStr = "FM"; break;
    case Input::MOD_USB: modeStr = "USB"; break;
    case Input::MOD_LSB: modeStr = "LSB"; break;
    case Input::MOD_CW: modeStr = "CW"; break;
  }
  
  json += "\"mode\":\"" + modeStr + "\",";
  
  // Get channel info from Cibi instance
  int channel = cibi.getChannel();
  bool bis = cibi.isBis();
  
  json += "\"channel\":" + String(channel) + ",";
  json += "\"bis\":" + String(bis ? "true" : "false") + ",";

  // Get band name based on current frequency logic from cibidisplay.cpp
  String bandStr = "OOB";
  uint32_t f = cibi.getCurrentFreq();
  if(135700 <= f && f <= 137800) bandStr = "2222";
  else if(472000 <= f && f <= 479000) bandStr = "630m";
  else if(1800000 <= f && f <= 2000000) bandStr = "160m";
  else if(3500000 <= f && f <= 4000000) bandStr = "80m";
  else if(5351500 <= f && f <= 5366500) bandStr = "60m";
  else if(7000000 <= f && f <= 7300000) bandStr = "40m";
  else if(10100000 <= f && f <= 10150000) bandStr = "30m";
  else if(14000000 <= f && f <= 14350000) bandStr = "20m";
  else if(18068000 <= f && f <= 18168000) bandStr = "17m";
  else if(21000000 <= f && f <= 21450000) bandStr = "15m";
  else if(24890000 <= f && f <= 24990000) bandStr = "12m";
  else if(26000000 <= f && f <= 28000000) bandStr = "11m";
  else if(28000000 <= f && f <= 29700000) bandStr = "10m";

  json += "\"band\":\"" + bandStr + "\",";
  json += "\"tx\":" + String(cibi.isTx() ? "true" : "false") + ",";
  json += "\"smeter\":" + String(cibi.getSMeter()) + ",";
  
  // Config parameters
  json += "\"conf\":{";
  json += "\"min\":" + String(config.getCibiMinFreq()) + ",";
  json += "\"max\":" + String(config.getCibiMaxFreq()) + ",";
  json += "\"step\":" + String(config.getFreqStepIncrement()) + ",";
  json += "\"fi_am\":" + String(config.getFiAm()) + ",";
  json += "\"fi_usb\":" + String(config.getFiUsb()) + ",";
  json += "\"fi_lsb\":" + String(config.getFiLsb()) + ",";
  json += "\"vfo_adj\":" + String(config.getVFOAdj()) + ",";
  json += "\"sm_adj\":" + String(config.getSMeterAdj()) + ",";
  json += "\"theme\":" + String(config.getTheme());
  json += "},";

  json += "\"memories\":[";
  for(int i=0; i<5; i++) {
    json += String(config.getMemory(i)) + (i < 4 ? "," : "");
  }
  json += "]";
  json += "}";
  server.send(200, "application/json", json);
}

void handleSet() {
  if (server.hasArg("freq")) {
    uint32_t newFreq = server.arg("freq").toInt();
    cibi.setFrequency(newFreq);
  }
  server.send(200, "text/plain", "OK");
}

void handleSetMem() {
  if (server.hasArg("index") && server.hasArg("freq")) {
    int index = server.arg("index").toInt();
    uint32_t freq = server.arg("freq").toInt();
    config.setMemory(index, freq);
  }
  server.send(200, "text/plain", "OK");
}

void handleUpdateConfig() {
  if (server.hasArg("min")) config.setCibiMinFreq(server.arg("min").toInt());
  if (server.hasArg("max")) config.setCibiMaxFreq(server.arg("max").toInt());
  if (server.hasArg("step")) config.setFreqStepIncrement(server.arg("step").toInt());
  if (server.hasArg("fi_am")) config.setFiAm(server.arg("fi_am").toInt());
  if (server.hasArg("fi_usb")) config.setFiUsb(server.arg("fi_usb").toInt());
  if (server.hasArg("fi_lsb")) config.setFiLsb(server.arg("fi_lsb").toInt());
  if (server.hasArg("vfo_adj")) config.setVFOAdj(server.arg("vfo_adj").toInt());
  if (server.hasArg("sm_adj")) config.setSMeterAdj(server.arg("sm_adj").toInt());
  if (server.hasArg("theme")) config.setTheme(server.arg("theme").toInt());
  server.send(200, "text/plain", "OK");
}

void handleResetMem() {
  config.resetMemories();
  server.send(200, "text/plain", "OK");
}

#ifdef WATCHDOG
/*
wdt_interrupt()
{
#ifdef DEBUG
  Serial.println(F("Arduino Cibi VFO - wdt_interrupt"));
#endif
  config.setCibiFreq(cibi.getCurrentFreq());
}
*/
#endif

/// Arduino setup
void setup()
{
  // 0X3C multiplied by 2 for U8G2
  const static byte I2C_OLED_ADDRESS_ PROGMEM = 0x78;
#ifdef DEBUG
    Serial.begin(115200);
    Serial.println(F("Arduino Cibi VFO - setup - begin"));
#endif
  
  EEPROM.begin(512);
  analogReadResolution(10);

  // initialize the Display I2C address (for the 128x64), force a reset
  display.setI2CAddress(I2C_OLED_ADDRESS_);
  // slow down I2C to 100kHz
  display.setBusClock(100000);
  display.begin();
  display.clear();
  // custom message/bitmap begins here
  display.setFont(u8g2_font_6x10_tr);
  display.setFontRefHeightExtendedText();
  display.setDrawColor(1);
  display.setFontDirection(0);
  display.drawStr(35, 10, "Starting...");
  display.drawStr(12, 30, "Connecting WiFi...");
  display.sendBuffer();

  // WIFI Setup
  WiFi.begin(ssid, password);
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    retry++;
  }

  display.clear();
  if (WiFi.status() == WL_CONNECTED) {
    display.drawStr(12, 10, "WiFi: Connected");
    display.drawStr(12, 30, WiFi.localIP().toString().c_str());
  } else {
    display.drawStr(12, 10, "WiFi: Failed");
  }
  display.sendBuffer();
  delay(2000);

  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/set", handleSet);
  server.on("/setmem", handleSetMem);
  server.on("/updateconfig", handleUpdateConfig);
  server.on("/resetmem", handleResetMem);
  server.begin();

  // OTA Configuration
  ArduinoOTA.setHostname("CIBI-VFO-ESP32");
  ArduinoOTA.setPassword("cibi123"); // Optional: adds security

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) type = "sketch";
    else type = "filesystem";
    display.clear();
    display.drawStr(10, 30, "OTA Updating...");
    display.sendBuffer();
  });
  ArduinoOTA.onEnd([]() {
    display.clear();
    display.drawStr(10, 30, "Update Finished!");
    display.sendBuffer();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    display.drawFrame(10, 40, 108, 10);
    display.drawBox(12, 42, (progress / (total / 100)), 6);
    display.sendBuffer();
  });
  ArduinoOTA.onError([](ota_error_t error) {
    display.clear();
    display.drawStr(10, 20, "OTA Error!");
    display.sendBuffer();
  });
  ArduinoOTA.begin();

  input.init();
  /* load config from EEPROM */
  config.open();
  /* init DDS crystal */
  dds.init();
  if(0 != config.getVFOAdj())
  {
#ifdef DEBUG
 //   Serial.print(F("Set correction: "));Serial.println(config.getVFOAdj());
#endif
    dds.setOutputCorrection(config.getVFOAdj());
  }
#ifdef DEBUG
    Serial.println(F("Setup - end"));
#endif
#ifdef WATCHDOG
  /* wdt_enable(); */
#endif
}

/// Arduino main loop
bool saveConfig = false;
int mode = -1;
void loop()
{
  server.handleClient();
  int new_mode;
  bool update_display = false;
  /* read inputs */
  input.read();
  new_mode = input.getApplicationMode();
  if(mode != new_mode)
  {
    display.clearDisplay();
    mode = new_mode;
    update_display = true;
  }
  switch(mode)
  {
    case Input::CONFIG_MODE:
      config.loop(update_display);
      /* Save config upon exit */
      saveConfig = true;
      break;
#ifdef TRANSVERTER
    case Input::TRANSVERTER_MODE:
#endif
      /* TODO: transverter.loop(update_display) */
    case Input::TRANSCEIVER_MODE:
    default:
      if(saveConfig)
      {
        config.save();
        saveConfig = false;
      }
      cibi.loop(update_display);
      break;
  }
#ifdef WATCHDOG
  /* wdt_reset(); */
#endif
}
