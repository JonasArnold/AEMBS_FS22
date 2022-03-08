/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(CPU_LPC845M301JBD48)
  #define PL_CONFIG_IS_LPC  (1)
  #define PL_CONFIG_IS_TINY (0)
#elif defined(CPU_MK22FN512VLH12)
  #define PL_CONFIG_IS_LPC  (0)
  #define PL_CONFIG_IS_TINY (1)
#else
  #error "unknown MCU?"
#endif

/*!
 * \brief de-initializes the platform
 */
void PL_Deinit(void);

/*!
 * \brief initializes the platform
 */
void PL_Init(void);

#endif /* PLATFORM_H_ */
