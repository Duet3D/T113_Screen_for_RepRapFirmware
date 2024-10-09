/*
 * SpiHelper.h
 *
 *  Created on: May 3, 2019
 *      Author: ZKSWE Develop Team
 */

#ifndef _UTILS_SPI_HELPER_H_
#define _UTILS_SPI_HELPER_H_

#include <stdint.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

struct spi_dev;

/**
 * @brief SPI operation helper class
 */
class SpiHelper {
public:
  /**
   * @brief Constructor
   * @param nr SPI bus number
   * @param mode Mode, possible values: SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3
   * @param speed SPI clock frequency
   * @param bits Number of bits per word, default value: 8
   * @param isLSB Bit order, true: LSB first, false: MSB first; default value: false, MSB first
   */
  SpiHelper(int nr, uint8_t mode, uint32_t speed, uint8_t bits = 8, bool isLSB = false);
  virtual ~SpiHelper();

  /**
   * @brief Set SPI mode
   * @param mode Mode, possible values: SPI_MODE_0, SPI_MODE_1, SPI_MODE_2, SPI_MODE_3
   * @return true if successful, false otherwise
   */
  bool setMode(uint8_t mode);

  /**
   * @brief Set SPI clock frequency
   * @param speed Clock frequency
   * @return true if successful, false otherwise
   */
  bool setSpeed(uint32_t speed);

  /**
   * @brief Set number of bits per word
   * @param bits Number of bits
   * @return true if successful, false otherwise
   */
  bool setBitsPerWord(uint8_t bits);

  /**
   * @brief Set bit order
   * @param isLSB true: LSB first, false: MSB first
   * @return true if successful, false otherwise
   */
  bool setBitSeq(bool isLSB);

  /**
   * @brief Full duplex transfer
   * @param tx Write data address
   * @param rx Read data address
   * @param len Data length, note: read and write data lengths must be the same
   * @return true if successful, false otherwise
   */
  bool fullduplexTransfer(const uint8_t* tx, uint8_t* rx, uint32_t len);

  /**
   * @brief Half duplex transfer
   * @param tx Write data address
   * @param txLen Write data length
   * @param rx Read data address
   * @param rxLen Read data length
   * @return true if successful, false otherwise
   */
  bool halfduplexTransfer(const uint8_t* tx, uint32_t txLen, uint8_t* rx, uint32_t rxLen);

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
	struct spi_dev *mSpiDevPtr;
};

#endif /* _UTILS_SPI_HELPER_H_ */
