/*
 * Copyright (c) 2019, Vincent Hervieux vincent.hervieux@gmail.com 
 * https://gitlab.com/croutor/arduino-cibi-vfo
 * Copyright (c) 2026, Patrick Ancher zeltron2k3@gmail.com
 * https://github.com/ZelTroN-2k3/arduino-cibi-vfo-esp32
 * 
 * All rights reserved.
 */

#ifndef WATCHDOG_H
#define WATCHDOG_H

#if defined(ARDUINO_ARCH_ESP32)
#include <esp_task_wdt.h>

// For ESP32, we use the Task Watchdog Timer (TWDT)
// Default timeout 5 seconds
#define WDT_TIMEOUT_SECONDS 5

inline void wdt_enable()
{
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    // New API for ESP-IDF 5.x (used in newer Arduino ESP32 cores)
    esp_task_wdt_config_t twdt_config = {
        .timeout_ms = WDT_TIMEOUT_SECONDS * 1000,
        .idle_core_mask = 0,    // Bitmask of cores
        .trigger_panic = true,
    };
    esp_task_wdt_init(&twdt_config);
#else
    // Legacy API
    esp_task_wdt_init(WDT_TIMEOUT_SECONDS, true);
#endif
    esp_task_wdt_add(NULL); // Add current thread (loop task)
}

inline void wdt_reset()
{
    esp_task_wdt_reset();
}

// Note: Pre-reset interrupts (ISR) are more complex on ESP32 
// and not directly compatible with the simple AVR wdt_interrupt().
// Frequency is already auto-saved every 30s in cibi.loop().

#elif defined(__AVR__)
#include <avr/wdt.h>
// friendly name for watchdog software interruption
#define wdt_interrupt() ISR(WDT_vect)

inline void wdt_enable()
{
  // Unlock watchdog register
  WDTCSR = (1 << WDCE) | (1 << WDE);
  // Set WDP2, WDP1 and WDP0 for 2 seconds countdown
  // Set WDIE and WDE to enable a software interrupt followed by µC reset
  WDTCSR = 0b01001111;
}
// wdt_reset() is already defined in avr/wdt.h
#endif

#endif /* WATCHDOG_H */
