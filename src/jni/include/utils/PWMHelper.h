/*
 * PWMHelper.h
 *
 *  Created on: May 3, 2019
 *      Author: ZKSWE Develop Team
 */

#ifndef _UTILS_PWM_HELPER_H_
#define _UTILS_PWM_HELPER_H_

#include <stdint.h>

struct pwm_dev;

/**
 * @brief PWM操作帮助类
 */
class PWMHelper {
public:
	/**
	 * @brief 构造函数
	 * @param nr 端口号
	 * @param freq 频率
	 * @param duty 占空比
	 * @param polarity 极性
	 */
	PWMHelper(int nr, uint32_t freq, uint8_t duty, uint8_t polarity);
	virtual ~PWMHelper();

	/**
	 * @brief 设置PWM频率
	 * @param freq 频率
	 * @return true 成功， false 失败
	 */
	bool setFreq(uint32_t freq);

	/**
	 * @brief 设置PWM占空比
	 * @param duty 占空比
	 * @return true 成功， false 失败
	 */
	bool setDuty(uint8_t duty);

	/**
	 * @brief 设置PWM极性
	 * @param polarity 极性
	 * @return true 成功， false 失败
	 */
	bool setPolarity(uint8_t polarity);

	/**
	 * @brief 设置PWM使能
	 * @param isEnable true 使能， false 禁止
	 * @return true 成功， false 失败
	 */
	bool setEnable(bool isEnable);

private:
	struct pwm_dev *mPwmDevPtr;
};

#endif /* _UTILS_PWM_HELPER_H_ */
