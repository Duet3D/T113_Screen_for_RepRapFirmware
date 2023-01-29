/*
 * GpioHelper.h
 *
 *  Created on: May 17, 2018
 *      Author: guoxs
 */

#ifndef _UTILS_GPIO_HELPER_H_
#define _UTILS_GPIO_HELPER_H_

#include <stdint.h>

// Z11
#define GPIO_PIN_B_02		"B_02"
#define GPIO_PIN_B_03		"B_03"
#define GPIO_PIN_E_20		"E_20"

// SV50PB
#define SV50PB_PIN7			"PIN7"
#define SV50PB_PIN8			"PIN8"
#define SV50PB_PIN9			"PIN9"
#define SV50PB_PIN10		"PIN10"
#define SV50PB_PIN11		"PIN11"
#define SV50PB_PIN12		"PIN12"
#define SV50PB_PIN13		"PIN13"
#define SV50PB_PIN14		"PIN14"
#define SV50PB_PIN23		"PIN23"
#define SV50PB_PIN24		"PIN24"
#define SV50PB_PIN26		"PIN26"
#define SV50PB_PIN27		"PIN27"

// SV50PC
#define SV50PC_PIN2			"PIN2"
#define SV50PC_PIN3			"PIN3"
#define SV50PC_PIN4			"PIN4"
#define SV50PC_PIN5			"PIN5"
#define SV50PC_PIN6			"PIN6"
#define SV50PC_PIN7			"PIN7"
#define SV50PC_PIN8			"PIN8"
#define SV50PC_PIN9			"PIN9"
#define SV50PC_PIN10		"PIN10"
#define SV50PC_PIN11		"PIN11"
#define SV50PC_PIN13		"PIN13"
#define SV50PC_PIN14		"PIN14"
#define SV50PC_PIN15		"PIN15"
#define SV50PC_PIN16		"PIN16"
#define SV50PC_PIN17		"PIN17"
#define SV50PC_PIN18		"PIN18"
#define SV50PC_PIN22		"PIN22"
#define SV50PC_PIN24		"PIN24"
#define SV50PC_PIN25		"PIN25"
#define SV50PC_PIN26		"PIN26"
#define SV50PC_PIN27		"PIN27"
#define SV50PC_PIN28		"PIN28"
#define SV50PC_PIN29		"PIN29"
#define SV50PC_PIN30		"PIN30"
#define SV50PC_PIN31		"PIN31"


/**
 * @brief GPIO边沿触发方式
 */
typedef enum {
	E_GPIO_EDGE_TYPE_NONE,		/**< 无中断触发 默认值 */
	E_GPIO_EDGE_TYPE_RISING,	/**< 上升沿触发 */
	E_GPIO_EDGE_TYPE_FALLING,	/**< 下降沿触发 */
	E_GPIO_EDGE_TYPE_BOTH		/**< 上升沿或下降沿触发 */
} EGpioEdgeType;

class IGpioListener {
public:
	virtual ~IGpioListener() { }
	virtual bool onGpioEdge(const char *pPin) = 0;
	virtual void onGpioError(const char *pPin, int error) = 0;
};

/**
 * @brief GPIO操作帮助类
 */
class GpioHelper {
public:
	/**
	 * @brief 读io口状态
	 * @return -1 失败， 1 / 0(高 / 低电平) 成功
	 */
	static int input(const char *pPin);

	/**
	 * @brief 写io口状态
	 * @param val 1 高电平，0 低电平
	 * @return -1 失败， 0 成功
	 */
	static int output(const char *pPin, int val);

	/**
	 * @brief 注册io口触发监听
	 * @param pPin io口号
	 * @param pListener 监听对象
	 * @param type 触发方式
	 */
	static void registerGpioListener(const char *pPin, IGpioListener *pListener, EGpioEdgeType type);

	/**
	 * @brief 反注册io口触发监听
	 * @param pPin io口号
	 * @param pListener 监听对象
	 */
	static void unregisterGpioListener(const char *pPin, IGpioListener *pListener);
};

#endif /* _UTILS_GPIO_HELPER_H_ */
