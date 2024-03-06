/*
 * Reset.cpp
 *
 * Created: 19/02/2023
 * Author: Loïc G.
 *
 */

#include "Reset.h"

#include <unistd.h>
#include <sys/reboot.h>

#ifdef __cplusplus
extern "C" {
#endif

void Reset() noexcept {
  // Synchronize data and save cached data to prevent data loss
  sync();
  reboot(RB_AUTOBOOT);
}

void EraseAndReset() noexcept {  // TODO: See if "erase" can be implemented ?
  // Synchronize data and save cached data to prevent data loss
  sync();
  reboot(RB_AUTOBOOT);
}

#ifdef __cplusplus
}
#endif
