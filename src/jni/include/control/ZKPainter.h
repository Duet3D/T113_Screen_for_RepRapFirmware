/*
 * ZKPainter.h - Zkswe
 *
 * Copyright (C) 2019 Zkswe Technology Corp.
 *
 *  Created on: Feb 20, 2019
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKPAINTER_H_
#define _CONTROL_ZKPAINTER_H_

#include "ZKBase.h"

class ZKPainterPrivate;

/**
 * @brief Canvas control
 */
class ZKPainter : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKPainter)

public:
	ZKPainter(ZKBase *pParent);
	virtual ~ZKPainter();

	/**
	 * @brief Set line width
	 * @param width Line width
	 */
	void setLineWidth(uint32_t width);

	/**
	 * @brief Set color value
	 * @param color Color value in 0xRGB format
	 */
	void setSourceColor(uint32_t color);

	/**
	 * @brief Draw a triangle
	 * @param x0 X coordinate of the first vertex
	 * @param y0 Y coordinate of the first vertex
	 * @param x1 X coordinate of the second vertex
	 * @param y1 Y coordinate of the second vertex
	 * @param x2 X coordinate of the third vertex
	 * @param y2 Y coordinate of the third vertex
	 */
	void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

	/**
	 * @brief Draw a rectangle
	 * @param left X coordinate of the top-left corner
	 * @param top Y coordinate of the top-left corner
	 * @param width Rectangle width
	 * @param height Rectangle height
	 * @param radius Corner radius, default value is 0 for a rectangle without rounded corners
	 */
	void drawRect(int left, int top, int width, int height, int radius = 0);

	/**
	 * @brief Draw an arc
	 * @param centerX X coordinate of the center point
	 * @param centerY Y coordinate of the center point
	 * @param radiusX Radius in the X direction
	 * @param radiusY Radius in the Y direction, default value is 0 for a circle, otherwise it's an ellipse
	 * @param startAngle Starting angle, 0 degrees is vertically upward, positive in clockwise direction, negative in
	 * counterclockwise direction; default value is 0 degrees
	 * @param sweepAngle Offset angle, positive in clockwise direction, negative in counterclockwise direction; default
	 * value is 360 degrees
	 */
	void drawArc(int centerX, int centerY, int radiusX, int radiusY = 0, int startAngle = 0, int sweepAngle = 360);

	/**
	 * @brief Fill a triangle
	 * @param x0 X coordinate of the first vertex
	 * @param y0 Y coordinate of the first vertex
	 * @param x1 X coordinate of the second vertex
	 * @param y1 Y coordinate of the second vertex
	 * @param x2 X coordinate of the third vertex
	 * @param y2 Y coordinate of the third vertex
	 */
	void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2);

	/**
	 * @brief Fill a rectangle
	 * @param left X coordinate of the top-left corner
	 * @param top Y coordinate of the top-left corner
	 * @param width Rectangle width
	 * @param height Rectangle height
	 * @param radius Corner radius, default value is 0 for a rectangle without rounded corners
	 */
	void fillRect(int left, int top, int width, int height, int radius = 0);

	/**
	 * @brief Fill an arc
	 * @param centerX X coordinate of the center point
	 * @param centerY Y coordinate of the center point
	 * @param radiusX Radius in the X direction
	 * @param radiusY Radius in the Y direction, default value is 0 for a circle, otherwise it's an ellipse
	 * @param startAngle Starting angle, 0 degrees is vertically upward, positive in clockwise direction, negative in
	 * counterclockwise direction; default value is 0 degrees
	 * @param sweepAngle Offset angle, positive in clockwise direction, negative in counterclockwise direction; default
	 * value is 360 degrees
	 */
	void fillArc(int centerX, int centerY, int radiusX, int radiusY = 0, int startAngle = 0, int sweepAngle = 360);

	/**
	 * @brief Draw lines
	 * @param pPoints Coordinate array
	 * @param count Array length
	 */
	void drawLines(const SZKPoint *pPoints, int count);

	/**
	 * @brief Draw curves
	 * @param pPoints Coordinate array
	 * @param count Array length
	 */
	void drawCurve(const SZKPoint *pPoints, int count);

	/**
	 * @brief Erase
	 */
	void erase(int x, int y, int w, int h);

protected:
	ZKPainter(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_PAINTER; }

	virtual void onDraw(ZKCanvas *pCanvas);

private:
	void parsePainterAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKPAINTER_H_ */
