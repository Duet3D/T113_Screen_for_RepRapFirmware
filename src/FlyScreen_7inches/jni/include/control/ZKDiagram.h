/*
 * ZKDiagram.h
 *
 *  Created on: Jul 31, 2017
 *      Author: guoxs
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
        E_DIAGRAM_STYLE_LINE,        // Polyline
        E_DIAGRAM_STYLE_CURVE        // Curve
    } EDiagramStyle;

public:
    ZKDiagram(HWND hParentWnd);
    virtual ~ZKDiagram();

    /**
     * @brief Set waveform line width
     * @param index Waveform index
     * @param width Waveform line width
     */
    void setPenWidth(int index, int width);

    /**
     * @brief Set waveform color
     * @param index Waveform index
     * @param color Waveform color value: 0xARGB
     */
    void setPenColor(int index, ARGB color);

    /**
     * @brief Set X-axis scaling factor for the waveform
     * @param index Waveform index
     * @param xScale X-axis scaling factor
     */
    void setXScale(int index, double xScale);

    /**
     * @brief Set Y-axis scaling factor for the waveform
     * @param index Waveform index
     * @param yScale Y-axis scaling factor
     */
    void setYScale(int index, double yScale);

    /**
     * @brief Set waveform data
     * @param index Waveform index
     * @param pPoints Coordinate array
     * @param count Array length
     */
    void setData(int index, const MPPOINT *pPoints, int count);

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

    void addDiagramInfo(int width, ARGB color, EDiagramStyle style,
            double xScale, double yScale, float step, UINT eraseSpace, bool isAntialiasOn);

protected:
    ZKDiagram(HWND hParentWnd, ZKBasePrivate *pBP);

    virtual void onBeforeCreateWindow(const Json::Value &json);
    virtual const char* getClassName() const { return ZK_DIAGRAM; }

    virtual void onDraw(HDC hdc);

    void _section_(zk) drawDiagram(HDC hdc);

private:
    void _section_(zk) parseDiagramAttributeFromJson(const Json::Value &json);

private:
    typedef struct {
        float lower;
        float upper;
    } SAxisRange;

    typedef struct {
        HGRAPHICS graphics;
        HPATH path;
        HPEN pen;
        int penWidth;
        ARGB penColor;
        EDiagramStyle style;
        double xScale;
        double yScale;
        MPPOINT bufPoints[8];
        int bufIndex;
        float step;
        UINT eraseSpace;
        MPMatrix matrix;
    } SDiagramInfo;

    void updateMatrix(SDiagramInfo &info);
    void transform(const MPMatrix &matrix, float &x, float &y) const;

private:
    vector<SDiagramInfo *> mDiagramInfoList;

    LayoutPosition mRegionPosition;        // Curve chart drawing area

    SAxisRange mXAxisRange;        // X-axis coordinate range
    SAxisRange mYAxisRange;        // Y-axis coordinate range

    double mXBaseScale;
    double mYBaseScale;
};

#endif /* _CONTROL_ZKDIAGRAM_H_ */
