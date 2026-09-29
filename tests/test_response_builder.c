#include "test_framework.h"
#include "../src/response_builder.h"
#include "../src/constants.h"

/* TC_RB_001 | Positive | Build positive response for Programming sub-function | SID=0x10, subFn=0x02 | Positive | [0x50, 0x02], length 2 */
static int TC_RB_001_BuildPositiveResponse_FormatsCorrectly(void)
{
    uint8_t response[8];
    uint16_t length = 0u;

    ResponseBuilder_BuildPositiveResponse(UDS_SID_DIAGNOSTIC_SESSION_CONTROL, 0x02u, response, &length);

    TEST_ASSERT_EQUAL(2, length);
    TEST_ASSERT_EQUAL(0x50, response[0]);
    TEST_ASSERT_EQUAL(0x02, response[1]);
    return 1;
}

/* TC_RB_002 | Negative | Build negative response for unsupported sub-function | SID=0x10, NRC=0x12 | Negative | [0x7F, 0x10, 0x12], length 3 */
static int TC_RB_002_BuildNegativeResponse_FormatsCorrectly(void)
{
    uint8_t response[8];
    uint16_t length = 0u;

    ResponseBuilder_BuildNegativeResponse(UDS_SID_DIAGNOSTIC_SESSION_CONTROL, UDS_NRC_SUBFUNCTION_NOT_SUPPORTED, response, &length);

    TEST_ASSERT_EQUAL(3, length);
    TEST_ASSERT_EQUAL(0x7F, response[0]);
    TEST_ASSERT_EQUAL(0x10, response[1]);
    TEST_ASSERT_EQUAL(0x12, response[2]);
    return 1;
}

/* TC_RB_003 | Error Handling | Null response buffer/length pointers | NULL buffers | Negative | no crash, no write */
static int TC_RB_003_NullPointers_DoNotCrash(void)
{
    uint16_t length = 0u;

    ResponseBuilder_BuildPositiveResponse(0x10u, 0x01u, NULL, &length);
    ResponseBuilder_BuildNegativeResponse(0x10u, 0x12u, NULL, &length);
    return 1;
}

int main(void)
{
    int total = 0;
    int passed = 0;

    RUN_TEST(TC_RB_001_BuildPositiveResponse_FormatsCorrectly, &total, &passed);
    RUN_TEST(TC_RB_002_BuildNegativeResponse_FormatsCorrectly, &total, &passed);
    RUN_TEST(TC_RB_003_NullPointers_DoNotCrash, &total, &passed);

    printf("\n%d/%d tests passed\n", passed, total);
    return (passed == total) ? 0 : 1;
}
