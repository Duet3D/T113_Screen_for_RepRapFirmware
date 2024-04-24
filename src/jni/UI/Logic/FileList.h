/*
 * FileList.h
 *
 *  Created on: 23 Apr 2024
 *      Author: andy
 */

#ifndef JNI_UI_FILELIST_H_
#define JNI_UI_FILELIST_H_

#include "UI/UserInterface.h"

namespace UI::FileList
{
	void RenderFileListItem(ZKListView::ZKListItem* pListItem, int index);
	void OnFileListItemClick(int index);
} // namespace UI::FileList

#endif /* JNI_UI_FILELIST_H_ */
