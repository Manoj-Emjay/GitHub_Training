#ifndef CONSTANTS_H
#define CONSTANTS_H

/* UDS Service Identifiers (SID) */
#define UDS_SID_DIAGNOSTIC_SESSION_CONTROL   (0x10u)
#define UDS_SID_NEGATIVE_RESPONSE            (0x7Fu)
#define UDS_POSITIVE_RESPONSE_OFFSET         (0x40u)

/* Diagnostic Session Control sub-functions (0x10) */
#define UDS_SUBFN_SESSION_DEFAULT            (0x01u)
#define UDS_SUBFN_SESSION_PROGRAMMING        (0x02u)
#define UDS_SUBFN_SESSION_EXTENDED           (0x03u)

/* Negative Response Codes (NRC) */
#define UDS_NRC_SERVICE_NOT_SUPPORTED        (0x11u)
#define UDS_NRC_SUBFUNCTION_NOT_SUPPORTED    (0x12u)
#define UDS_NRC_INCORRECT_LENGTH_OR_FORMAT   (0x13u)

/* Request/response length constraints */
#define UDS_REQUEST_MIN_LENGTH               (2u)
#define UDS_RESPONSE_MAX_LENGTH              (8u)

#endif /* CONSTANTS_H */
