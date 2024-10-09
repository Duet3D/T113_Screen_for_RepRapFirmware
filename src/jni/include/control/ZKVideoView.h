/*
 * ZKVideoView.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Nov 13, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _CONTROL_ZKVIDEOVIEW_H_
#define _CONTROL_ZKVIDEOVIEW_H_

#include "ZKBase.h"
#include "media/ZKMediaPlayer.h"

class ZKVideoViewPrivate;

/**
 * @brief Video control
 */
class ZKVideoView : public ZKBase {
	ZK_DECLARE_PRIVATE(ZKVideoView)

public:
	ZKVideoView(ZKBase *pParent);
	virtual ~ZKVideoView();

	/**
	 * @brief Play the specified video file
	 * @param pFilePath Video file path
	 * @param msec Specify the starting position to play from, in milliseconds. Default is to start from the beginning.
	 */
	void play(const char *pFilePath, int msec = 0);

	/**
	 * @brief Pause playback
	 */
	void pause();

	/**
	 * @brief Resume playback
	 */
	void resume();

	/**
	 * @brief Seek to the specified position to play
	 * @param msec Position in milliseconds
	 */
	void seekTo(int msec);

	/**
	 * @brief Stop playback
	 */
	void stop();

	/**
	 * @brief Check if playing
	 */
	bool isPlaying();

	/**
	 * @brief Set the volume
	 * @param volume Range: 0.0 ~ 1.0
	 */
	void setVolume(float volume);

	void setPosition(const LayoutPosition &position);

	/* clockwise rotation: val=0 no rotation, val=1 90 degrees; val=2 180 degrees, val=3 270 degrees */
	void setRotation(int val);

	/**
	 * @brief Get the total duration of the video in milliseconds
	 */
	int getDuration();

	/**
	 * @brief Get the current playback position in milliseconds
	 */
	int getCurrentPosition();

public:
	typedef enum {
		E_MSGTYPE_VIDEO_PLAY_STARTED,
		E_MSGTYPE_VIDEO_PLAY_COMPLETED,
		E_MSGTYPE_VIDEO_PLAY_ERROR
	} EMessageType;

	/**
	 * @brief Video player message listener interface
	 */
	class IVideoPlayerMessageListener {
	public:
		virtual ~IVideoPlayerMessageListener() { }
		virtual void onVideoPlayerMessage(ZKVideoView *pVideoView, int msg) = 0;
	};

	void setVideoPlayerMessageListener(IVideoPlayerMessageListener *pListener) {
		mVideoPlayerMessageListenerPtr = pListener;
	}

protected:
	ZKVideoView(ZKBase *pParent, ZKBasePrivate *pBP);

	virtual void onBeforeCreateWindow(const Json::Value &json);
	virtual void onAfterCreateWindow(const Json::Value &json);
	virtual const char* getClassName() const { return ZK_VIDEOVIEW; }

	virtual void onDraw(ZKCanvas *pCanvas);

private:
	void parseVideoViewAttributeFromJson(const Json::Value &json);

	class PlayerMessageListener : public ZKMediaPlayer::IPlayerMessageListener {
	public:
		PlayerMessageListener(ZKVideoView *pVideoView) : mVideoViewPtr(pVideoView) { }
		virtual void onPlayerMessage(ZKMediaPlayer *pMediaPlayer, int msg, void *pMsgData);

	private:
		ZKVideoView * const mVideoViewPtr;
	};

private:
	ZKMediaPlayer *mMediaPlayerPtr;
	IVideoPlayerMessageListener *mVideoPlayerMessageListenerPtr;

	PlayerMessageListener mPlayerMessageListener;
};

#endif /* _CONTROL_ZKVIDEOVIEW_H_ */
