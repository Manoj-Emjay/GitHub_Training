#ifndef RESPONSE_BUILDER_H
#define RESPONSE_BUILDER_H

#include <stdint.h>

/* Builds [SID+0x40][subFunction] into pResponse and sets *pResponseLength = 2. */
void ResponseBuilder_BuildPositiveResponse(
    uint8_t serviceId,
    uint8_t subFunction,
    uint8_t* pResponse,
    uint16_t* pResponseLength);

/* Builds [0x7F][originalServiceId][nrc] into pResponse and sets *pResponseLength = 3. */
void ResponseBuilder_BuildNegativeResponse(
    uint8_t originalServiceId,
    uint8_t nrc,
    uint8_t* pResponse,
    uint16_t* pResponseLength);

#endif /* RESPONSE_BUILDER_H */
