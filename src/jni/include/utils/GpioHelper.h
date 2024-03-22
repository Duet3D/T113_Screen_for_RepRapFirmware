/*
 * GpioHelper.h
 *
 *  Created on: May 17, 2018
 *      Author: guoxs
 */

#ifndef _UTILS_GPIO_HELPER_H_
#define _UTILS_GPIO_HELPER_H_

#include <stdint.h>

// T113
#define PA_BASE 0
#define PB_BASE 32
#define PC_BASE 64
#define PD_BASE 96
#define PE_BASE 128
#define PF_BASE 160
#define PG_BASE 192
#define PH_BASE 224
#define PI_BASE 256
#define PJ_BASE 288
#define PK_BASE 320
#define PL_BASE 352
#define PM_BASE 384
#define PN_BASE 416
#define PO_BASE 448

#define GPIO_PIN_B_02 "GPIO_34"
#define GPIO_PIN_B_03 "GPIO_35"
#define GPIO_PIN_B_04 "GPIO_36"
#define GPIO_PIN_B_05 "GPIO_37"
#define GPIO_PIN_B_06 "GPIO_38"
#define GPIO_PIN_B_07 "GPIO_39"

#define GPIO_PIN_C_02 "GPIO_66"
#define GPIO_PIN_C_03 "GPIO_67"
#define GPIO_PIN_C_04 "GPIO_68"
#define GPIO_PIN_C_05 "GPIO_69"
#define GPIO_PIN_C_06 "GPIO_70"
#define GPIO_PIN_C_07 "GPIO_71"

#define GPIO_PIN_D_00 "GPIO_96"
#define GPIO_PIN_D_01 "GPIO_97"
#define GPIO_PIN_D_02 "GPIO_98"
#define GPIO_PIN_D_03 "GPIO_99"
#define GPIO_PIN_D_04 "GPIO_100"
#define GPIO_PIN_D_05 "GPIO_101"
#define GPIO_PIN_D_06 "GPIO_102"
#define GPIO_PIN_D_07 "GPIO_103"
#define GPIO_PIN_D_08 "GPIO_104"
#define GPIO_PIN_D_09 "GPIO_105"
#define GPIO_PIN_D_10 "GPIO_106"
#define GPIO_PIN_D_11 "GPIO_107"
#define GPIO_PIN_D_12 "GPIO_108"
#define GPIO_PIN_D_13 "GPIO_109"
#define GPIO_PIN_D_14 "GPIO_110"
#define GPIO_PIN_D_15 "GPIO_111"
#define GPIO_PIN_D_16 "GPIO_112"
#define GPIO_PIN_D_17 "GPIO_113"
#define GPIO_PIN_D_18 "GPIO_114"
#define GPIO_PIN_D_19 "GPIO_115"
#define GPIO_PIN_D_20 "GPIO_116"
#define GPIO_PIN_D_21 "GPIO_117"
#define GPIO_PIN_D_22 "GPIO_118"

#define GPIO_PIN_E_00 "GPIO_128"
#define GPIO_PIN_E_01 "GPIO_129"
#define GPIO_PIN_E_02 "GPIO_130"
#define GPIO_PIN_E_03 "GPIO_131"
#define GPIO_PIN_E_04 "GPIO_132"
#define GPIO_PIN_E_05 "GPIO_133"
#define GPIO_PIN_E_06 "GPIO_134"
#define GPIO_PIN_E_07 "GPIO_135"
#define GPIO_PIN_E_08 "GPIO_136"
#define GPIO_PIN_E_09 "GPIO_137"
#define GPIO_PIN_E_10 "GPIO_138"
#define GPIO_PIN_E_11 "GPIO_139"
#define GPIO_PIN_E_12 "GPIO_140"
#define GPIO_PIN_E_13 "GPIO_141"

#define GPIO_PIN_F_00 "GPIO_160"
#define GPIO_PIN_F_01 "GPIO_161"
#define GPIO_PIN_F_02 "GPIO_162"
#define GPIO_PIN_F_03 "GPIO_163"
#define GPIO_PIN_F_04 "GPIO_164"
#define GPIO_PIN_F_05 "GPIO_165"
#define GPIO_PIN_F_06 "GPIO_166"

#define GPIO_PIN_G_00 "GPIO_192"
#define GPIO_PIN_G_01 "GPIO_193"
#define GPIO_PIN_G_02 "GPIO_194"
#define GPIO_PIN_G_03 "GPIO_195"
#define GPIO_PIN_G_04 "GPIO_196"
#define GPIO_PIN_G_05 "GPIO_197"
#define GPIO_PIN_G_06 "GPIO_198"
#define GPIO_PIN_G_07 "GPIO_199"
#define GPIO_PIN_G_08 "GPIO_200"
#define GPIO_PIN_G_09 "GPIO_201"
#define GPIO_PIN_G_10 "GPIO_202"
#define GPIO_PIN_G_11 "GPIO_203"
#define GPIO_PIN_G_12 "GPIO_204"
#define GPIO_PIN_G_13 "GPIO_205"
#define GPIO_PIN_G_14 "GPIO_206"
#define GPIO_PIN_G_15 "GPIO_207"

#if 0
// Z11
#define GPIO_PIN_B_02 "B_02"
#define GPIO_PIN_B_03 "B_03"
#define GPIO_PIN_E_20 "E_20"
#define GPIO_PIN_E_14 "E_14" // beep

// SV50PB
#define SV50PB_PIN7 "PIN7"
#define SV50PB_PIN8 "PIN8"
#define SV50PB_PIN9 "PIN9"
#define SV50PB_PIN10 "PIN10"
#define SV50PB_PIN11 "PIN11"
#define SV50PB_PIN12 "PIN12"
#define SV50PB_PIN13 "PIN13"
#define SV50PB_PIN14 "PIN14"
#define SV50PB_PIN23 "PIN23"
#define SV50PB_PIN24 "PIN24"
#define SV50PB_PIN26 "PIN26"
#define SV50PB_PIN27 "PIN27"

// SV50PC
#define SV50PC_PIN2 "PIN2"
#define SV50PC_PIN3 "PIN3"
#define SV50PC_PIN4 "PIN4"
#define SV50PC_PIN5 "PIN5"
#define SV50PC_PIN6 "PIN6"
#define SV50PC_PIN7 "PIN7"
#define SV50PC_PIN8 "PIN8"
#define SV50PC_PIN9 "PIN9"
#define SV50PC_PIN10 "PIN10"
#define SV50PC_PIN11 "PIN11"
#define SV50PC_PIN13 "PIN13"
#define SV50PC_PIN14 "PIN14"
#define SV50PC_PIN15 "PIN15"
#define SV50PC_PIN16 "PIN16"
#define SV50PC_PIN17 "PIN17"
#define SV50PC_PIN18 "PIN18"
#define SV50PC_PIN22 "PIN22"
#define SV50PC_PIN24 "PIN24"
#define SV50PC_PIN25 "PIN25"
#define SV50PC_PIN26 "PIN26"
#define SV50PC_PIN27 "PIN27"
#define SV50PC_PIN28 "PIN28"
#define SV50PC_PIN29 "PIN29"
#define SV50PC_PIN30 "PIN30"
#define SV50PC_PIN31 "PIN31"
#endif

/**
 * @brief GPIO边沿触发方式
 */
typedef enum
{
	E_GPIO_EDGE_TYPE_NONE,	  /**< 无中断触发 默认值 */
	E_GPIO_EDGE_TYPE_RISING,  /**< 上升沿触发 */
	E_GPIO_EDGE_TYPE_FALLING, /**< 下降沿触发 */
	E_GPIO_EDGE_TYPE_BOTH	  /**< 上升沿或下降沿触发 */
} EGpioEdgeType;

class IGpioListener
{
  public:
	virtual ~IGpioListener() {}
	virtual bool onGpioEdge(const char* pPin) = 0;
	virtual void onGpioError(const char* pPin, int error) = 0;
};

/**
 * @brief GPIO操作帮助类
 */
class GpioHelper
{
  public:
	static void initPinMap(uint32_t enmap, uint32_t dirmap, uint32_t levelmap);

	/**
	 * @brief 读io口状态
	 * @return -1 失败， 1 / 0(高 / 低电平) 成功
	 */
	static int input(const char* pPin);

	/**
	 * @brief 写io口状态
	 * @param val 1 高电平，0 低电平
	 * @return -1 失败， 0 成功
	 */
	static int output(const char* pPin, int val);

	/**
	 * @brief 注册io口触发监听
	 * @param pPin io口号
	 * @param pListener 监听对象
	 * @param type 触发方式
	 */
	static void registerGpioListener(const char* pPin, IGpioListener* pListener, EGpioEdgeType type);

	/**
	 * @brief 反注册io口触发监听
	 * @param pPin io口号
	 * @param pListener 监听对象
	 */
	static void unregisterGpioListener(const char* pPin, IGpioListener* pListener);
};

#endif /* _UTILS_GPIO_HELPER_H_ */
