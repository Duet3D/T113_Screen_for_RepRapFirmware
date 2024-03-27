/*
 * HardwareManager.h
 *
 *  Created on: Sep 23, 2020
 *      Author: ZKSWE Develop Team
 */

#ifndef _HW_HARDWARE_MANAGER_H_
#define _HW_HARDWARE_MANAGER_H_

#include <stdio.h>
#include <stdint.h>

typedef void (*custom_beep_cb_t)();

class HardwareManager {
public:
	virtual ~HardwareManager();

	static HardwareManager* getInstance();

	void beep();

	/**
	 * @brief 设置beep pwm
	 * @param freq    频率 默认2500
	 * @param duty    占空比 默认50
	 */
	void setBeepPWM(uint32_t freq, uint8_t duty);

	// 设置自定义beep回调接口
	void setCustomBeep(custom_beep_cb_t cb);

private:
	HardwareManager();

private:
	uint32_t mSystemPara;
};

#define HARDWAREMANAGER			HardwareManager::getInstance()

#endif /* _HW_HARDWARE_MANAGER_H_ */