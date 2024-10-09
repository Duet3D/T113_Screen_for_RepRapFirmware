/*
 * ZKListView.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Jun 28, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKLISTVIEW_H_
#define _CONTROL_ZKLISTVIEW_H_

#include "ZKButton.h"

class ZKListViewPrivate;
class ZKListSubItemPrivate;

/**
 * @brief List control
 */
class ZKListView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKListView)

public:
	ZKListView(ZKBase *pParent);
	virtual ~ZKListView();

public:
	class ZKListItem;
	class ZKListSubItem : public ZKButton {
		friend class ZKListView;
		friend class ZKListItem;
		ZK_DECLARE_PRIVATE(ZKListSubItem)
	public:
		ZKListSubItem();
		virtual ~ZKListSubItem();

	protected:
		virtual bool createWindow() { return false; }
		virtual const char* getClassName() const { return NULL; }

		void drawSubItem(ZKCanvas *pCanvas, int xOffset, int yOffset);

		int getLongClickTimeOut() const;
		int getLongClickIntervalTime() const;
	};

	class ZKListItem : public ZKListSubItem {
		friend class ZKListView;
		friend class ZKListViewPrivate;
	public:
		ZKListItem();
		virtual ~ZKListItem();

		/**
		 * @brief Get subitem by ID
		 */
		ZKListSubItem* findSubItemByID(int id) const;
		int getSubItemCount() const { return mSubItemCount; }
		ZKListSubItem* getSubItem(int index) const
		{
			if (index >= 0 && index < mSubItemCount)
			{
				return mListSubItemList[index];
			}
			return nullptr;
		}

	protected:
		virtual void onBeforeCreateWindow(const Json::Value &json);

		int hitItemID(const MotionEvent &ev);
		void drawItem(ZKCanvas *pCanvas, int xOffset, int yOffset);
		void setItemPressed(int itemID, bool isPressed);

	private:
		void parseListItemAttributeFromJson(const Json::Value &json);

	private:
		ZKListSubItem **mListSubItemList;
		int mSubItemCount;
	};

	/**
	 * @brief Data and UI binding adapter
	 */
	class AbsListAdapter {
		friend class ZKListView;
	public:
		virtual ~AbsListAdapter() { }
		virtual int getListItemCount(const ZKListView *pListView) const = 0;
		virtual void obtainListItemData(ZKListView *pListView, ZKListItem *pListItem, int index) = 0;

		/**
		 * Update data and redraw UI
		 */
		void notifyDataSetChanged(ZKListView *pListView) {
			if (pListView) {
				pListView->refreshListView();
			}
		}
	};

	/**
	 * @brief List item click listener interface
	 */
	class IItemClickListener {
	public:
		virtual ~IItemClickListener() { }
		virtual void onItemClick(ZKListView *pListView, int index, int itemID) = 0;
	};

	/**
	 * @brief List item long click listener interface
	 */
	class IItemLongClickListener {
	public:
		virtual ~IItemLongClickListener() { }
		virtual void onItemLongClick(ZKListView *pListView, int index, int itemID) = 0;
	};

	void setListAdapter(AbsListAdapter *pAdapter);
	void setItemClickListener(IItemClickListener *pListener);
	void setItemLongClickListener(IItemLongClickListener *pListener);

	/**
	 * @brief Refresh the list
	 */
	void refreshListView();

	/**
	 * @brief Jump to the specified row or column
	 */
	void setSelection(int index);

	/**
	 * @brief Get the item width
	 */
	uint32_t getItemWidth() const;

	/**
	 * @brief Get the item height
	 */
	uint32_t getItemHeight() const;

	/**
	 * @brief Get the number of rows in the list
	 */
	uint32_t getRows() const;

	/**
	 * @brief Get the number of columns in the list
	 */
	uint32_t getCols() const;

	/**
	 * @brief Get the total number of list items
	 */
	int getListItemCount() const;

	/**
	 * @brief Get the index value of the first visible item
	 */
	int getFirstVisibleItemIndex() const;

	/**
	 * @brief Get the offset value of the first visible item
	 */
	int getFirstVisibleItemOffset() const;

	/**
	 * @brief Set the deceleration ratio for inertial scrolling
	 * @param ratio Deceleration ratio Range: 0 < ratio < 1
	 */
	void setDecRatio(float ratio);

protected:
	ZKListView(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_LISTVIEW; }

	virtual void onDraw(ZKCanvas *pCanvas);
	virtual bool onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

private:
	void parseListViewAttributeFromJson(const Json::Value &json);
};

#endif /* _CONTROL_ZKLISTVIEW_H_ */
