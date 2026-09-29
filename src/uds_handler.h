#ifndef UDS_HANDLER_H
#define UDS_HANDLER_H

#include <stdbool.h>
#include <stdint.h>
#include "session_manager.h"

/* Facade coordinating request validation, session transitions, and response building */
typedef struct
{
    SessionManager_t* pSessionMgr;
} UdsHandler_t;

/* Binds pHandler to pSessionMgr. No-op if pHandler is NULL. */
void UdsHandler_Init(UdsHandler_t* pHandler, SessionManager_t* pSessionMgr);

/*
 * Processes a raw UDS request and writes the response into pResponse / pResponseLength.
 * Returns true only for a valid Diagnostic Session Control (0x10) request that
 * resulted in a positive response and a session transition.
 */
bool UdsHandler_ProcessRequest(
    UdsHandler_t* pHandler,
    const uint8_t* pRequest,
    uint16_t requestLength,
    uint8_t* pResponse,
    uint16_t* pResponseLength);

#endif /* UDS_HANDLER_H */
