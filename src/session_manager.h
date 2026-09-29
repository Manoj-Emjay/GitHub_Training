#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <stdbool.h>
#include "constants.h"

/* Diagnostic session identifiers, aligned with UDS 0x10 sub-functions */
typedef enum
{
    SESSION_DEFAULT     = UDS_SUBFN_SESSION_DEFAULT,
    SESSION_PROGRAMMING = UDS_SUBFN_SESSION_PROGRAMMING,
    SESSION_EXTENDED    = UDS_SUBFN_SESSION_EXTENDED
} DiagnosticSession_t;

/* Statically allocated by the caller; no dynamic memory is used */
typedef struct
{
    DiagnosticSession_t currentSession;
    DiagnosticSession_t previousSession;
} SessionManager_t;

/* Initializes pMgr to SESSION_DEFAULT. No-op if pMgr is NULL. */
void SessionManager_Init(SessionManager_t* pMgr);

/* Validates and applies requestedSession. Returns false and leaves state unchanged if invalid. */
bool SessionManager_RequestSession(SessionManager_t* pMgr, DiagnosticSession_t requestedSession);

/* Returns SESSION_DEFAULT if pMgr is NULL. */
DiagnosticSession_t SessionManager_GetCurrentSession(const SessionManager_t* pMgr);

/* Resets pMgr to SESSION_DEFAULT. No-op if pMgr is NULL. */
void SessionManager_Reset(SessionManager_t* pMgr);

#endif /* SESSION_MANAGER_H */
