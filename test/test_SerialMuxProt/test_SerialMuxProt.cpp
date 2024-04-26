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
 * @author  Gabryel Reyes <gabryelrdiaz@gmail.com>
 * @brief   This module contains the SerialMuxProt Server tests.
 */

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <unity.h>
#include <TestStream.h>
#include <SerialMuxProtServer.hpp>
#include <stdio.h>

/******************************************************************************
 * Compiler Switches
 *****************************************************************************/

/******************************************************************************
 * Macros
 *****************************************************************************/

/******************************************************************************
 * Types and classes
 *****************************************************************************/

/******************************************************************************
 * Prototypes
 *****************************************************************************/

static void setup();
static void loop();
static void testChannelCallback(const uint8_t* payload, uint8_t payloadSize);
static void testCmdSync();
static void testCmdSyncRsp();
static void testCmdScrb();
static void testCmdScrbRsp();
static void testChannelCreation();
static void testDataSend();
static void testEventCallbacks();

/******************************************************************************
 * Local Variables
 *****************************************************************************/

static uint8_t       emptyOutputBuffer[MAX_FRAME_LEN];
static TestStream    gTestStream;
static const uint8_t controlChannelFrameLength = (HEADER_LEN + CONTROL_CHANNEL_PAYLOAD_LENGTH);
static const uint8_t testPayload[4U]           = {0x12, 0x34, 0x56, 0x78};
static bool          callbackCalled            = false;

/******************************************************************************
 * Public Methods
 *****************************************************************************/

/******************************************************************************
 * Protected Methods
 *****************************************************************************/

/******************************************************************************
 * Private Methods
 *****************************************************************************/

/******************************************************************************
 * External Functions
 *****************************************************************************/

/**
 * Tests main entry point.
 * @param[in] argc  Number of arguments
 * @param[in] argv  Arguments
 */
int main(int argc, char** argv)
{
    setup(); /* Prepare test */
    loop();  /* Run test once */

    return 0;
}

/**
 * Program setup routine, which is called once at startup.
 */
static void setup()
{
    memset(emptyOutputBuffer, 0xA5, sizeof(emptyOutputBuffer));
#ifndef TARGET_NATIVE
    /* https://docs.platformio.org/en/latest/plus/unit-testing.html#demo */
    delay(2000);
#endif /* Not defined TARGET_NATIVE */
}

/**
 * Main entry point.
 */
static void loop()
{
    UNITY_BEGIN();

    RUN_TEST(testCmdSync);
    RUN_TEST(testCmdSyncRsp);
    RUN_TEST(testCmdScrb);
    RUN_TEST(testCmdScrbRsp);
    RUN_TEST(testChannelCreation);
    RUN_TEST(testDataSend);
    RUN_TEST(testEventCallbacks);

    UNITY_END();

#ifndef TARGET_NATIVE
    /* Don't exit on the robot to avoid a endless test loop.
     * If the test runs on the pc, it must exit.
     */
    for (;;)
    {
    }
#endif /* Not defined TARGET_NATIVE */
}

/**
 * Initialize the test setup.
 */
extern void setUp(void)
{
    /* Not used. */
}

/**
 * Clean up test setup.
 */
extern void tearDown(void)
{
    /* Not used. */
}

/******************************************************************************
 * Local Functions
 *****************************************************************************/

/**
 * Callback for incoming data from test channel.
 * @param[in] payload Byte buffer containing incomming data.
 * @param[in] payloadSize Number of bytes received.
 * @param[in] userData      User data provided by the application.
 */
static void testChannelCallback(const uint8_t* payload, uint8_t payloadSize, void* userData)
{
    callbackCalled = true;
    TEST_ASSERT_EQUAL_UINT8_ARRAY(testPayload, payload, payloadSize);
}

/**
 * Test SYNC Command of SerialMuxProt Server.
 */
static void testCmdSync()
{
    SerialMuxProtServer<2U> testSerialMuxProtServer(gTestStream);
    uint8_t                 expectedOutputBufferVector[6U][MAX_FRAME_LEN] = {
        {0x00, 0x10, 0xFB, 0x00, 0xE8, 0x03, 0x00, 0x00}, /* SYNC 1000ms*/
        {0x00, 0x10, 0xE7, 0x00, 0xD0, 0x07, 0x00, 0x00}, /* SYNC 2000ms*/
        {0x00, 0x10, 0x83, 0x00, 0x58, 0x1B, 0x00, 0x00}, /* SYNC 7000ms*/
        {0x00, 0x10, 0x1F, 0x00, 0xE0, 0x2E, 0x00, 0x00}, /* SYNC 12000ms*/
        {0x00, 0x10, 0xBA, 0x00, 0x68, 0x42, 0x00, 0x00}  /* SYNC 17000ms*/
    };
    uint8_t inputQueueVector[2U][MAX_FRAME_LEN] = {{0x00, 0x10, 0xE8, 0x01, 0xD0, 0x07, 0x00, 0x00},
                                                   {0x00, 0x10, 0x84, 0x01, 0x58, 0x1B, 0x00, 0x00}};

    /*
     * Case: Unsynced Heartbeat.
     */

    gTestStream.flushOutputBuffer();

    /* Unsynced Heartbeat at 0 milliseconds */
    testSerialMuxProtServer.process(0U);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, controlChannelFrameLength);
    gTestStream.flushOutputBuffer();

    /* Unsynced Heartbeat after 1000 milliseconds */
    testSerialMuxProtServer.process(1000U);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[0U], gTestStream.m_outputBuffer,
                                  controlChannelFrameLength);
    gTestStream.flushOutputBuffer();

    /* No Heartbeat expected */
    testSerialMuxProtServer.process(1500U);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, controlChannelFrameLength);
    gTestStream.flushOutputBuffer();

    /* Unsynced Heartbeat after 2000 milliseconds */
    testSerialMuxProtServer.process(2000U);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[1U], gTestStream.m_outputBuffer,
                                  controlChannelFrameLength);
    gTestStream.flushOutputBuffer();

    /*
     * Case: Sync
     */

    /* Put Data in RX Queue */
    gTestStream.pushToQueue(inputQueueVector[0], controlChannelFrameLength);

    /* Two process calls required */
    testSerialMuxProtServer.process(2500U); /* Read Frame Header */
    testSerialMuxProtServer.process(2700U); /* Read Frame Payload */
    TEST_ASSERT_TRUE(testSerialMuxProtServer.isSynced());

    /* No output expected */
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, controlChannelFrameLength);
    gTestStream.flushInputBuffer();

    /*
     * Case: Synced Heartbeat.
     * Last Sync = 2000 ms
     * isSynced = true
     * Next Sync = 7000 ms
     */

    /* No output expected. Would be Heartbeat if Unsynced. */
    testSerialMuxProtServer.process(3000U);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, controlChannelFrameLength);

    /* Synced Heartbeat. */
    testSerialMuxProtServer.process(7000U);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[2U], gTestStream.m_outputBuffer,
                                  controlChannelFrameLength);
    gTestStream.flushOutputBuffer();

    /**
     * Case: Maintain Sync
     */

    /* Put SYNC_RSP in RX Queue. Otherwise will fall out of Sync. */
    gTestStream.pushToQueue(inputQueueVector[1], controlChannelFrameLength);

    /* Two process calls required */
    testSerialMuxProtServer.process(9000U);  /* Read Frame Header */
    testSerialMuxProtServer.process(11000U); /* Read Frame Payload */
    TEST_ASSERT_TRUE(testSerialMuxProtServer.isSynced());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, controlChannelFrameLength);

    /* Synced Heartbeat */
    testSerialMuxProtServer.process(12000U);
    TEST_ASSERT_TRUE(testSerialMuxProtServer.isSynced());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[3U], gTestStream.m_outputBuffer,
                                  controlChannelFrameLength);
    gTestStream.flushInputBuffer();
    gTestStream.flushOutputBuffer();

    /**
     * Case: Fall out of Sync.
     * No data passed to RX Queue.
     */

    /* Synced Heartbeat. Fall out fo sync, as last Heartbeat was not Acknowledged. */
    testSerialMuxProtServer.process(17000U);
    TEST_ASSERT_FALSE(testSerialMuxProtServer.isSynced());
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[4U], gTestStream.m_outputBuffer,
                                  controlChannelFrameLength);
    gTestStream.flushOutputBuffer();
}

/**
 * Test SYNC_RSP Command of SerialMuxProt Server.
 */
static void testCmdSyncRsp()
{
    SerialMuxProtServer<2U> testSerialMuxProtServer(gTestStream);
    uint8_t                 testTime                                                 = 0U;
    uint8_t                 numberOfCases                                            = 3U;
    uint8_t                 expectedOutputBufferVector[numberOfCases][MAX_FRAME_LEN] = {
        {0x00, 0x10, 0x11, 0x01, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x10, 0x26, 0x01, 0x78, 0x56, 0x34, 0x12},
        {0x00, 0x10, 0x11, 0x01, 0xFF, 0xFF, 0xFF, 0xFF}};
    uint8_t inputQueueVector[numberOfCases][MAX_FRAME_LEN] = {{0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00},
                                                              {0x00, 0x10, 0x25, 0x00, 0x78, 0x56, 0x34, 0x12},
                                                              {0x00, 0x10, 0x10, 0x00, 0xFF, 0xFF, 0xFF, 0xFF}};

    /* Ignore SYNC */
    testSerialMuxProtServer.process(testTime++);
    gTestStream.flushOutputBuffer();

    /* Test for SYNC_RSP */
    for (uint8_t testCase = 0; testCase < numberOfCases; testCase++)
    {
        gTestStream.pushToQueue(inputQueueVector[testCase], controlChannelFrameLength);
        testSerialMuxProtServer.process(testTime++);
        testSerialMuxProtServer.process(testTime++);
        TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[testCase], gTestStream.m_outputBuffer,
                                      controlChannelFrameLength);
        gTestStream.flushInputBuffer();
        gTestStream.flushOutputBuffer();
    }
}

/**
 * Test SCRB Command of SerialMuxProt Server.
 */
static void testCmdScrb()
{
    SerialMuxProtServer<2U> testSerialMuxProtServer(gTestStream);
    uint8_t                 testTime                                                 = 0U;
    uint8_t                 numberOfCases                                            = 2U;
    uint8_t                 expectedOutputBufferVector[numberOfCases][MAX_FRAME_LEN] = {
        {0x00, 0x10, 0x54, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 'T', 'E', 'S', 'T'},
        {0x00, 0x10, 0x55, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 'T', 'E', 'S', 'T'}};
    uint8_t inputQueueVector[numberOfCases][MAX_FRAME_LEN] = {
        {0x00, 0x10, 0x53, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 'T', 'E', 'S', 'T', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

    /* Ignore SYNC */
    testSerialMuxProtServer.process(testTime++);
    gTestStream.flushOutputBuffer();

    /*
     * Case: Suscribe to Unknown Channel
     */
    gTestStream.pushToQueue(inputQueueVector[0], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[0], gTestStream.m_outputBuffer, controlChannelFrameLength);
    gTestStream.flushOutputBuffer();
    gTestStream.flushInputBuffer();

    /*
     * Case: Subscribe to Known Channel
     */
    TEST_ASSERT_EQUAL_UINT8(1U, testSerialMuxProtServer.createChannel("TEST", 8U));

    gTestStream.pushToQueue(inputQueueVector[0], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[1], gTestStream.m_outputBuffer, controlChannelFrameLength);
    gTestStream.flushOutputBuffer();
    gTestStream.flushInputBuffer();

    /*
     * Case: Subscribe to a Duplicate Channel
     */
    TEST_ASSERT_EQUAL_UINT8(2U, testSerialMuxProtServer.createChannel("TEST", 8U));

    gTestStream.pushToQueue(inputQueueVector[0], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[1], gTestStream.m_outputBuffer, controlChannelFrameLength);
    gTestStream.flushOutputBuffer();
    gTestStream.flushInputBuffer();
}

/**
 * Test SCRB_RSP Command of SerialMuxProt Server.
 */
static void testCmdScrbRsp()
{
    SerialMuxProtServer<2U> testSerialMuxProtServer(gTestStream);
    uint8_t                 testTime                                                 = 1U;
    uint8_t                 numberOfCases                                            = 3U;
    uint8_t                 expectedOutputBufferVector[numberOfCases][MAX_FRAME_LEN] = {
        {0x00, 0x10, 0x53, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 'T', 'E', 'S', 'T', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    uint8_t inputQueueVector[numberOfCases][MAX_FRAME_LEN] = {
        {0x00, 0x10, 0x11, 0x01, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x10, 0x54, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 'T', 'E', 'S', 'T'},
        {0x00, 0x10, 0x55, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 'T', 'E', 'S', 'T'}};

    /*
     * Case: Suscribe to Unknown Channel
     */
    testSerialMuxProtServer.subscribeToChannel("TEST", testChannelCallback);

    /* Sync */
    gTestStream.pushToQueue(inputQueueVector[0U], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_TRUE(testSerialMuxProtServer.isSynced());

    /* Subscription sent. */
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[0], gTestStream.m_outputBuffer, controlChannelFrameLength);

    /* Clear Subscription. */
    gTestStream.pushToQueue(inputQueueVector[1U], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_EQUAL_UINT8(0U, testSerialMuxProtServer.getNumberOfRxChannels());
    gTestStream.flushInputBuffer();
    gTestStream.flushOutputBuffer();

    /* De-Sync */
    testTime += 5000U;
    testSerialMuxProtServer.process(testTime++);

    /*
     * Case: Suscribe to Known Channel
     */
    testSerialMuxProtServer.subscribeToChannel("TEST", testChannelCallback);

    /* Sync */
    gTestStream.pushToQueue(inputQueueVector[0U], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_TRUE(testSerialMuxProtServer.isSynced());

    /* Subscription sent. */
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[0], gTestStream.m_outputBuffer, controlChannelFrameLength);

    /* Clear Subscription. */
    gTestStream.pushToQueue(inputQueueVector[2U], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_EQUAL_UINT8(1U, testSerialMuxProtServer.getNumberOfRxChannels());
    gTestStream.flushInputBuffer();
    gTestStream.flushOutputBuffer();

    /* De-Sync */
    testTime += 5000U;
    testSerialMuxProtServer.process(testTime++);

    /*
     * Case: Suscribe again to Known Channel
     */
    testSerialMuxProtServer.subscribeToChannel("TEST", testChannelCallback);

    /* Sync */
    gTestStream.pushToQueue(inputQueueVector[0U], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_TRUE(testSerialMuxProtServer.isSynced());

    /* Subscription sent. */
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[0], gTestStream.m_outputBuffer, controlChannelFrameLength);

    /* Clear Subscription. */
    gTestStream.pushToQueue(inputQueueVector[2U], controlChannelFrameLength);
    testSerialMuxProtServer.process(testTime++);
    testSerialMuxProtServer.process(testTime++);
    TEST_ASSERT_EQUAL_UINT8(1U, testSerialMuxProtServer.getNumberOfRxChannels());
    gTestStream.flushInputBuffer();
    gTestStream.flushOutputBuffer();
}

/**
 * Test Channel Creation on a SerialMuxProt Server.
 */
static void testChannelCreation()
{
    const uint8_t                    maxChannels = 5U;
    SerialMuxProtServer<maxChannels> testSerialMuxProtServer(gTestStream);

    /* No Channels Configured on Start */
    TEST_ASSERT_EQUAL_UINT8(0U, testSerialMuxProtServer.getNumberOfTxChannels());

    /*
     * Case: Try to configure invalid channels.
     */

    /* Channel Name is empty */
    TEST_ASSERT_EQUAL_UINT8(0U, testSerialMuxProtServer.createChannel("", 1U));
    TEST_ASSERT_EQUAL_UINT8(0U, testSerialMuxProtServer.getNumberOfTxChannels());

    /* DLC = 0U */
    TEST_ASSERT_EQUAL_UINT8(0U, testSerialMuxProtServer.createChannel("TEST", 0U));
    TEST_ASSERT_EQUAL_UINT8(0U, testSerialMuxProtServer.getNumberOfTxChannels());

    /*
     * Case: Configure maximum valid channels.
     */
    for (uint8_t channelNumber = 0; channelNumber < maxChannels; channelNumber++)
    {
        TEST_ASSERT_EQUAL_UINT8(channelNumber, testSerialMuxProtServer.getNumberOfTxChannels());
        TEST_ASSERT_EQUAL_UINT8((channelNumber + 1U), testSerialMuxProtServer.createChannel("TEST", 1U));
        TEST_ASSERT_EQUAL_UINT8((channelNumber + 1U), testSerialMuxProtServer.getNumberOfTxChannels());
    }

    /*
     * Case: Try to configure more than the maximum number of channels.
     */

    TEST_ASSERT_EQUAL_UINT8(0U, testSerialMuxProtServer.createChannel("TEST", 1U));
    TEST_ASSERT_EQUAL_UINT8(maxChannels, testSerialMuxProtServer.getNumberOfTxChannels());
}

/**
 * Test data send on SerialMuxProt Server.
 */
static void testDataSend()
{
    SerialMuxProtServer<1U> testSerialMuxProtServer(gTestStream);
    uint8_t expectedOutputBufferVector[1U][MAX_FRAME_LEN] = {{0x01, 0x04, 0x1A, 0x12, 0x34, 0x56, 0x78}};
    uint8_t inputQueueVector[1U][MAX_FRAME_LEN]           = {{0x00, 0x10, 0x11, 0x01, 0x00, 0x00, 0x00, 0x00}};

    /* Flush Stream */
    gTestStream.flushInputBuffer();
    gTestStream.flushOutputBuffer();

    /*
     * Case: Send data on Control Channel.
     */
    testSerialMuxProtServer.sendData((uint8_t)CONTROL_CHANNEL_NUMBER, testPayload, sizeof(testPayload));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, sizeof(testPayload));

    /*
     * Case: Send on non-existent channel while unsynced.
     */
    testSerialMuxProtServer.sendData("TEST", testPayload, sizeof(testPayload));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, sizeof(testPayload));

    /* Create a Channel */
    TEST_ASSERT_EQUAL_UINT8(1U, testSerialMuxProtServer.createChannel("TEST", sizeof(testPayload)));

    /*
     * Case: Send on existent channel while unsynced.
     */
    testSerialMuxProtServer.sendData("TEST", testPayload, sizeof(testPayload));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, sizeof(testPayload));

    /* Sync */
    gTestStream.pushToQueue(inputQueueVector[0U], controlChannelFrameLength);
    testSerialMuxProtServer.process(1U);
    testSerialMuxProtServer.process(2U);
    TEST_ASSERT_TRUE(testSerialMuxProtServer.isSynced());

    /*
     * Case: Send on non-existent channel.
     */
    testSerialMuxProtServer.sendData("HELLO", testPayload, sizeof(testPayload));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(emptyOutputBuffer, gTestStream.m_outputBuffer, sizeof(testPayload));

    /*
     * Case: Send on existent channel.
     */
    testSerialMuxProtServer.sendData("TEST", testPayload, sizeof(testPayload));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedOutputBufferVector[0U], gTestStream.m_outputBuffer, sizeof(testPayload));
}

/**
 * Test Event Callbacks on SerialMuxProt Server.
 */
static void testEventCallbacks()
{
    SerialMuxProtServer<1U> testSerialMuxProtServer(gTestStream);
    uint8_t expectedOutputBufferVector[1U][MAX_FRAME_LEN] = {{0x01, 0x04, 0x1A, 0x12, 0x34, 0x56, 0x78}};
    uint8_t inputQueueVector[1U][MAX_FRAME_LEN]           = {{0x00, 0x10, 0x11, 0x01, 0x00, 0x00, 0x00, 0x00}};

    /* Register callbacks. */
    callbackCalled = false;

    TEST_ASSERT_TRUE(testSerialMuxProtServer.registerOnSyncedCallback([](void* userData) { callbackCalled = true; }));
    TEST_ASSERT_TRUE(testSerialMuxProtServer.registerOnDeSyncedCallback([](void* userData) { callbackCalled = true; }));

    /* Flush Stream */
    gTestStream.flushInputBuffer();
    gTestStream.flushOutputBuffer();

    /* Sync */
    gTestStream.pushToQueue(inputQueueVector[0U], controlChannelFrameLength);
    testSerialMuxProtServer.process(1U);
    testSerialMuxProtServer.process(2U);
    TEST_ASSERT_TRUE(testSerialMuxProtServer.isSynced());
    TEST_ASSERT_TRUE(callbackCalled);

    /* De-sync.*/
    callbackCalled = false;
    testSerialMuxProtServer.process(2000U);
    testSerialMuxProtServer.process(7000U);
    testSerialMuxProtServer.process(12000U);
    TEST_ASSERT_FALSE(testSerialMuxProtServer.isSynced());
    TEST_ASSERT_TRUE(callbackCalled);
}