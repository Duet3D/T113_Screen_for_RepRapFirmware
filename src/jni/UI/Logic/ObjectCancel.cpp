/*
 * ObjectCancel.cpp
 *
 *  Created on: 19 Apr 2024
 *      Author: andy
 */

#include "Debug.h"

#include "ObjectCancel.h"
#include "UI/Themes.h"
#include "control/ZKPainter.h"

namespace UI::ObjectCancel
{
	static ZKPainter* s_canvas;

	static std::string GetObjectLabel(OM::JobObject* object)
	{
		if (object == nullptr)
		{
			return "";
		}

		return utils::format("[%d]: %s", object->index, object->name.c_str());
	}

	static int ConvertPointToCanvas(int point, float min, float max, int canvasSize, bool invert)
	{
		verbose("point(%d), min(%f), max(%f), canvasSize(%d), invert(%d)", point, min, max, canvasSize, invert);
		if (max - min == 0)
		{
			return 0;
		}
		if (invert)
		{
			return canvasSize - canvasSize * (point - min) / (max - min);
		}
		return canvasSize * (point - min) / (max - min);
	}

	static LayoutPosition ConvertBoundsToCanvas(const OM::JobObject& object, const LayoutPosition& canvasPos)
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
		verbose("Converted Layout position: (%d, %d, %d, %d)", pos.mLeft, pos.mTop, pos.mWidth, pos.mHeight);
		return pos;
	}

	static bool TouchInObjectBounds(const OM::JobObject& object, const LayoutPosition& canvasPos, const MotionEvent& ev)
	{
		LayoutPosition pos = ConvertBoundsToCanvas(object, canvasPos);
		if (ev.mX < pos.mLeft || ev.mX > pos.mLeft + pos.mWidth)
		{
			return false;
		}
		if (ev.mY < pos.mTop || ev.mY > pos.mTop + pos.mHeight)
		{
			return false;
		}
		return true;
	}

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
			verbose("Canvas position: %d, %d, %d, %d",
					canvasPos.mLeft,
					canvasPos.mTop,
					canvasPos.mWidth,
					canvasPos.mHeight);
			dbg("Touch position: %d, %d", ev.mX, ev.mY);

			OM::JobObject* object = OM::GetJobObject(OM::GetCurrentJobObjectIndex());
			if (object != nullptr)
			{
				if (TouchInObjectBounds(*object, canvasPos, ev))
				{
					CancelJobObject(object->index);
					return;
				}
			}

			// In reverse so that it picks the top object first
			if (OM::GetJobObjectCount() <= 0)
			{
				info("No objects to cancel");
				return;
			}

			for (int i = OM::GetJobObjectCount() - 1; i >= 0; i--)
			{
				object = OM::GetJobObject(i);
				if (object == nullptr)
				{
					warn("Invalid object index %d", i);
					continue;
				}
				if (TouchInObjectBounds(*object, canvasPos, ev))
				{
					CancelJobObject(object->index);
					return;
				}
			}
		}
	}

	void Init()
	{
		info("Initialising ObjectCancel UI...");
		s_canvas = UI::GetUIControl<ZKPainter>(ID_MAIN_ObjectCancelPainter);
		s_canvas->setTouchable(true);
		s_canvas->setTouchListener(&UI::ObjectCancel::GetTouchListener());
		UI::GetUIControl<ZKButton>(ID_MAIN_CancelCurrentObjectBtn)->setInvalid(OM::GetCurrentJobObjectIndex() < 0);
	}

	void SetObjectCancelXAxisText(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		OM::Move::Axis* axis = OM::Move::GetAxisByLetter('X');
		if (axis == nullptr)
		{
			error("Failed to get axis");
			pListItem->setText("");
			return;
		}
		dbg("Setting X axis %d text", index);

		float min = axis->minPosition;
		float max = axis->maxPosition;

		pListItem->setTextf("%.0f", min + (max - min) * (float)index / (pListView->getCols() - 1));
	}

	void SetObjectCancelYAxisText(ZKListView* pListView, ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		OM::Move::Axis* axis = OM::Move::GetAxisByLetter('Y');
		if (axis == nullptr)
		{
			error("Failed to get axis");
			pListItem->setText("");
			return;
		}
		dbg("Setting Y axis %d text", index);

		float min = axis->minPosition;
		float max = axis->maxPosition;

		pListItem->setTextf("%.0f", max - (max - min) * (float)index / (pListView->getRows() - 1));
	}

	void SetObjectLabel(ZKListView::ZKListItem* pListItem, const int index)
	{
		verbose("%d", index);
		OM::JobObject* object = OM::GetJobObject(index);
		pListItem->setText(GetObjectLabel(object));
		if (object == nullptr)
		{
			warn("Invalid object index %d", index);
			return;
		}
		const UI::Theme::Theme* theme = UI::Theme::GetCurrentTheme();
		if (theme == nullptr)
		{
			warn("Failed to get current theme");
			return;
		}

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
		UI::POPUP_WINDOW.Open([index, object]() {
			if (object == nullptr)
			{
				warn("Invalid object index %d", index);
				return;
			}
			OM::SetJobObjectActive(object->index, object->cancelled);
		});
		UI::POPUP_WINDOW.SetTitle(
			LANGUAGEMANAGER->getValue(object->cancelled ? "resume_object" : "cancel_object").c_str());
		UI::POPUP_WINDOW.SetTextf(LANGUAGEMANAGER->getValue("cancel_object_text").c_str(), index, object->name.c_str());
		UI::POPUP_WINDOW.CancelTimeout();
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

		UI::POPUP_WINDOW.Open([index]() { OM::SetJobObjectActive(index, false); });
		UI::POPUP_WINDOW.SetTitle(LANGUAGEMANAGER->getValue("cancel_object").c_str());
		UI::POPUP_WINDOW.SetTextf(LANGUAGEMANAGER->getValue("cancel_object_text").c_str(), index, object->name.c_str());
		UI::POPUP_WINDOW.CancelTimeout();
	}

	static void DrawObject(const OM::JobObject* object,
						   ZKPainter* canvas,
						   const UI::Theme::Theme* theme,
						   const bool current)
	{
		if (object == nullptr || canvas == nullptr || theme == nullptr)
		{
			warn("object, canvas, or theme is null");
			return;
		}
		verbose("Drawing Object %d (%d, %d) -> (%d, %d)",
				object->index,
				object->bounds.x[0],
				object->bounds.y[0],
				object->bounds.x[1],
				object->bounds.y[1]);
		if (object->cancelled)
		{
			canvas->setSourceColor(theme->colors->objectCancel.bgCancelled);
		}
		else if (current)
		{
			canvas->setSourceColor(theme->colors->objectCancel.bgCurrent);
		}
		else
		{
			canvas->setSourceColor(theme->colors->objectCancel.bgDefault);
		}

		// Fill
		LayoutPosition pos = ConvertBoundsToCanvas(*object, canvas->getPosition());
		canvas->fillRect(pos.mLeft, pos.mTop, pos.mWidth, pos.mHeight);

		// Border
		canvas->setSourceColor(theme->colors->objectCancel.bgBorder);
		canvas->drawRect(pos.mLeft, pos.mTop, pos.mWidth, pos.mHeight);
		verbose("Finished drawing object %u", object->index);
		return;
	}

	void RenderObjectMap()
	{
		if (s_canvas == nullptr)
		{
			error("Failed to get canvas");
			return;
		}
		static LayoutPosition canvasPos = s_canvas->getPosition();

		const UI::Theme::Theme* theme = UI::Theme::GetCurrentTheme();
		if (theme == nullptr)
		{
			warn("Failed to get current theme");
			return;
		}
		dbg("Rendering ObjectCancel canvas");

		s_canvas->erase(0, 0, canvasPos.mWidth, canvasPos.mHeight);
		s_canvas->setLineWidth(3);

		OM::IterateJobObjectsWhile(
			[&](OM::JobObject*& object, size_t index)
			{
				if (object == nullptr)
				{
					warn("Invalid object index %d", index);
					return true;
				}
				DrawObject(object, s_canvas, theme, index == (size_t)OM::GetCurrentJobObjectIndex());
				return true;
			},
			0);
		verbose("Drawing current object");
		// Draw the current object last so it is on top
		OM::JobObject* object = OM::GetJobObject(OM::GetCurrentJobObjectIndex());
		if (object != nullptr)
			DrawObject(object, s_canvas, theme, true);
	}

	TouchListener& GetTouchListener()
	{
		static TouchListener listener;
		return listener;
	}

} // namespace UI::ObjectCancel
