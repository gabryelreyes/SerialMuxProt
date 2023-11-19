/* MIT License
 *
 * Copyright (c) 2023 Gabryel Reyes <gabryelrdiaz@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*******************************************************************************
    DESCRIPTION
*******************************************************************************/
/**
 * @brief  Channel structure definition for the SerialMuxProt.
 * @author Gabryel Reyes <gabryelrdiaz@gmail.com>
 */
#ifndef SERIAL_MUX_CHANNELS_H
#define SERIAL_MUX_CHANNELS_H

/******************************************************************************
 * Compile Switches
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/

#include <Arduino.h>

/******************************************************************************
 * Macros
 *****************************************************************************/

/** Maximum number of SerialMuxProt Channels. */
#define MAX_CHANNELS (2U)

/** Name of Channel to send Timestamp to. */
#define TIMESTAMP_CHANNEL_NAME "TIMESTAMP"

/** DLC of Timestamp Channel */
#define TIMESTAMP_CHANNEL_DLC (sizeof(Timestamp))

/** Name of Channel to send Counter to. */
#define COUNTER_CHANNEL_NAME "COUNTER"

/** DLC of Counter Channel */
#define COUNTER_CHANNEL_DLC (sizeof(Counter))

/******************************************************************************
 * Types and Classes
 *****************************************************************************/

/** Struct of the "Timestamp" channel payload. */
typedef struct _Timestamp
{
    uint32_t timestamp; /**< Timestamp [ms]. */
} __attribute__((packed)) Timestamp;

/** Struct of the "Counter" channel payload. */
typedef struct _Counter
{
    uint32_t count; /**< Count [digits]. */
} __attribute__((packed)) Counter;

/******************************************************************************
 * Functions
 *****************************************************************************/

#endif /* SERIAL_MUX_CHANNELS_H */
