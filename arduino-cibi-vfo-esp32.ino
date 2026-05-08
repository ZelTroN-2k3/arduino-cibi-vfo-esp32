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
 *** Main entry point
 ***/
#include "cibi.h"
#include "config.h"
#include "dds.h"
#include "display.h"
#include "input.h"
#include "vfo.h"
#include "utils.h"
#include <EEPROM.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h> // Ensure this library is installed
#ifdef WATCHDOG
#include "watchdog.h"
#endif

/* Constants: */
#define VERSION     "2.2.0"
#define AUTHOR      "Patrick.A"
#define WEB_VERSION "VFO v2.9"

#include "web_interface.h"
#include "logo.h"

/* Globals: */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, OLED_RESET, SCL /* A5 */ , SDA /* A4 */); 
DDS dds;
Input input;
Config config(input, display, dds);
Cibi cibi(config, input, display, dds);
WebServer server(80);
WiFiManager wm;

void handleRoot() {
  server.send(200, "text/html", WEB_PAGE);
}

void handleStatus() {
  String json;
  json.reserve(1024); // Optimization: avoid multiple reallocations
  
  json = F("{\"freq\":");
  json += cibi.getCurrentFreq();
  json += F(",\"mode\":\"");
  
  const char* modeStr = "UNKNOWN";
  switch(cibi.getModulation()) {
    case Input::MOD_AM: modeStr = "AM"; break;
    case Input::MOD_FM: modeStr = "FM"; break;
    case Input::MOD_USB: modeStr = "USB"; break;
    case Input::MOD_LSB: modeStr = "LSB"; break;
    case Input::MOD_CW: modeStr = "CW"; break;
  }
  
  json += modeStr;
  json += F("\",\"modeIdx\":");
  json += cibi.getModulation();
  
  json += F(",\"channel\":");
  json += cibi.getChannel();
  json += F(",\"bis\":");
  json += (cibi.isBis() ? F("true") : F("false"));

  json += F(",\"band\":\"");
  json += getBandName(cibi.getCurrentFreq());
  json += F("\",\"tx\":");
  json += (cibi.isTx() ? F("true") : F("false"));
  json += F(",\"smeter\":");
  json += cibi.getSMeter();
  
  json += F(",\"conf\":{");
  json += F("\"min\":"); json += config.getCibiMinFreq();
  json += F(",\"max\":"); json += config.getCibiMaxFreq();
  json += F(",\"step\":"); json += config.getFreqStepIncrement();
  json += F(",\"fi_am\":"); json += config.getFiAm();
  json += F(",\"fi_usb\":"); json += config.getFiUsb();
  json += F(",\"fi_lsb\":"); json += config.getFiLsb();
  json += F(",\"vfo_adj\":"); json += config.getVFOAdj();
  json += F(",\"sm_adj\":"); json += config.getSMeterAdj();
  json += F(",\"cl_center\":"); json += config.getClarifierCenter();
  json += F(",\"theme\":"); json += config.getTheme();
  json += F(",\"show_sm\":"); json += config.getShowSMeter();
  json += F(",\"show_wf\":"); json += config.getShowWaterfall();
  json += F(",\"layout\":"); json += config.getLayoutMode();
  json += F("},");

  json += F("\"memories\":[");
  for(int i=0; i<5; i++) {
    json += config.getMemory(i);
    if (i < 4) json += ',';
  }
  json += F("]}");
  
  server.send(200, "application/json", json);
}

void handleSet() {
  if (server.hasArg("freq")) {
    uint32_t newFreq = server.arg("freq").toInt();
    cibi.setFrequency(newFreq);
  }
  if (server.hasArg("mode")) {
    int newMode = server.arg("mode").toInt();
    cibi.setModulation(newMode);
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
  if (server.hasArg("cl_center")) {
    uint32_t center = server.arg("cl_center").toInt();
    config.setClarifierCenter(center);
    input.setClarifierCenter(center);
  }
  if (server.hasArg("theme")) config.setTheme(server.arg("theme").toInt());
  if (server.hasArg("show_sm")) config.setShowSMeter(server.arg("show_sm").toInt());
  if (server.hasArg("show_wf")) config.setShowWaterfall(server.arg("show_wf").toInt());
  if (server.hasArg("layout")) config.setLayoutMode(server.arg("layout").toInt());
  server.send(200, "text/plain", "OK");
}

void handleResetMem() {
  config.resetMemories();
  server.send(200, "text/plain", "OK");
}

// Callback for WiFiManager when it enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  display.setDrawColor(0);
  display.drawBox(0, 0, 128, 19);
  display.setDrawColor(1);
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(2, 9, "AP: CIBI-VFO-ESP32-AP");
  display.drawStr(2, 18, "Portal: 192.168.4.1");
  display.sendBuffer();
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
  analogReadResolution(12);

  // initialize the Display I2C address (for the 128x64), force a reset
  display.setI2CAddress(I2C_OLED_ADDRESS_);
  // Set I2C to 400kHz (Fast Mode)
  display.setBusClock(400000);
  display.begin();
  display.clear();
  // custom message/bitmap begins here
  display.drawXBMP(1, 20, 126, 44, logo);
  
  display.setFont(u8g2_font_6x10_tr);
  display.setFontRefHeightExtendedText();
  display.setDrawColor(1);
  display.setFontDirection(0);
  display.drawStr(2, 12, "WiFi: Connecting...");
  display.sendBuffer();
  delay(4000);

  // WiFiManager - tries to connect to last known settings
  // If it fails, it starts an Access Point named "CIBI-VFO-ESP32-AP"
  wm.setAPCallback(configModeCallback);
  wm.setConfigPortalBlocking(false); // Optimization: non-blocking portal
  wm.setConnectTimeout(10); // Don't wait too long if no known network
  bool res = wm.autoConnect("CIBI-VFO-ESP32-AP");

  // On ESP32, we can force auto-reconnect
  WiFi.setAutoReconnect(true);

  // Clear top part for result
  display.setDrawColor(0);
  display.drawBox(0, 0, 128, 19);
  display.setDrawColor(1);
  if (WiFi.status() == WL_CONNECTED) {
    display.drawStr(2, 12, ("IP: " + WiFi.localIP().toString()).c_str());
  } else {
    display.drawStr(2, 12, "WiFi: Offline / AP Mode");
  }
  display.sendBuffer();
  delay(2000);

  // Display Author and Version info
  display.clear();
  display.setFont(u8g2_font_6x10_tr);
  display.drawStr(10, 10, "Author: " AUTHOR);
  display.drawStr(10, 30, "Firmware: " VERSION);
  display.drawStr(10, 42, "Remote: " WEB_VERSION);
  display.drawStr(10, 58, "System Starting...");
  display.sendBuffer();
  delay(4000);

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
  input.setClarifierCenter(config.getClarifierCenter());
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
  wdt_enable();
#endif
}

/// Arduino main loop
bool saveConfig = false;
int mode = -1;
void loop()
{
  wm.process(); // Handle WiFi events non-blocking
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
  wdt_reset();
#endif
}
