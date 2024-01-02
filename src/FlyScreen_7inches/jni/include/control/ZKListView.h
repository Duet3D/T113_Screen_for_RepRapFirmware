/*
 * ZKListView.h
 *
 *  Created on: Jun 28, 2017
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKLISTVIEW_H_
#define _CONTROL_ZKLISTVIEW_H_

#include "ZKButton.h"
#include "utils/VelocityTracker.h"

class ZKBasePrivate;
class ZKListViewPrivate;

/**
 * @brief List control
 */
class ZKListView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKListView)

public:
	ZKListView(HWND hParentWnd);
	virtual ~ZKListView();

public:
	class ZKListItem;
	class ZKListSubItem : public ZKButton {
		friend class ZKListView;
		friend class ZKListItem;
		friend class ZKBasePrivate;
	public:
		ZKListSubItem();
		virtual ~ZKListSubItem();

	protected:
		virtual BOOL createWindow() { return FALSE; }
		virtual const char* getClassName() const { return NULL; }

		void drawSubItem(HDC hdc, int xOffset, int yOffset);

		int getLongClickTimeOut() const;
		int getLongClickIntervalTime() const;
	};

	class ZKListItem : public ZKListSubItem {
		friend class ZKListView;
	public:
		ZKListItem();
		virtual ~ZKListItem();

		/**
		 * @brief Get sub-item by ID
		 */
		ZKListSubItem* findSubItemByID(int id) const;
		int getSubItemCount() const { return mSubItemCount; }

	protected:
		virtual void onBeforeCreateWindow(const Json::Value &json);

		int hitItemID(const MotionEvent &ev);
		void drawItem(HDC hdc, int xOffset, int yOffset);
		void setItemPressed(int itemID, BOOL isPressed);

	private:
		void parseListItemAttributeFromJson(const Json::Value &json);

	private:
		const static int S_MAX_SUB_ITEM_COUNT = 5;
		ZKListSubItem *mListSubItems[S_MAX_SUB_ITEM_COUNT];
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
		 * Data update, redraw UI
		 */
		void notifyDataSetChanged(ZKListView *pListView) {
			if (pListView) {
				pListView->invalidate();
			}
		}
	};

	void setListAdapter(AbsListAdapter *pAdapter) {
		mAdapterPtr = pAdapter;
	}

	/**
	 * @brief List item click listener interface
	 */
	class IItemClickListener {
	public:
		virtual ~IItemClickListener() { }
		virtual void onItemClick(ZKListView *pListView, int index, int itemID) = 0;
	};

	/**
	 * @brief List item long press listener interface
	 */
	class IItemLongClickListener {
	public:
		virtual ~IItemLongClickListener() { }
		virtual void onItemLongClick(ZKListView *pListView, int index, int itemID) = 0;
	};

	void setItemClickListener(IItemClickListener *pListener) { mItemClickListenerPtr = pListener; }
	void setItemLongClickListener(IItemLongClickListener *pListener) { mItemLongClickListenerPtr = pListener; }

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
	UINT getItemWidth() const;

	/**
	 * @brief Get the item height
	 */
	UINT getItemHeight() const;

	/**
	 * @brief Get the number of rows in the list
	 */
	UINT getRows() const;

	/**
	 * @brief Get the number of columns in the list
	 */
	UINT getCols() const;

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

protected:
	ZKListView(HWND hParentWnd, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_LISTVIEW; }

	virtual void onDraw(HDC hdc);
	virtual BOOL onTouchEvent(const MotionEvent &ev);
	virtual void onTimer(int id);

	void _section_(zk) drawListView(HDC hdc);
	void _section_(zk) drawListItem(HDC hdc, int index, int xOffset, int yOffset);

private:
	void _section_(zk) parseListViewAttributeFromJson(const Json::Value &json);
	void getFirstVisibleItemOffset(int &xOffset, int &yOffset) const;
	int getEndEdgeOffset() const;

	int getRowsOrCols() const;

	void resetOffset();
	void resetItemPressInfo();

	// Start rollback
	void startRollback();

	void startSinkScrollbar();

	// Stop scrolling
	void stopRoll();

	// Cycle list Adjust the offset value of the first item
	void cycleFirstItemOffset();

	/**
	 * Record list item press information
	 */
	typedef struct {
		BOOL isPressed;
		int index;
		int itemID;
	} SItemPressInfo;

	typedef enum {
		E_EDGE_EFFECT_NONE,
		E_EDGE_EFFECT_DRAG,			// Drag effect
		E_EDGE_EFFECT_FADING		// Shadow gradually fades effect
	} EEdgeEffect;

	BOOL isVerticalOrientation() const { return mOrientation == E_ORIENTATION_VERTICAL; }
	BOOL isDragEffect() const { return mEdgeEffect == E_EDGE_EFFECT_DRAG; }
	BOOL isFadingEffect() const { return mEdgeEffect == E_EDGE_EFFECT_FADING; }

private:
	UINT mRows;
	UINT mCols;

	int mFirstItemXOffset;
	int mFirstItemYOffset;

	int mFirstItemXOffsetOfDown;	// Record the offset position of the first item when pressed
	int mFirstItemYOffsetOfDown;

	UINT mItemWidth;
	UINT mItemHeight;

	BOOL mIsDamping;
	BOOL mIsRolling;

	BOOL mIsAutoRollback;		// Auto rollback alignment flag
	BOOL mIsCycleEnable;		// Circular list

	VelocityTracker mVelocityTracker;
	float mCurVelocity;

	EOrientation mOrientation;	// Direction: 0 Horizontal, 1 Vertical
	EEdgeEffect mEdgeEffect;

	UINT mDragMaxDistance;

	IItemClickListener *mItemClickListenerPtr;
	IItemLongClickListener *mItemLongClickListenerPtr;

	SItemPressInfo mItemPressInfo;

	ZKListItem *mListItemPtr;
	AbsListAdapter *mAdapterPtr;

	HGRAPHICS mBackgroundGraphics;
};

#endif /* _CONTROL_ZKLISTVIEW_H_ */
