#include <stdio.h>
#include <stddef.h>
#include "session_manager.h"
#include "uds_handler.h"
#include "constants.h"
#include "logger.h"

static void PrintResponse(const uint8_t* pResponse, uint16_t length)
{
    uint16_t i;

    for (i = 0u; i < length; i++)
    {
        printf("%02X ", pResponse[i]);
    }
    printf("\n");
}

int main(void)
{
    SessionManager_t sessionMgr;
    UdsHandler_t udsHandler;
    uint8_t response[UDS_RESPONSE_MAX_LENGTH];
    uint16_t responseLength;
    size_t i;

    /* Simulated tester requests; no real CAN/CAN FD communication is used */
    static const uint8_t requests[][2] =
    {
        { 0x10u, 0x02u }, /* Valid: switch to Programming session */
        { 0x10u, 0x05u }, /* Invalid: unsupported sub-function */
        { 0x22u, 0x01u }  /* Invalid: unsupported service */
    };

    SessionManager_Init(&sessionMgr);
    UdsHandler_Init(&udsHandler, &sessionMgr);

    LOG_INFO("Diagnostic Session Manager initialized. Current session: 0x%02X",
              SessionManager_GetCurrentSession(&sessionMgr));

    for (i = 0u; i < (sizeof(requests) / sizeof(requests[0])); i++)
    {
        LOG_INFO("Processing request: %02X %02X", requests[i][0], requests[i][1]);

        (void)UdsHandler_ProcessRequest(&udsHandler, requests[i], 2u, response, &responseLength);

        printf("Response: ");
        PrintResponse(response, responseLength);
        LOG_INFO("Current session: 0x%02X", SessionManager_GetCurrentSession(&sessionMgr));
    }

    return 0;
}
