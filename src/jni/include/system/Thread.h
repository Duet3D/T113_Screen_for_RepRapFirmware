/*
 * Thread.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Aug 24, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _SYSTEM_THREAD_H_
#define _SYSTEM_THREAD_H_

#include "Condition.h"
#include "Mutex.h"

/**
 * @brief Thread class
 */
class Thread
{
  public:
	Thread();
	virtual ~Thread();

	/**
	 * @brief Start the thread
	 * @param name Thread name; default is NULL, automatically assigned by the system
	 */
	bool run(const char* name = 0);

	/**
	 * @brief Request to exit the thread
	 * @attention The function returns immediately after calling, but it does not mean that the thread has also exited
	 */
	void requestExit();

	/**
	 * @brief Request and wait for the thread to exit
	 * @attention The function returns only when the thread exits
	 */
	void requestExitAndWait();

	/**
	 * @brief Whether the thread is running
	 */
	bool isRunning() const;

	static void sleep(int msec);

  protected:
	/**
	 * @brief Whether there is a request to exit the thread
	 */
	bool exitPending() const;

	/**
	 * @brief Callback interface when the thread starts running
	 */
	virtual bool readyToRun();

	/**
	 * @brief The thread calls this interface in a loop
	 * @return true The thread will not exit, false The thread will exit
	 */
	virtual bool threadLoop() = 0;

  private:
	Thread& operator=(const Thread&);
	static void* _threadLoop(void* user);

  private:
	typedef struct
	{
		void* userData;
		char* threadName;
	} SThreadData;

	bool mExitPending;
	bool mIsRunning;

	Condition mThreadExitedCondition;
	mutable Mutex mLock;
};

#endif /* _SYSTEM_THREAD_H_ */