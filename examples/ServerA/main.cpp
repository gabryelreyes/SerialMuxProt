/* MIT License
 *
 * Copyright (c) 2023 - 2024 Gabryel Reyes <gabryelrdiaz@gmail.com>
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
 * @brief  SerialMuxProt Example
 * @author Gabryel Reyes <gabryelrdiaz@gmail.com>
 */

/******************************************************************************
 * Includes
 *****************************************************************************/

#include <Arduino.h>
#include <SerialMuxProtServer.hpp>
#include "SerialMuxChannels.h"

/******************************************************************************
 * Compiler Switches
 *****************************************************************************/

#ifndef LED_BUILTIN
#define LED_BUILTIN (13U)
#endif /* LED_BUILTIN */

/******************************************************************************
 * Macros
 *****************************************************************************/

/******************************************************************************
 * Types and classes
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

/******************************************************************************
 * Local Variables
 *****************************************************************************/

/** Serial interface baudrate. */
static const uint32_t SERIAL_BAUDRATE = 115200U;

/** Sending LED Data Period. */
static const uint32_t LED_SEND_PERIOD = 1000U;

/**
 * SerialMuxProt Server Instance.
 * @note Serial is given as parameter to the constructor. This means, it cannot be used for other purposes.
 *
 * @tparam tMaxChannels set to MAX_CHANNELS, defined in SerialMuxChannels.h.
 */
SerialMuxProtServer<MAX_CHANNELS> gSmpServer(Serial);

/** SerialMuxProt Channel id for sending LED data. */
uint8_t gSerialMuxProtChannelIdLedData = 0U;

/** Last timestamp of sent data. */
uint32_t gLastLedSendTimestamp = 0U;

/******************************************************************************
 * Public Methods
 *****************************************************************************/

/**
 * Setup the application.
 */
void setup()
{
    /* Initialize Serial */
    Serial.begin(SERIAL_BAUDRATE);

    /* Initialize LED. */
    pinMode(LED_BUILTIN, OUTPUT);

    /* Create Channel for sending LED data. */
    gSerialMuxProtChannelIdLedData = gSmpServer.createChannel(LED_CHANNEL_NAME, LED_CHANNEL_DLC);
}

/**
 * Process the application periodically.
 */
void loop()
{
    /* Process SerialMuxProt. */
    gSmpServer.process(millis());

    /* Send LED data periodically. */
    if ((millis() - gLastLedSendTimestamp) > LED_SEND_PERIOD)
    {
        /* Read LED state. */
        int currentLedState = digitalRead(LED_BUILTIN);

        /* Create LED data payload. */
        LedData payload = {.state = !currentLedState};

        /* Send LED data. */
        if (true == gSmpServer.sendData(gSerialMuxProtChannelIdLedData, &payload, sizeof(LedData)))
        {
            /* Toggle LED. */
            digitalWrite(LED_BUILTIN, !currentLedState);

            /* Update timestamp. */
            gLastLedSendTimestamp = millis();
        }
    }
}

/******************************************************************************
 * Protected Methods
 *****************************************************************************/

/******************************************************************************
 * Private Methods
 *****************************************************************************/

/******************************************************************************
 * External Functions
 *****************************************************************************/

/******************************************************************************
 * Local Functions
 *****************************************************************************/
