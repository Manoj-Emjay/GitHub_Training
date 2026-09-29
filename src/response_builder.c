#include "response_builder.h"
#include "constants.h"
#include <stddef.h>

void ResponseBuilder_BuildPositiveResponse(
    uint8_t serviceId,
    uint8_t subFunction,
    uint8_t* pResponse,
    uint16_t* pResponseLength)
{
    if ((pResponse == NULL) || (pResponseLength == NULL))
    {
        return;
    }

    pResponse[0] = (uint8_t)(serviceId + UDS_POSITIVE_RESPONSE_OFFSET);
    pResponse[1] = subFunction;
    *pResponseLength = 2u;
}

void ResponseBuilder_BuildNegativeResponse(
    uint8_t originalServiceId,
    uint8_t nrc,
    uint8_t* pResponse,
    uint16_t* pResponseLength)
{
    if ((pResponse == NULL) || (pResponseLength == NULL))
    {
        return;
    }

    pResponse[0] = UDS_SID_NEGATIVE_RESPONSE;
    pResponse[1] = originalServiceId;
    pResponse[2] = nrc;
    *pResponseLength = 3u;
}
