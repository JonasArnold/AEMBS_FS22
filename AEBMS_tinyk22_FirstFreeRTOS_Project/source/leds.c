/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "McuLib.h"
#include "leds_config.h"
#include "leds.h"
#include "McuLED.h"

#if LEDS_CONFIG_HAS_RED_LED
  static McuLED_Handle_t ledRed;
#endif
#if LEDS_CONFIG_HAS_GREEN_LED
  static McuLED_Handle_t ledGreen;
#endif
#if LEDS_CONFIG_HAS_BLUE_LED
  static McuLED_Handle_t ledBlue;
#endif
#if LEDS_CONFIG_HAS_ORANGE_LED
  static McuLED_Handle_t ledOrange;
#endif
#if LEDS_CONFIG_HAS_BLUE_TINY_LED
  static McuLED_Handle_t ledTiny;
#endif

void LEDS_On(LEDS_Leds_e led) {
  switch(led) {
  #if LEDS_CONFIG_HAS_RED_LED
    case LEDS_RED:        McuLED_On(ledRed); break;
  #endif
  #if LEDS_CONFIG_HAS_GREEN_LED
    case LEDS_GREEN:      McuLED_On(ledGreen); break;
  #endif
  #if LEDS_CONFIG_HAS_BLUE_LED
    case LEDS_BLUE:       McuLED_On(ledBlue); break;
  #endif
  #if LEDS_CONFIG_HAS_ORANGE_LED
    case LEDS_ORANGE:     McuLED_On(ledOrange); break;
  #endif
  #if LEDS_CONFIG_HAS_BLUE_TINY_LED
    case LEDS_TINY_BLUE:  McuLED_On(ledTiny); break;
  #endif
    default:
      break; /* error */
  }
}

void LEDS_Off(LEDS_Leds_e led) {
  switch(led) {
  #if LEDS_CONFIG_HAS_RED_LED
    case LEDS_RED:        McuLED_Off(ledRed); break;
  #endif
  #if LEDS_CONFIG_HAS_GREEN_LED
    case LEDS_GREEN:      McuLED_Off(ledGreen); break;
  #endif
  #if LEDS_CONFIG_HAS_BLUE_LED
    case LEDS_BLUE:       McuLED_Off(ledBlue); break;
  #endif
  #if LEDS_CONFIG_HAS_ORANGE_LED
    case LEDS_ORANGE:     McuLED_Off(ledOrange); break;
  #endif
  #if LEDS_CONFIG_HAS_BLUE_TINY_LED
    case LEDS_TINY_BLUE:  McuLED_Off(ledTiny); break;
  #endif
    default:
      break; /* error */
  }
}

void LEDS_Neg(LEDS_Leds_e led) {
  switch(led) {
  #if LEDS_CONFIG_HAS_RED_LED
    case LEDS_RED:        McuLED_Toggle(ledRed); break;
  #endif
  #if LEDS_CONFIG_HAS_GREEN_LED
    case LEDS_GREEN:      McuLED_Toggle(ledGreen); break;
  #endif
  #if LEDS_CONFIG_HAS_BLUE_LED
    case LEDS_BLUE:       McuLED_Toggle(ledBlue); break;
  #endif
  #if LEDS_CONFIG_HAS_ORANGE_LED
    case LEDS_ORANGE:     McuLED_Toggle(ledOrange); break;
  #endif
  #if LEDS_CONFIG_HAS_BLUE_TINY_LED
    case LEDS_TINY_BLUE:  McuLED_Toggle(ledTiny); break;
  #endif
    default:
      break; /* error */
  }
}

#include "McuUtility.h"

static int32_t LEDS_value;

static uint8_t PrintStatus(McuShell_ConstStdIOType *io) {
  uint8_t buf[16];

  McuShell_SendStatusStr((const unsigned char*)"led", (const unsigned char*)"LED module status\r\n", io->stdOut);
  if (McuLED_Get(ledRed)) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"is ON\r\n");
  } else {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"is OFF\r\n");
  }
  McuShell_SendStatusStr((const unsigned char*)"  red", (const unsigned char*)buf, io->stdOut);

  if (McuLED_Get(ledRed)) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"is ON\r\n");
  } else {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"is OFF\r\n");
  }
  McuShell_SendStatusStr((const unsigned char*)"  green", (const unsigned char*)buf, io->stdOut);

  McuUtility_Num32sToStr(buf, sizeof(buf), LEDS_value);
  McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  McuShell_SendStatusStr((const unsigned char*)"  val", (const unsigned char*)buf, io->stdOut);
  return ERR_OK;
}

static uint8_t PrintHelp(McuShell_ConstStdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"led", (const unsigned char*)"Group of LED commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (const unsigned char*)"Print help or status information\r\n", io->stdOut);
#if LEDS_CONFIG_HAS_RED_LED
  McuShell_SendHelpStr((unsigned char*)"  red on|off|neg", (const unsigned char*)"Control red LED\r\n", io->stdOut);
#endif
#if LEDS_CONFIG_HAS_GREEN_LED
  McuShell_SendHelpStr((unsigned char*)"  green on|off|neg", (const unsigned char*)"Control green LED\r\n", io->stdOut);
#endif
  McuShell_SendHelpStr((unsigned char*)"  val <val>", (const unsigned char*)"Set value\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t LEDS_ParseCommand(const uint8_t *cmd, bool *handled, McuShell_ConstStdIOType *io) {
  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "led help")==0) {
    *handled = TRUE;
    return PrintHelp(io);
  } else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "led status")==0)) {
    *handled = TRUE;
    return PrintStatus(io);
#if LEDS_CONFIG_HAS_RED_LED
  } else if (McuUtility_strcmp((char*)cmd, "led red on")==0) {
    *handled = TRUE;
    LEDS_On(LEDS_RED);
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, "led red off")==0) {
    *handled = TRUE;
    LEDS_Off(LEDS_RED);
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, "led red neg")==0) {
    *handled = TRUE;
    LEDS_Neg(LEDS_RED);
    return ERR_OK;
#endif
#if LEDS_CONFIG_HAS_GREEN_LED
  } else if (McuUtility_strncmp((char*)cmd, "led green ", sizeof("led green ")-1)==0) {
    const uint8_t *p;

    p = cmd+sizeof("led green ")-1;
    if (McuUtility_strcmp((char*)p, "on")==0) {
      *handled = TRUE;
      LEDS_On(LEDS_GREEN);
      return ERR_OK;
    } else if (McuUtility_strcmp((char*)p, "off")==0) {
      *handled = TRUE;
      LEDS_Off(LEDS_GREEN);
      return ERR_OK;
    } else if (McuUtility_strcmp((char*)p, "neg")==0) {
      *handled = TRUE;
      LEDS_Neg(LEDS_GREEN);
      return ERR_OK;
    }
    return ERR_FAILED;
#endif
  } else if (McuUtility_strncmp((char*)cmd, "led val  ", sizeof("led val ")-1)==0) {
    const uint8_t *p;

    *handled = TRUE;
    p = cmd+sizeof("led val ")-1;
    return McuUtility_ScanDecimal32sNumber(&p, &LEDS_value);
  }
  return ERR_OK; /* no error */
}

void LEDS_Init(void) {
  LEDS_CONFIG_ENABLE_CLOCK();
  McuLED_Config_t config;

  McuLED_GetDefaultConfig(&config);
  config.isOnInit = false;

#if LEDS_CONFIG_HAS_RED_LED
  config.hw.gpio = LEDS_CONFIG_RED_GPIO;
  config.hw.port = LEDS_CONFIG_RED_PORT;
  config.hw.pin = LEDS_CONFIG_RED_PIN;
  #if McuLib_CONFIG_CPU_IS_LPC
  config.hw.iocon = LEDS_CONFIG_RED_IOCON;
  #endif
  config.isLowActive = LEDS_CONFIG_RED_LOW_ACTIVE;
  ledRed = McuLED_InitLed(&config);
  if (ledRed==NULL) {
    for(;;) {}
  }
#endif

#if LEDS_CONFIG_HAS_GREEN_LED
  config.isLowActive = true;
  config.hw.gpio = LEDS_CONFIG_GREEN_GPIO;
  config.hw.port = LEDS_CONFIG_GREEN_PORT;
  config.hw.pin = LEDS_CONFIG_GREEN_PIN;
  #if McuLib_CONFIG_CPU_IS_LPC
  config.hw.iocon = LEDS_CONFIG_GREEN_IOCON;
  #endif
  config.isLowActive = LEDS_CONFIG_GREEN_LOW_ACTIVE;
  ledGreen = McuLED_InitLed(&config);
  if (ledGreen==NULL) {
    for(;;) {}
  }
#endif

#if LEDS_CONFIG_HAS_BLUE_LED
  config.isLowActive = true;
  config.hw.gpio = LEDS_CONFIG_BLUE_GPIO;
  config.hw.port = LEDS_CONFIG_BLUE_PORT;
  config.hw.pin = LEDS_CONFIG_BLUE_PIN;
  #if McuLib_CONFIG_CPU_IS_LPC
  config.hw.iocon = LEDS_CONFIG_BLUE_IOCON;
  #endif
  config.isLowActive = LEDS_CONFIG_BLUE_LOW_ACTIVE;
  ledBlue = McuLED_InitLed(&config);
  if (ledBlue==NULL) {
    for(;;) {}
  }
#endif

#if LEDS_CONFIG_HAS_ORANGE_LED
  config.isLowActive = false;
  config.hw.gpio = LEDS_CONFIG_ORANGE_GPIO;
  config.hw.port = LEDS_CONFIG_ORANGE_PORT;
  config.hw.pin = LEDS_CONFIG_ORANGE_PIN;
  ledOrange = McuLED_InitLed(&config);
  config.isLowActive = LEDS_CONFIG_ORANGE_LOW_ACTIVE;
  if (ledOrange==NULL) {
    for(;;) {}
  }
#endif

#if LEDS_CONFIG_HAS_BLUE_TINY_LED
  config.isLowActive = true;
  config.hw.gpio = LEDS_CONFIG_BLUE_TINY_GPIO;
  config.hw.port = LEDS_CONFIG_BLUE_TINY_PORT;
  config.hw.pin = LEDS_CONFIG_BLUE_TINY_PIN;
  config.isLowActive = LEDS_CONFIG_BLUE_TINY_LOW_ACTIVE;
  ledTiny = McuLED_InitLed(&config);
  if (ledTiny==NULL) {
    for(;;) {}
  }
#endif
}

void LED_Deinit(void) {
#if LEDS_CONFIG_HAS_RED_LED
  ledRed = McuLED_DeinitLed(ledRed);
#endif
#if LEDS_CONFIG_HAS_GREEN_LED
  ledGreen = McuLED_DeinitLed(ledGreen);
#endif
#if LEDS_CONFIG_HAS_BLUE_LED
  ledBlue = McuLED_DeinitLed(ledBlue);
#endif
#if LEDS_CONFIG_HAS_ORANGE_LED
  ledOrange = McuLED_DeinitLed(ledOrange);
#endif
#if LEDS_CONFIG_HAS_BLUE_TINY_LED
  ledTiny = McuLED_DeinitLed(ledTiny);
#endif
}
