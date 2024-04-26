/*
 * ZKMediaPlayer.h - Zkswe
 *
 * Copyright (C) 2018 Zkswe Technology Corp.
 *
 *  Created on: Mar 6, 2018
 *      Author: zkswe@zkswe.com
 */

#ifndef _MEDIA_ZKMEDIAPLAYER_H_
#define _MEDIA_ZKMEDIAPLAYER_H_

#include "system/Thread.h"
#include "utils/MessageQueue.h"
#include "control/Common.h"

class InternalMediaPlayer;

/**
 * @brief Media player
 */
class ZKMediaPlayer {
	friend class InternalMediaPlayer;
public:
  /**
   * @brief Media type
   */
  typedef enum
  {
	  E_MEDIA_TYPE_AUDIO, /**< Audio type */
	  E_MEDIA_TYPE_VIDEO  /**< Video type */
	} EMediaType;

	typedef enum {
		E_MSGTYPE_PLAY_FILE = 1,
		E_MSGTYPE_PAUSE_FILE,
		E_MSGTYPE_RESUME_FILE,
		E_MSGTYPE_SEEK_FILE,
		E_MSGTYPE_STOP_FILE,
		E_MSGTYPE_PLAY_STARTED,
		E_MSGTYPE_PLAY_COMPLETED,
		E_MSGTYPE_ERROR_MEDIA_ERROR,  	// MEDIA_ERROR_UNKNOWN, ...
		E_MSGTYPE_ERROR_UNKNOWN,
		E_MSGTYPE_ERROR_INVALID_FILEPATH,
		E_MSGTYPE_ERROR_PREPARE_FAIL,
		E_MSGTYPE_ERROR_PLAY_FAIL,
		E_MSGTYPE_QUIT_THREAD,
	} EMessageType;

	typedef struct {			// for MSGTYPE_ERROR_MEDIA_ERROR
		int mediaErrorType;		// enum media_error_type
		int extra;
	} SMediaErrorInfo;

public:
  /**
   * @param mediaType Media type
   */
  ZKMediaPlayer(EMediaType mediaType);
  virtual ~ZKMediaPlayer();

  /**
   * @brief Play media file at the specified path
   * @param pFilePath Media file path
   * @param msec Specify the starting position in milliseconds, default is from the beginning
   */
  void play(const char* pFilePath, int msec = 0);

  /**
   * @brief Pause playback
   */
  void pause();

  /**
   * @brief Resume playback
   */
  void resume();

  /**
   * @brief Seek to the specified position in msec
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
   * @brief Set volume
   * @param volume Volume range: 0.0 to 1.0
   */
  void setVolume(float volume);

  void setPreviewPos(const LayoutPosition& pos);

  /* clockwise rotation: val=0 no rotation, val=1 90 degrees; val=2 180 degrees, val=3 270 degrees */
  void setRotation(int val);

  /**
   * @brief Get total duration in milliseconds
   */
  int getDuration();

  /**
   * @brief Get current playback position in milliseconds
   */
  int getCurrentPosition();

  class IPlayerMessageListener
  {
	public:
		virtual ~IPlayerMessageListener() { }
		virtual void onPlayerMessage(ZKMediaPlayer *pMediaPlayer, int msg, void *pMsgData) = 0;
	};

	void setPlayerMessageListener(IPlayerMessageListener *pListener) {
		mPlayerMessageListenerPtr = pListener;
	}

private:
	bool playerThreadLoop();

	enum EPlayState {
		E_PLAY_STATE_ERROR = -1,
		E_PLAY_STATE_IDLE = 0,
		E_PLAY_STATE_INITIALIZED = 1,
		E_PLAY_STATE_STARTED = 4,
		E_PLAY_STATE_PAUSED = 5,
	};

	class PlayerThread : public Thread {
	public:
		PlayerThread(ZKMediaPlayer &mp) : mPlayer(mp) { }

		void start();
		void stop();
		virtual bool threadLoop();

	private:
		ZKMediaPlayer &mPlayer;
	};

private:
	PlayerThread mPlayerThread;

	EMediaType mMediaType;
	InternalMediaPlayer *mMediaPlayerPtr;

	MessageQueue mMsgQueue;

	mutable Mutex mLock;
	Condition mExecuteDoneCondition;

	EPlayState mCurrentPlayState;
	IPlayerMessageListener *mPlayerMessageListenerPtr;
};

#endif  /* _MEDIA_ZKMEDIAPLAYER_H_ */
