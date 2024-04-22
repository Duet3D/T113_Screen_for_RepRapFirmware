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

namespace UI
{
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
		if (index == OM::GetCurrentJobObjectIndex())
		{
			pListItem->setBackgroundColor(theme->colors->objectCancel.bgCurrent);
		}
		else
		{
			pListItem->setBackgroundColor(theme->colors->objectCancel.bgDefault);
		}
		if (object->cancelled)
		{
			pListItem->setBackgroundColor(theme->colors->objectCancel.bgCancelled);
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

} // namespace UI
