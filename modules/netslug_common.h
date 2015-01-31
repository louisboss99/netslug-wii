/* netslug_common.h
 *   by Alex Chadwick
 * 
 * Contains symbols and definitions shared by all netslug_* modules at run time.
 */
 
#ifndef NETSLUG_COMMON_H_
#define NETSLUG_COMMON_H_

#include <stdbool.h>

// Method called to indicate frame advance. Skipped is true if the game was not
// notified of this frame in order to account for lag.
typedef void (*frame_handler_t)(bool skipped);

// Handler to run to indicate frame advance. Any module may set this, but they
// MUST ensure that the existing handler is called.
extern frame_handler_t on_frame_advance;

// Variable to indicate if a network error has occurred on any thread. This
// should always be zero in normal operations.
extern int network_error;

#endif