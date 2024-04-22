/*
 * ObjectCancel.cpp
 *
 *  Created on: 19 Apr 2024
 *      Author: andy
 */

#include "DebugLevels.h"
#define DEBUG_LEVEL DEBUG_LEVEL_DBG
#include "Debug.h"

#include "ObjectCancel.h"
#include "Themes.h"
#include "control/ZKPainter.h"

namespace UI::ObjectCancel
{
	void TouchListener::onTouchEvent(ZKBase* pBase, const MotionEvent& ev)
	{
		if (ev.mActionStatus == MotionEvent::E_ACTION_UP)
		{
			ZKPainter* canvas = dynamic_cast<ZKPainter*>(pBase);
			if (canvas == nullptr)
			{
				error("Failed to get canvas");
				return;
			}

			LayoutPosition canvasPos = canvas->getPosition();
			info("Canvas position: %d, %d, %d, %d",
				 canvasPos.mLeft,
				 canvasPos.mTop,
				 canvasPos.mWidth,
				 canvasPos.mHeight);
			info("Touch position: %d, %d", ev.mX, ev.mY);
		}
	}

	std::string GetObjectCancelXAxisText(int index)
	{
		static ZKListView* axisList = UI::GetUIControl<ZKListView>(ID_MAIN_ObjectCancelXAxis);

		if (axisList == nullptr)
		{
			error("Failed to get axis list");
			return "";
		}

		OM::Move::Axis* axis = OM::Move::GetAxisByLetter('X');
		if (axis == nullptr)
		{
			error("Failed to get axis");
			return "";
		}

		float min = axis->minPosition;
		float max = axis->maxPosition;

		return utils::format("%.0f", min + (max - min) * (float)index / (axisList->getCols() - 1));
	}

	std::string GetObjectCancelYAxisText(int index)
	{
		static ZKListView* axisList = UI::GetUIControl<ZKListView>(ID_MAIN_ObjectCancelYAxis);

		if (axisList == nullptr)
		{
			error("Failed to get axis list");
			return "";
		}

		OM::Move::Axis* axis = OM::Move::GetAxisByLetter('Y');
		if (axis == nullptr)
		{
			error("Failed to get axis");
			return "";
		}

		float min = axis->minPosition;
		float max = axis->maxPosition;

		return utils::format("%.0f", max - (max - min) * (float)index / (axisList->getRows() - 1));
	}

	static std::string GetObjectLabel(OM::JobObject* object)
	{
		if (object == nullptr)
		{
			return "";
		}

		return utils::format("[%d]: %s", object->index, object->name.c_str());
	}

	void SetObjectLabel(ZKListView::ZKListItem* pListItem, const int index)
	{
		OM::JobObject* object = OM::GetJobObject(index);
		pListItem->setText(GetObjectLabel(object));
		if (object == nullptr)
		{
			warn("Invalid object index %d", index);
			return;
		}
		const UI::Theme::Theme* theme = UI::Theme::GetCurrentTheme();
		if (object->cancelled)
		{
			pListItem->setBackgroundColor(theme->colors->objectCancel.bgCancelled);
		}
		else if (index == OM::GetCurrentJobObjectIndex())
		{
			pListItem->setBackgroundColor(theme->colors->objectCancel.bgCurrent);
		}
		else
		{
			pListItem->setBackgroundColor(theme->colors->objectCancel.bgDefault);
		}
	}

	void CancelJobObject(const int index)
	{
		OM::JobObject* object = OM::GetJobObject(index);
		UI::POPUP_WINDOW->Open([index, object]() {
			if (object == nullptr)
			{
				warn("Invalid object index %d", index);
				return;
			}
			OM::SetJobObjectActive(object->index, object->cancelled);
		});
		UI::POPUP_WINDOW->SetTitle(
			LANGUAGEMANAGER->getValue(object->cancelled ? "resume_object" : "cancel_object").c_str());
		UI::POPUP_WINDOW->SetTextf(
			LANGUAGEMANAGER->getValue("cancel_object_text").c_str(), index, object->name.c_str());
	}

	void CancelCurrentJobObject()
	{
		int8_t index = OM::GetCurrentJobObjectIndex();
		OM::JobObject* object = OM::GetJobObject(index);
		if (object == nullptr)
		{
			warn("Invalid object index %d", index);
			return;
		}

		UI::POPUP_WINDOW->Open([index]() { OM::SetJobObjectActive(index, false); });
		UI::POPUP_WINDOW->SetTitle(LANGUAGEMANAGER->getValue("cancel_object").c_str());
		UI::POPUP_WINDOW->SetTextf(
			LANGUAGEMANAGER->getValue("cancel_object_text").c_str(), index, object->name.c_str());
	}

	static int ConvertPointToCanvas(int point, float min, float max, int canvasSize, bool invert)
	{
		if (invert)
		{
			return canvasSize - canvasSize * (point - min) / (max - min);
		}
		return canvasSize * (point - min) / (max - min);
	}

	static LayoutPosition ConvertBoundsToCanvas(OM::JobObject& object, const LayoutPosition& canvasPos)
	{
		LayoutPosition pos;

		OM::Move::Axis* xAxis = OM::Move::GetAxisByLetter('X');
		OM::Move::Axis* yAxis = OM::Move::GetAxisByLetter('Y');
		if (xAxis == nullptr || yAxis == nullptr)
		{
			warn("Failed to get axes");
			return pos;
		}

		pos.mLeft =
			ConvertPointToCanvas(object.bounds.x[0], xAxis->minPosition, xAxis->maxPosition, canvasPos.mWidth, false);
		pos.mTop =
			ConvertPointToCanvas(object.bounds.y[1], yAxis->minPosition, yAxis->maxPosition, canvasPos.mHeight, true);

		pos.mWidth =
			ConvertPointToCanvas(object.bounds.x[1], xAxis->minPosition, xAxis->maxPosition, canvasPos.mWidth, false) -
			pos.mLeft;
		pos.mHeight =
			ConvertPointToCanvas(object.bounds.y[0], yAxis->minPosition, yAxis->maxPosition, canvasPos.mHeight, true) -
			pos.mTop;
		return pos;
	}

	void RenderObjectMap()
	{
		static ZKPainter* canvas = UI::GetUIControl<ZKPainter>(ID_MAIN_ObjectCancelPainter);
		if (canvas == nullptr)
		{
			error("Failed to get canvas");
			return;
		}
		static LayoutPosition canvasPos = canvas->getPosition();

		const UI::Theme::Theme* theme = UI::Theme::GetCurrentTheme();

		canvas->erase(0, 0, canvasPos.mWidth, canvasPos.mHeight);
		canvas->setLineWidth(3);

		OM::IterateJobObjectsWhile(
			[&](OM::JobObject*& object, size_t index) {
				if (object == nullptr)
				{
					return true;
				}
				if (object->cancelled)
				{
					canvas->setSourceColor(theme->colors->objectCancel.bgCancelled);
				}
				else if (index == (size_t)OM::GetCurrentJobObjectIndex())
				{
					canvas->setSourceColor(theme->colors->objectCancel.bgCurrent);
				}
				else
				{
					canvas->setSourceColor(theme->colors->objectCancel.bgDefault);
				}

				// Fill
				LayoutPosition pos = ConvertBoundsToCanvas(*object, canvasPos);
				canvas->fillRect(pos.mLeft, pos.mTop, pos.mWidth, pos.mHeight);

				// Border
				canvas->setSourceColor(theme->colors->objectCancel.bgBorder);
				canvas->drawRect(pos.mLeft, pos.mTop, pos.mWidth, pos.mHeight);
				return true;
			},
			0);
	}

	TouchListener& GetTouchListener()
	{
		static TouchListener listener;
		return listener;
	}

} // namespace UI
