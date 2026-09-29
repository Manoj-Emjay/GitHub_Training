#include "test_framework.h"
#include "../src/session_manager.h"

/* TC_SM_001 | Init | Initialize manager | none | Positive | current session = DEFAULT */
static int TC_SM_001_Init_SetsDefaultSession(void)
{
    SessionManager_t mgr;

    SessionManager_Init(&mgr);

    TEST_ASSERT_EQUAL(SESSION_DEFAULT, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_SM_002 | Valid | Request Default session | SESSION_DEFAULT | Positive | succeeds, stays DEFAULT */
static int TC_SM_002_RequestDefault_Succeeds(void)
{
    SessionManager_t mgr;
    bool result;

    SessionManager_Init(&mgr);
    result = SessionManager_RequestSession(&mgr, SESSION_DEFAULT);

    TEST_ASSERT(result);
    TEST_ASSERT_EQUAL(SESSION_DEFAULT, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_SM_003 | Valid | Request Programming session | SESSION_PROGRAMMING | Positive | succeeds, transitions */
static int TC_SM_003_RequestProgramming_Succeeds(void)
{
    SessionManager_t mgr;
    bool result;

    SessionManager_Init(&mgr);
    result = SessionManager_RequestSession(&mgr, SESSION_PROGRAMMING);

    TEST_ASSERT(result);
    TEST_ASSERT_EQUAL(SESSION_PROGRAMMING, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_SM_004 | Valid | Request Extended session | SESSION_EXTENDED | Positive | succeeds, transitions */
static int TC_SM_004_RequestExtended_Succeeds(void)
{
    SessionManager_t mgr;
    bool result;

    SessionManager_Init(&mgr);
    result = SessionManager_RequestSession(&mgr, SESSION_EXTENDED);

    TEST_ASSERT(result);
    TEST_ASSERT_EQUAL(SESSION_EXTENDED, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_SM_005 | Negative | Request unsupported session id 0x05 | 0x05 | Boundary | fails, state unchanged */
static int TC_SM_005_RequestUnsupportedSession_Fails(void)
{
    SessionManager_t mgr;
    bool result;

    SessionManager_Init(&mgr);
    result = SessionManager_RequestSession(&mgr, (DiagnosticSession_t)0x05);

    TEST_ASSERT(!result);
    TEST_ASSERT_EQUAL(SESSION_DEFAULT, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_SM_006 | Valid | Reset after transition | SESSION_EXTENDED then reset | Positive | returns to DEFAULT */
static int TC_SM_006_Reset_ReturnsToDefault(void)
{
    SessionManager_t mgr;

    SessionManager_Init(&mgr);
    (void)SessionManager_RequestSession(&mgr, SESSION_EXTENDED);
    SessionManager_Reset(&mgr);

    TEST_ASSERT_EQUAL(SESSION_DEFAULT, SessionManager_GetCurrentSession(&mgr));
    return 1;
}

/* TC_SM_007 | Error Handling | Null manager pointer | NULL | Negative | no crash, request fails */
static int TC_SM_007_NullPointer_DoesNotCrash(void)
{
    SessionManager_Init(NULL);
    SessionManager_Reset(NULL);

    TEST_ASSERT(!SessionManager_RequestSession(NULL, SESSION_DEFAULT));
    TEST_ASSERT_EQUAL(SESSION_DEFAULT, SessionManager_GetCurrentSession(NULL));
    return 1;
}

int main(void)
{
    int total = 0;
    int passed = 0;

    RUN_TEST(TC_SM_001_Init_SetsDefaultSession, &total, &passed);
    RUN_TEST(TC_SM_002_RequestDefault_Succeeds, &total, &passed);
    RUN_TEST(TC_SM_003_RequestProgramming_Succeeds, &total, &passed);
    RUN_TEST(TC_SM_004_RequestExtended_Succeeds, &total, &passed);
    RUN_TEST(TC_SM_005_RequestUnsupportedSession_Fails, &total, &passed);
    RUN_TEST(TC_SM_006_Reset_ReturnsToDefault, &total, &passed);
    RUN_TEST(TC_SM_007_NullPointer_DoesNotCrash, &total, &passed);

    printf("\n%d/%d tests passed\n", passed, total);
    return (passed == total) ? 0 : 1;
}
