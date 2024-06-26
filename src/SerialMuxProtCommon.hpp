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
 * @brief  Common Constants and Structures of SerialMuxProt.
 * @author Gabryel Reyes <gabryelrdiaz@gmail.com>
 *
 * @{
 */

#ifndef SERIALMUXPROT_COMMON_H_
#define SERIALMUXPROT_COMMON_H_

/******************************************************************************
 * Includes
 *****************************************************************************/

#include <stdint.h>

/******************************************************************************
 * Macros
 *****************************************************************************/

/** Channel Field Length in Bytes */
#define CHANNEL_LEN (1U)

/** DLC Field Length in Bytes */
#define DLC_LEN (1U)

/** Checksum Field Length in Bytes */
#define CHECKSUM_LEN (1U)

/** Length of Complete Header Field */
#define HEADER_LEN (CHANNEL_LEN + DLC_LEN + CHECKSUM_LEN)

/** Data Field Length in Bytes */
#define MAX_DATA_LEN (32U)

/** Total Frame Length in Bytes */
#define MAX_FRAME_LEN (HEADER_LEN + MAX_DATA_LEN)

/** Max length of channel name */
#define CHANNEL_NAME_MAX_LEN (10U)

/** Number of Control Channel. */
#define CONTROL_CHANNEL_NUMBER (0U)

/** DLC of Control Channel Payload. */
#define CONTROL_CHANNEL_PAYLOAD_LENGTH (sizeof(ControlChannelPayload))

/** Period of Heartbeat when Synced. */
#define HEATBEAT_PERIOD_SYNCED (5000U)

/** Period of Heartbeat when Unsynced */
#define HEATBEAT_PERIOD_UNSYNCED (1000U)

/** Max number of attempts at receiving a Frame before resetting RX Buffer */
#define MAX_RX_ATTEMPTS (MAX_FRAME_LEN)

/******************************************************************************
 * Types and Classes
 *****************************************************************************/

/**
 * Channel Notification Prototype Callback.
 * Provides the received data in the respective channel to the application.
 *
 * @param[in] payload       Received data.
 * @param[in] payloadSize   Size of the received data.
 * @param[in] userData      User data provided by the application.
 */
typedef void (*ChannelCallback)(const uint8_t* payload, uint8_t payloadSize, void* userData);

/**
 * Event Notification Prototype Callback.
 * Provides a notification to the application on the event it is registered to.
 *
 * @param[in] userData  User data provided by the application.
 */
typedef void (*EventCallback)(void* userData);

/**
 * Channel Definition.
 */
struct Channel
{
    char            m_name[CHANNEL_NAME_MAX_LEN]; /**< Name of the channel. */
    uint8_t         m_dlc;                        /**< Payload length of channel */
    ChannelCallback m_callback;                   /**< Callback to provide received data to the application. */

    /**
     * Channel Constructor.
     */
    Channel() : m_name{0U}, m_dlc(0U), m_callback(nullptr)
    {
    }
};

/** Data container of the Frame Fields */
typedef union _Frame
{
    /** Frame Fields */
    struct _Fields
    {
        /** Header */
        union _Header
        {
            /** Header Fields Struct */
            struct _HeaderFields
            {
                /** Channel ID */
                uint8_t m_channel;

                /** Channel ID */
                uint8_t m_dlc;

                /** Frame Checksum */
                uint8_t m_checksum;

            } __attribute__((packed)) headerFields; /**< Header Fields */

            /** Raw Header Data*/
            uint8_t rawHeader[HEADER_LEN];

        } __attribute__((packed)) header; /**< Header */

        /** Payload */
        struct _Payload
        {
            /** Data of the Frame */
            uint8_t m_data[MAX_DATA_LEN];

        } __attribute__((packed)) payload; /**< Payload */

    } __attribute__((packed)) fields; /**< Frame Fields */

    /** Raw Frame Data */
    uint8_t raw[MAX_FRAME_LEN] = {0U};

} __attribute__((packed)) Frame; /**< Frame */

/**
 * Enumeration of Commands of Control Channel.
 */
enum COMMANDS : uint8_t
{
    SYNC = 0x00, /**< SYNC Command */
    SYNC_RSP,    /**< SYNC Response */
    SCRB,        /**< Subscribe Command */
    SCRB_RSP,    /**< Subscribe Response */
};

/**
 * Control Channel Payload Structure.
 */
typedef struct _ControlChannelPayload
{
    uint8_t  commandByte                       = 0U;   /**< Command Byte */
    uint32_t timestamp                         = 0U;   /**< Timestamp */
    uint8_t  channelNumber                     = 0U;   /**< Channel Number */
    char     channelName[CHANNEL_NAME_MAX_LEN] = {0U}; /**< Channel Name */
} __attribute__((packed)) ControlChannelPayload;       /**< ControlChannelPayload */

#endif /* SERIALMUXPROT_COMMON_H_ */
/** @} */
