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
	 * @brief Set beep PWM
	 * @param freq    Frequency, default is 2500
	 * @param duty    Duty cycle, default is 50
	 */
	void setBeepPWM(uint32_t freq, uint8_t duty);

	// Set custom beep callback interface
	void setCustomBeep(custom_beep_cb_t cb);

private:
	HardwareManager();

private:
	uint32_t mSystemPara;
};

#define HARDWAREMANAGER			HardwareManager::getInstance()

#endif /* _HW_HARDWARE_MANAGER_H_ */