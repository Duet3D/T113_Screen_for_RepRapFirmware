/*
 * UserInterface.cpp
 *
 *  Created on: Nov 19, 2023
 *      Author: Loïc
 */


#include <UI/UserInterface.hpp>

#include <ObjectModel/Axis.hpp>
#include <ObjectModel/Utils.hpp>

#define DEBUG (1)
#include "Debug.hpp"



static uint8_t numVisibleAxes = 0;	// initialize to 0 so we refresh the macros list when we receive the number of axes
static uint8_t numDisplayedAxes = 0;
static bool isDelta = false;

namespace UI
{

	// Show or hide an axis on the move button grid and on the axis display
	void ShowAxis(size_t slot, bool b, const char* axisLetter)
	{
		if (slot >= MaxDisplayableAxes)
		{
			return;
		}
		// TODO: uncomment things ...
		/*
		// The table gives us a pointer to the label field, which is followed by 8 buttons. So we need to show or hide 9 fields.
		DisplayField *f = moveAxisRows[slot];
		for (int i = 0; i < 9 && f != nullptr; ++i)
		{
			mgr.Show(f, b);
			if (i > 0) // actual move buttons
			{
				TextButtonForAxis *textButton = static_cast<TextButtonForAxis*>(f);
				textButton->SetAxisLetter(axisLetter[0]);
			}
			f = f->next;
		}
		mgr.Show(controlTabAxisPos[slot], b);
#if DISPLAY_X == 800
		mgr.Show(printTabAxisPos[slot], b);
#endif
		if (numDisplayedAxes < MaxDisplayableAxes)
		{
			mgr.Show(movePopupAxisPos[slot], b);		// the move popup axis positions occupy the last axis row of the move popup
		}
		else
		{
			// This is incremental and we might end up that this row is no longer available
			for (size_t i = 0; i < MaxDisplayableAxes; ++i)
			{
				mgr.Show(movePopupAxisPos[i], false);
			}
		}
		*/
	}

	// Update the geometry or the number of axes
	// TODO: uncomment things ...
	void UpdateGeometry(unsigned int p_numAxes, bool p_isDelta)
	{
		if (p_numAxes != numVisibleAxes || p_isDelta != isDelta)
		{
			numVisibleAxes = p_numAxes;
			isDelta = p_isDelta;
			//FileManager::RefreshMacrosList();
			numDisplayedAxes = 0;
			OM::IterateAxesWhile([](OM::Axis*& axis, size_t)
			{
				axis->slot = MaxTotalAxes;
				if (!axis->visible)
				{
					return true;
				}
				const char * letter = axis->letter;
				if (numDisplayedAxes < MaxDisplayableAxes)
				{
					axis->slot = numDisplayedAxes;
					++numDisplayedAxes;

					// Update axis letter everywhere we display it
					const uint8_t slot = axis->slot;

					//mAxisSlot1_labelPtr->setText(letter);
					dbg("Slot: %d - Letter: %s\n", slot, letter);

					//controlTabAxisPos[slot]->SetLabel(letter);	// TODO: comprendre pourquoi cette ligne fait planter le systeme
					/*
					moveAxisRows		[slot]->SetValue(letter);
					movePopupAxisPos	[slot]->SetLabel(letter);
					homeButtons			[slot]->SetText(letter);

					// Update axis letter to be sent for homing commands
					homeButtons[slot]->SetEvent(homeButtons[slot]->GetEvent(), letter);
					homeButtons[slot]->SetColours(colours->buttonTextColour, (axis->homed) ? colours->homedButtonBackColour : colours->notHomedButtonBackColour);

					mgr.Show(homeButtons[slot], !isDelta);
					*/
					ShowAxis(slot, true, axis->letter);
				}
				// When we get here it's likely to be the initialisation phase
				// and we won't have the babystep amount set
				if (axis->letter[0] == 'Z')
				{
					//babystepOffsetField->SetValue(axis->babystep);
				}
				return true;
			});
			// Hide axes possibly shown before
			for (size_t i = numDisplayedAxes; i < MaxDisplayableAxes; ++i)
			{
				//mgr.Show(homeButtons[i], false);
				ShowAxis(i, false);
			}
		}
	}





	void UpdateAxisPosition(size_t axisIndex, float fval)
	{
		if (axisIndex < MaxTotalAxes)
		{
			auto axis = OM::GetAxis(axisIndex);
			if (axis != nullptr && axis->slot < MaxDisplayableAxes)
			{
				size_t slot = axis->slot;
				dbg("Slot: %d - Position: %f\n", slot, fval);

				controlTabAxisPos[slot]->SetValue(fval);
				/*
				movePopupAxisPos[slot]->SetValue(fval);
				*/
			}
		}
	}

	void SetAxisLetter(size_t index, char l)
	{
		if (index < MaxTotalAxes)
		{
			OM::Axis *axis = OM::GetOrCreateAxis(index);
			if (axis != nullptr)
			{
				axis->letter[0] = l;
			}
		}
	}

	void SetAxisVisible(size_t index, bool v)
	{
		if (index < MaxTotalAxes)
		{
			OM::Axis *axis = OM::GetOrCreateAxis(index);
			if (axis != nullptr)
			{
				axis->visible = v;
			}
		}
	}
}
