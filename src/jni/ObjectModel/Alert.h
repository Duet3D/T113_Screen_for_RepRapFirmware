/*
 * Alert.h
 *
 *  Created on: 23 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_UI_ALERT_H_
#define JNI_UI_ALERT_H_

#include "Configuration.h"
#include "Duet3D/General/Bitmap.h"
#include "Duet3D/General/String.h"
#include "Duet3D/General/Vector.h"
#include "uart/CommDef.h"
#include <cstddef>
#include <float.h>

namespace OM
{
	struct Alert
	{
		enum class Mode
		{
			None = -1,
			Info = 0,
			InfoClose = 1,
			InfoConfirm = 2,
			ConfirmCancel = 3,
			Choices = 4,
			NumberInt = 5,
			NumberFloat = 6,
			Text = 7,
			Max
		} mode;
		uint32_t seq;
		uint32_t controls;
		float timeout;
		Bitmap<uint8_t> flags;
		String<ALERT_TITLE_LENGTH> title;
		String<ALERT_TEXT_LENGTH> text;

		static constexpr uint8_t GotMode = 0;
		static constexpr uint8_t GotSeq = 1;
		static constexpr uint8_t GotTimeout = 2;
		static constexpr uint8_t GotTitle = 3;
		static constexpr uint8_t GotText = 4;
		static constexpr uint8_t GotControls = 5;
		static constexpr uint8_t GotAll =
			(1 << GotMode) | (1 << GotSeq) | (1 << GotTimeout) | (1 << GotTitle) | (1 << GotText) | (1 << GotControls);

		bool cancelButton;
		String<ALERT_CHOICES_TEXT_LENGTH> choices[ALERT_MAX_CHOICES];
		size_t choices_count;

		struct Limits
		{
			struct
			{
				int32_t min;
				int32_t max;
				int32_t valueDefault;
			} numberInt;
			struct
			{
				float min;
				float max;
				float valueDefault;
			} numberFloat;
			struct
			{
				int32_t min;
				int32_t max;
				String<ALERT_RESPONSE_LENGTH> valueDefault;
			} text;
		} limits;

		Alert() : mode(Mode::Info), seq(0), controls(0), timeout(0.0) { Reset(); }

		bool AllFlagsSet() const { return flags.GetRaw() == GotAll; }
		void Reset();
	};

	extern Alert g_currentAlert;
	extern uint32_t g_lastAlertSeq;
} // namespace OM

#endif /* JNI_UI_ALERT_H_ */
