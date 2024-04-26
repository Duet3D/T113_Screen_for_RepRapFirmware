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
 * @brief PWM operation helper class
 */
class PWMHelper {
public:
  /**
   * @brief Constructor
   * @param nr Port number
   * @param freq Frequency
   * @param duty Duty cycle
   * @param polarity Polarity
   */
  PWMHelper(int nr, uint32_t freq, uint8_t duty, uint8_t polarity);
  virtual ~PWMHelper();

  /**
   * @brief Set PWM frequency
   * @param freq Frequency
   * @return true if successful, false otherwise
   */
  bool setFreq(uint32_t freq);

  /**
   * @brief Set PWM duty cycle
   * @param duty Duty cycle
   * @return true if successful, false otherwise
   */
  bool setDuty(uint8_t duty);

  /**
   * @brief Set PWM polarity
   * @param polarity Polarity
   * @return true if successful, false otherwise
   */
  bool setPolarity(uint8_t polarity);

  /**
   * @brief Set PWM enable
   * @param isEnable true to enable, false to disable
   * @return true if successful, false otherwise
   */
  bool setEnable(bool isEnable);

private:
	struct pwm_dev *mPwmDevPtr;
};

#endif /* _UTILS_PWM_HELPER_H_ */
