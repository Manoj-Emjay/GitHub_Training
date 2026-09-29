#include "session_manager.h"
#include <stddef.h>

static bool SessionManager_IsSupported(DiagnosticSession_t session)
{
    return (session == SESSION_DEFAULT) ||
           (session == SESSION_PROGRAMMING) ||
           (session == SESSION_EXTENDED);
}

void SessionManager_Init(SessionManager_t* pMgr)
{
    if (pMgr == NULL)
    {
        return;
    }

    pMgr->currentSession = SESSION_DEFAULT;
    pMgr->previousSession = SESSION_DEFAULT;
}

bool SessionManager_RequestSession(SessionManager_t* pMgr, DiagnosticSession_t requestedSession)
{
    if ((pMgr == NULL) || !SessionManager_IsSupported(requestedSession))
    {
        return false;
    }

    /* Transition is atomic: state only changes after successful validation */
    pMgr->previousSession = pMgr->currentSession;
    pMgr->currentSession = requestedSession;

    return true;
}

DiagnosticSession_t SessionManager_GetCurrentSession(const SessionManager_t* pMgr)
{
    if (pMgr == NULL)
    {
        return SESSION_DEFAULT;
    }

    return pMgr->currentSession;
}

void SessionManager_Reset(SessionManager_t* pMgr)
{
    if (pMgr == NULL)
    {
        return;
    }

    pMgr->previousSession = pMgr->currentSession;
    pMgr->currentSession = SESSION_DEFAULT;
}
