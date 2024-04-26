/*
 * BrightnessHelper.h
 *
 *  Created on: Sep 23, 2017
 *      Author: guoxs
 */

#ifndef _UTILS_BRIGHTNESS_HELPER_H_
#define _UTILS_BRIGHTNESS_HELPER_H_

/**
 * @brief Brightness adjustment helper class
 */
class BrightnessHelper {
public:
  /**
   * @brief Get the maximum brightness value, currently set to 100
   */
  int getMaxBrightness() const;

  /**
   * @brief Get the current brightness value
   */
  int getBrightness() const;

  /**
   * @brief Set the brightness value
   * @param brightness Range: 0 ~ 100
   */
  void setBrightness(int brightness);

  /**
   * @brief Turn off the screen
   */
  void screenOff();

  /**
   * @brief Turn on the screen
   */
  void screenOn();

  /**
   * @brief Turn off the backlight
   */
  void backlightOff();

  /**
   * @brief Turn on the backlight
   */
  void backlightOn();

  bool screenOffEx();
  bool screenOnEx();

  void setLCDEnable(bool enable);

  /**
   * @brief Check if the screen is on
   */
  bool isScreenOn() const;

  /**
   * @brief Set the contrast
   * @param contrast Range: 0 ~ 100
   */
  void setContrast(int contrast);

  /**
   * @brief Set the saturation
   * @param saturation Range: 0 ~ 100
   */
  void setSaturation(int saturation);

  /**
   * @brief Set the hue
   * @param hue Range: 0 ~ 100
   */
  void setHue(int hue);

  static BrightnessHelper* getInstance();

private:
	BrightnessHelper();

private:
	int mMinBrightness;
	int mMaxBrightness;
	int mBrightness;

	float mRatio;

	bool mIsScreenOn;
};

#define BRIGHTNESSHELPER		BrightnessHelper::getInstance()

#endif /* _UTILS_BRIGHTNESS_HELPER_H_ */
