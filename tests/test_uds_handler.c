#include "test_framework.h"
#include "../src/uds_handler.h"
#include "../src/session_manager.h"
#include "../src/constants.h"

static void InitFixture(SessionManager_t* pMgr, UdsHandler_t* pHandler)
{
    SessionManager_Init(pMgr);
    UdsHandler_Init(pHandler, pMgr);
}

/* TC_UH_001 | Init | Initialize handler and manager | none | Positive | current session = DEFAULT */
static int TC_UH_001_Init_DefaultSession(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;

    InitFixture(&mgr, &handler);

    TEST_ASSERT_EQUAL(SESSION_DEFAULT, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_UH_002 | Valid | Request Default session | [0x10, 0x01] | Positive | [0x50,0x01], session=DEFAULT */
static int TC_UH_002_RequestDefault_PositiveResponse(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t request[2] = { 0x10u, 0x01u };
    uint8_t response[8];
    uint16_t length = 0u;
    bool result;

    InitFixture(&mgr, &handler);
    result = UdsHandler_ProcessRequest(&handler, request, 2u, response, &length);

    TEST_ASSERT(result);
    TEST_ASSERT_EQUAL(2, length);
    TEST_ASSERT_EQUAL(0x50, response[0]);
    TEST_ASSERT_EQUAL(0x01, response[1]);
    TEST_ASSERT_EQUAL(SESSION_DEFAULT, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_UH_003 | Valid | Request Programming session | [0x10, 0x02] | Positive | [0x50,0x02], session=PROGRAMMING */
static int TC_UH_003_RequestProgramming_PositiveResponse(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t request[2] = { 0x10u, 0x02u };
    uint8_t response[8];
    uint16_t length = 0u;
    bool result;

    InitFixture(&mgr, &handler);
    result = UdsHandler_ProcessRequest(&handler, request, 2u, response, &length);

    TEST_ASSERT(result);
    TEST_ASSERT_EQUAL(2, length);
    TEST_ASSERT_EQUAL(0x50, response[0]);
    TEST_ASSERT_EQUAL(0x02, response[1]);
    TEST_ASSERT_EQUAL(SESSION_PROGRAMMING, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_UH_004 | Valid | Request Extended session | [0x10, 0x03] | Positive | [0x50,0x03], session=EXTENDED */
static int TC_UH_004_RequestExtended_PositiveResponse(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t request[2] = { 0x10u, 0x03u };
    uint8_t response[8];
    uint16_t length = 0u;
    bool result;

    InitFixture(&mgr, &handler);
    result = UdsHandler_ProcessRequest(&handler, request, 2u, response, &length);

    TEST_ASSERT(result);
    TEST_ASSERT_EQUAL(2, length);
    TEST_ASSERT_EQUAL(0x50, response[0]);
    TEST_ASSERT_EQUAL(0x03, response[1]);
    TEST_ASSERT_EQUAL(SESSION_EXTENDED, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_UH_005 | Negative | Unsupported sub-function | [0x10, 0x05] | Boundary | [0x7F,0x10,0x12], session unchanged */
static int TC_UH_005_UnsupportedSubFunction_NegativeResponse(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t request[2] = { 0x10u, 0x05u };
    uint8_t response[8];
    uint16_t length = 0u;
    bool result;

    InitFixture(&mgr, &handler);
    result = UdsHandler_ProcessRequest(&handler, request, 2u, response, &length);

    TEST_ASSERT(!result);
    TEST_ASSERT_EQUAL(3, length);
    TEST_ASSERT_EQUAL(0x7F, response[0]);
    TEST_ASSERT_EQUAL(0x10, response[1]);
    TEST_ASSERT_EQUAL(0x12, response[2]);
    TEST_ASSERT_EQUAL(SESSION_DEFAULT, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_UH_006 | Negative | Unsupported service id | [0x22, 0x01] | Negative | [0x7F,0x22,0x11] */
static int TC_UH_006_UnsupportedServiceId_NegativeResponse(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t request[2] = { 0x22u, 0x01u };
    uint8_t response[8];
    uint16_t length = 0u;
    bool result;

    InitFixture(&mgr, &handler);
    result = UdsHandler_ProcessRequest(&handler, request, 2u, response, &length);

    TEST_ASSERT(!result);
    TEST_ASSERT_EQUAL(3, length);
    TEST_ASSERT_EQUAL(0x7F, response[0]);
    TEST_ASSERT_EQUAL(0x22, response[1]);
    TEST_ASSERT_EQUAL(0x11, response[2]);
    return 1;
}

/* TC_UH_007 | Negative | One-byte request | [0x10] | Boundary | [0x7F,0x10,0x13] */
static int TC_UH_007_OneByteRequest_NegativeResponse(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t request[1] = { 0x10u };
    uint8_t response[8];
    uint16_t length = 0u;
    bool result;

    InitFixture(&mgr, &handler);
    result = UdsHandler_ProcessRequest(&handler, request, 1u, response, &length);

    TEST_ASSERT(!result);
    TEST_ASSERT_EQUAL(3, length);
    TEST_ASSERT_EQUAL(0x7F, response[0]);
    TEST_ASSERT_EQUAL(0x10, response[1]);
    TEST_ASSERT_EQUAL(0x13, response[2]);
    return 1;
}

/* TC_UH_008 | Negative | Empty request (NULL, length 0) | none | Boundary | [0x7F,0x00,0x13] */
static int TC_UH_008_EmptyRequest_NegativeResponse(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t response[8];
    uint16_t length = 0u;
    bool result;

    InitFixture(&mgr, &handler);
    result = UdsHandler_ProcessRequest(&handler, NULL, 0u, response, &length);

    TEST_ASSERT(!result);
    TEST_ASSERT_EQUAL(3, length);
    TEST_ASSERT_EQUAL(0x7F, response[0]);
    TEST_ASSERT_EQUAL(0x00, response[1]);
    TEST_ASSERT_EQUAL(0x13, response[2]);
    return 1;
}

/* TC_UH_009 | Transition | Default to Programming | [0x10, 0x02] | Positive | session=PROGRAMMING */
static int TC_UH_009_TransitionDefaultToProgramming(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t request[2] = { 0x10u, 0x02u };
    uint8_t response[8];
    uint16_t length = 0u;

    InitFixture(&mgr, &handler);
    (void)UdsHandler_ProcessRequest(&handler, request, 2u, response, &length);

    TEST_ASSERT_EQUAL(SESSION_PROGRAMMING, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_UH_010 | Transition | Programming to Extended | [0x10,0x02] then [0x10,0x03] | Positive | session=EXTENDED */
static int TC_UH_010_TransitionProgrammingToExtended(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t toProgramming[2] = { 0x10u, 0x02u };
    uint8_t toExtended[2] = { 0x10u, 0x03u };
    uint8_t response[8];
    uint16_t length = 0u;

    InitFixture(&mgr, &handler);
    (void)UdsHandler_ProcessRequest(&handler, toProgramming, 2u, response, &length);
    (void)UdsHandler_ProcessRequest(&handler, toExtended, 2u, response, &length);

    TEST_ASSERT_EQUAL(SESSION_EXTENDED, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_UH_011 | Robustness | Invalid request after a valid one | [0x10,0x02] then [0x10,0x05] | Negative | session stays PROGRAMMING */
static int TC_UH_011_InvalidAfterValid_PreservesState(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t validRequest[2] = { 0x10u, 0x02u };
    uint8_t invalidRequest[2] = { 0x10u, 0x05u };
    uint8_t response[8];
    uint16_t length = 0u;

    InitFixture(&mgr, &handler);
    (void)UdsHandler_ProcessRequest(&handler, validRequest, 2u, response, &length);
    (void)UdsHandler_ProcessRequest(&handler, invalidRequest, 2u, response, &length);

    TEST_ASSERT_EQUAL(SESSION_PROGRAMMING, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_UH_012 | Robustness | Extra data appended to a valid request | [0x10,0x02,0xAA,0xBB] | Positive | extra bytes ignored, session=PROGRAMMING */
static int TC_UH_012_ExtraDataInRequest_IgnoredBytes(void)
{
    SessionManager_t mgr;
    UdsHandler_t handler;
    uint8_t request[4] = { 0x10u, 0x02u, 0xAAu, 0xBBu };
    uint8_t response[8];
    uint16_t length = 0u;
    bool result;

    InitFixture(&mgr, &handler);
    result = UdsHandler_ProcessRequest(&handler, request, 4u, response, &length);

    TEST_ASSERT(result);
    TEST_ASSERT_EQUAL(2, length);
    TEST_ASSERT_EQUAL(0x50, response[0]);
    TEST_ASSERT_EQUAL(0x02, response[1]);
    TEST_ASSERT_EQUAL(SESSION_PROGRAMMING, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

int main(void)
{
    int total = 0;
    int passed = 0;

    RUN_TEST(TC_UH_001_Init_DefaultSession, &total, &passed);
    RUN_TEST(TC_UH_002_RequestDefault_PositiveResponse, &total, &passed);
    RUN_TEST(TC_UH_003_RequestProgramming_PositiveResponse, &total, &passed);
    RUN_TEST(TC_UH_004_RequestExtended_PositiveResponse, &total, &passed);
    RUN_TEST(TC_UH_005_UnsupportedSubFunction_NegativeResponse, &total, &passed);
    RUN_TEST(TC_UH_006_UnsupportedServiceId_NegativeResponse, &total, &passed);
    RUN_TEST(TC_UH_007_OneByteRequest_NegativeResponse, &total, &passed);
    RUN_TEST(TC_UH_008_EmptyRequest_NegativeResponse, &total, &passed);
    RUN_TEST(TC_UH_009_TransitionDefaultToProgramming, &total, &passed);
    RUN_TEST(TC_UH_010_TransitionProgrammingToExtended, &total, &passed);
    RUN_TEST(TC_UH_011_InvalidAfterValid_PreservesState, &total, &passed);
    RUN_TEST(TC_UH_012_ExtraDataInRequest_IgnoredBytes, &total, &passed);

    printf("\n%d/%d tests passed\n", passed, total);
    return (passed == total) ? 0 : 1;
}
