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
	 * @brief 获取屏幕宽度
	 */
	static int getScreenWidth();

	/**
	 * @brief 获取屏幕高度
	 */
	static int getScreenHeight();

	/**
	 * @brief 获取屏幕坐标像素点颜色值
	 */
	static uint32_t getPixelColor(int x, int y);

	/**
	 * @brief 屏幕快照
	 * @param pSavePath BMP图片保存路径
	 */
	static bool screenShot(const char *pSavePath);
};

#endif /* _UTILS_SCREEN_HELPER_H_ */
