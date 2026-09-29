#include "uds_handler.h"
#include "response_builder.h"
#include "constants.h"
#include <stddef.h>

void UdsHandler_Init(UdsHandler_t* pHandler, SessionManager_t* pSessionMgr)
{
    if (pHandler == NULL)
    {
        return;
    }

    pHandler->pSessionMgr = pSessionMgr;
}

bool UdsHandler_ProcessRequest(
    UdsHandler_t* pHandler,
    const uint8_t* pRequest,
    uint16_t requestLength,
    uint8_t* pResponse,
    uint16_t* pResponseLength)
{
    uint8_t serviceId;
    uint8_t subFunction;

    if ((pHandler == NULL) || (pHandler->pSessionMgr == NULL) ||
        (pResponse == NULL) || (pResponseLength == NULL))
    {
        return false;
    }

    if ((pRequest == NULL) || (requestLength < UDS_REQUEST_MIN_LENGTH))
    {
        /* Original SID is unknown when no byte is available; 0x00 is used as a placeholder */
        serviceId = ((pRequest != NULL) && (requestLength >= 1u)) ? pRequest[0] : 0x00u;
        ResponseBuilder_BuildNegativeResponse(serviceId, UDS_NRC_INCORRECT_LENGTH_OR_FORMAT, pResponse, pResponseLength);
        return false;
    }

    serviceId = pRequest[0];

    if (serviceId != UDS_SID_DIAGNOSTIC_SESSION_CONTROL)
    {
        ResponseBuilder_BuildNegativeResponse(serviceId, UDS_NRC_SERVICE_NOT_SUPPORTED, pResponse, pResponseLength);
        return false;
    }

    /* Only the first two bytes define behavior; any extra bytes are ignored */
    subFunction = pRequest[1];

    if (!SessionManager_RequestSession(pHandler->pSessionMgr, (DiagnosticSession_t)subFunction))
    {
        ResponseBuilder_BuildNegativeResponse(serviceId, UDS_NRC_SUBFUNCTION_NOT_SUPPORTED, pResponse, pResponseLength);
        return false;
    }

    ResponseBuilder_BuildPositiveResponse(serviceId, subFunction, pResponse, pResponseLength);
    return true;
}
