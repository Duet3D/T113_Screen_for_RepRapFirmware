/*
 * StoragePreferences.h - Zkswe
 *
 * Copyright (C) 2017 Zkswe Technology Corp.
 *
 *  Created on: Nov 9, 2017
 *      Author: zkswe@zkswe.com
 */

#ifndef _STORAGE_STORAGEPREFERENCES_H_
#define _STORAGE_STORAGEPREFERENCES_H_

#include <string>

/**
 * @brief Data storage
 */
class StoragePreferences {
public:
  /**
   * @brief Store a string value
   */
  static bool putString(const std::string& key, const std::string& val);

  /**
   * @brief Store an integer value
   */
  static bool putInt(const std::string& key, int val);

  /**
   * @brief Store a boolean value
   */
  static bool putBool(const std::string& key, bool val);

  /**
   * @brief Store a floating-point value
   */
  static bool putFloat(const std::string& key, float val);

  /**
   * @brief Remove data corresponding to a key
   */
  static bool remove(const std::string& key);

  /**
   * @brief Clear stored data
   */
  static bool clear();

  /**
   * @brief Get the string value corresponding to a key, return defVal if not found
   */
  static std::string getString(const std::string& key, const std::string& defVal);

  /**
   * @brief Get the integer value corresponding to a key, return defVal if not found
   */
  static int getInt(const std::string& key, int defVal);

  /**
   * @brief Get the boolean value corresponding to a key, return defVal if not found
   */
  static bool getBool(const std::string& key, bool defVal);

  /**
   * @brief Get the floating-point value corresponding to a key, return defVal if not found
   */
  static float getFloat(const std::string& key, float defVal);
};

#endif /* _STORAGE_STORAGEPREFERENCES_H_ */
