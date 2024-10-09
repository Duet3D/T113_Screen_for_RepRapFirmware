/*
 * ZKDiagram.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Jul 31, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKDIAGRAM_H_
#define _CONTROL_ZKDIAGRAM_H_

#include <vector>
#include "ZKBase.h"

class ZKDiagramPrivate;

/**
 * @brief Waveform chart control
 */
class ZKDiagram : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKDiagram)

public:
	typedef enum {
		E_DIAGRAM_STYLE_LINE, // Line
		E_DIAGRAM_STYLE_CURVE // Curve
	} EDiagramStyle;

public:
	ZKDiagram(ZKBase *pParent);
	virtual ~ZKDiagram();

	/**
	 * @brief Set the waveform line width
	 * @param index Waveform index
	 * @param width Waveform line width
	 */
	void setPenWidth(int index, uint32_t width);

	/**
	 * @brief Set the waveform color
	 * @param index Waveform index
	 * @param color Waveform color value: 0xRGB
	 */
	void setPenColor(int index, uint32_t color);

	/**
	 * @brief Set the X-axis scaling factor of the waveform
	 * @param index Waveform index
	 * @param xScale X-axis scaling factor
	 */
	void setXScale(int index, float xScale);

	/**
	 * @brief Set the Y-axis scaling factor of the waveform
	 * @param index Waveform index
	 * @param yScale Y-axis scaling factor
	 */
	void setYScale(int index, float yScale);

	/**
	 * @brief Set the waveform step value
	 * @param index Waveform index
	 * @param step Step value
	 */
	void setStep(int index, float step);

	/**
	 * @brief Set the waveform data
	 * @param index Waveform index
	 * @param pPoints Coordinate array
	 * @param count Array length
	 */
	void setData(int index, const SZKPoint *pPoints, int count);

	/**
	 * @brief Add waveform data
	 * @param index Waveform index
	 * @param data Y-axis data value
	 */
	void addData(int index, float data);

	/**
	 * @brief Clear the waveform chart
	 * @param index Waveform index
	 */
	void clear(int index);

	void addDiagramInfo(uint32_t width, uint32_t color, EDiagramStyle style,
			float xScale, float yScale, float step, uint32_t eraseSpace, bool isAntialiasOn);

protected:
	ZKDiagram(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_DIAGRAM; }

	virtual void onDraw(ZKCanvas *pCanvas);

private:
	void parseDiagramAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKDIAGRAM_H_ */
