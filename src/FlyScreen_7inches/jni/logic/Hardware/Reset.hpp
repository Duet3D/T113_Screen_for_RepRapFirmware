/*
 * Reset.cpp
 *
 * Created: 19/02/2023
 * Author: Loïc G.
 *
 */

#ifndef RESET_H_
#define RESET_H_

#ifdef __cplusplus
extern "C" {
#endif

// Restart the hardware
void Reset() noexcept __attribute__((noreturn));
void EraseAndReset() noexcept __attribute__((noreturn));

#ifdef __cplusplus
}
#endif

#endif /* RESET_H_ */
