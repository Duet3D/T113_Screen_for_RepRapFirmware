/*
 * ScreenHelper.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Oct 12, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _UTILS_SCREEN_HELPER_H_
#define _UTILS_SCREEN_HELPER_H_

#include <stdio.h>
#include <stdint.h>

class ScreenHelper {
public:
  /**
   * @brief Get the screen width
   */
  static int getScreenWidth();

  /**
   * @brief Get the screen height
   */
  static int getScreenHeight();

  /**
   * @brief Get the color value of a pixel at the screen coordinates
   */
  static uint32_t getPixelColor(int x, int y);

  /**
   * @brief Take a screenshot of the screen
   * @param pSavePath The path to save the BMP image
   */
  static bool screenShot(const char* pSavePath);
};

#endif /* _UTILS_SCREEN_HELPER_H_ */
