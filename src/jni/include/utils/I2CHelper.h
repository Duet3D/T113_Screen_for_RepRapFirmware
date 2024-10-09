/*
 * I2CHelper.h
 *
 *  Created on: May 3, 2019
 *      Author: ZKSWE Develop Team
 */

#ifndef _UTILS_I2C_HELPER_H_
#define _UTILS_I2C_HELPER_H_

#include <stdint.h>
#include <linux/types.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

struct i2c_dev;

/**
 * @brief I2C operation helper class
 */
class I2CHelper {
public:
  /**
   * @brief Constructor
   * @param nr I2C bus number
   * @param slaveAddr Slave address
   * @param timeout Timeout, in milliseconds
   * @param retries Retry count
   */
  I2CHelper(int nr, uint32_t slaveAddr, uint32_t timeout, uint32_t retries);
  virtual ~I2CHelper();

  /**
   * @brief Set the slave address
   * @param slaveAddr Slave address
   * @return true if successful, false otherwise
   */
  bool setSlaveAddr(uint32_t slaveAddr);

  /**
   * @brief Set the communication timeout
   * @param timeout Timeout, in milliseconds
   * @return true if successful, false otherwise
   */
  bool setTimeout(uint32_t timeout);

  /**
   * @brief Set the communication retry count
   * @param retries Retry count
   * @return true if successful, false otherwise
   */
  bool setRetries(uint32_t retries);

  /**
   * @brief Half-duplex transfer, i.e., shared read/write without stop signal in between
   * @param tx Write data address
   * @param txLen Write data length
   * @param rx Read data address
   * @param rxLen Read data length
   * @return true if successful, false otherwise
   */
  bool transfer(const uint8_t* tx, uint32_t txLen, uint8_t* rx, uint32_t rxLen);

  /**
   * @brief Simplex read
   * @param rx Read data address
   * @param len Data length
   * @return true if successful, false otherwise
   */
  bool read(uint8_t* rx, uint32_t len);

  /**
   * @brief Simplex write
   * @param tx Write data address
   * @param len Data length
   * @return true if successful, false otherwise
   */
  bool write(const uint8_t* tx, uint32_t len);

private:
	struct i2c_dev *mI2CDevPtr;
};

#endif /* _UTILS_I2C_HELPER_H_ */
