# Software Design Document (SDD)
## UDS Diagnostic Session Manager

**Document Version:** 1.0  
**Last Updated:** 2026-09-28  
**Status:** Draft  
**Target Audience:** Bootloader Development Team, GitHub Copilot Training Participants

---

## 1. Executive Summary

This Software Design Document defines the architecture, design patterns, module structure, and implementation details for the **UDS Diagnostic Session Manager** component. The component handles automotive diagnostic session transitions as part of a bootloader's diagnostic interface.

The MVP implementation focuses on:
- UDS Service 0x10 (Diagnostic Session Control)
- Support for three session types: Default, Programming, and Extended
- Request validation and response generation
- Unit test coverage ≥80%
- CI/CD automation and AWS deployment

---

## 2. Design Objectives

| Objective | Rationale |
|-----------|-----------|
| **Modularity** | Clear separation of concerns for testability and maintainability |
| **Simplicity** | Intentionally simplified to fit 16-hour training without external dependencies |
| **Testability** | Core logic independent of hardware, CAN bus, or ECU integration |
| **Extensibility** | Foundation for adding more UDS services (e.g., 0x3E Tester Present) |
| **Documentation** | Clear APIs and design for team understanding and Copilot training |

---

## 3. Architecture Overview

### 3.1 High-Level Architecture

```
┌─────────────────────────────────────────────────────────┐
│                  Diagnostic Application                  │
│                   (main / entry point)                   │
└────────────────────┬────────────────────────────────────┘
                     │
      ┌──────────────┴──────────────┐
      │                             │
      v                             v
┌─────────────────┐        ┌────────────────┐
│  UDS Handler    │        │   REST API     │
│  (Input Logic)  │        │  (AWS Layer)   │
└────────┬────────┘        └────────┬───────┘
         │                          │
         └──────────────┬───────────┘
                        │
                        v
         ┌──────────────────────────┐
         │ Session Manager (Core)   │
         │ - State Management       │
         │ - Validation             │
         │ - Transitions            │
         └──────────────┬───────────┘
                        │
         ┌──────────────┴──────────────┐
         │                             │
         v                             v
    ┌─────────────┐          ┌──────────────────┐
    │  Response   │          │    Logging       │
    │  Builder    │          │  & Monitoring    │
    └─────────────┘          └──────────────────┘
```

### 3.2 Module Decomposition

| Module | Responsibility | Dependencies |
|--------|-----------------|--------------|
| **session_manager.py/c** | Core session state management and transitions | None (pure logic) |
| **uds_handler.py/c** | UDS protocol handling and request routing | session_manager |
| **response_builder.py/c** | Positive/negative response generation | None (utility) |
| **constants.py/c** | UDS constants, service IDs, NRCs | None |
| **logger.py/c** (Optional) | Logging interface | None |
| **main.py/c** | Application entry point and initialization | All modules |
| **rest_api.py** (AWS) | Flask/FastAPI REST wrapper | session_manager, uds_handler |

---

## 4. Data Design

### 4.1 Session Types (Enumeration)

```c
// Embedded C Style
typedef enum {
    SESSION_DEFAULT = 0x01,
    SESSION_PROGRAMMING = 0x02,
    SESSION_EXTENDED = 0x03
} DiagnosticSession_t;
```

Or:

```python
# Python Style
from enum import Enum

class DiagnosticSession(Enum):
    DEFAULT = 0x01
    PROGRAMMING = 0x02
    EXTENDED = 0x03
```

### 4.2 UDS Request/Response Structure

**Request Format:**
```
Byte 0: Service ID (0x10)
Byte 1: Sub-function (session type)
Optional: Additional data
```

**Response Formats:**

Positive Response:
```
Byte 0: Service ID + 0x40 (0x50 for 0x10)
Byte 1: Session sub-function
Optional: Session transition data
```

Negative Response:
```
Byte 0: 0x7F (Negative Response SID)
Byte 1: Original Service ID
Byte 2: Negative Response Code (NRC)
```

### 4.3 NRC (Negative Response Code) Definition

```c
typedef enum {
    NRC_INCORRECT_LENGTH = 0x13,
    NRC_SERVICE_NOT_SUPPORTED = 0x11,
    NRC_SUBFUNCTION_NOT_SUPPORTED = 0x12
} NegativeResponseCode_t;
```

### 4.4 Application State Structure

```python
class SessionManagerState:
    def __init__(self):
        self.current_session = DiagnosticSession.DEFAULT
        self.previous_session = None
        self.last_request = None
        self.transition_timestamp = None
```

---

## 5. Detailed Module Design

### 5.1 Session Manager Module

**Purpose:** Core session state management and validation  
**Language:** C or Python (language-agnostic design)  
**Dependencies:** None (pure business logic)

#### 5.1.1 Key Responsibilities

1. Initialize with DEFAULT_SESSION
2. Validate requested session
3. Execute session transition
4. Provide session query API
5. Maintain transition history (optional)

#### 5.1.2 API Design

**C API:**
```c
// Initialize
SessionManager_t* SessionManager_Init(void);

// Process session change request
bool SessionManager_RequestSession(
    SessionManager_t* pMgr,
    DiagnosticSession_t requestedSession
);

// Get current session
DiagnosticSession_t SessionManager_GetCurrentSession(
    const SessionManager_t* pMgr
);

// Reset to default (optional)
void SessionManager_Reset(SessionManager_t* pMgr);
```

**Python API:**
```python
class SessionManager:
    def __init__(self):
        self.current_session = DiagnosticSession.DEFAULT
    
    def request_session(self, session: DiagnosticSession) -> bool:
        """Request session change. Returns True if successful."""
        pass
    
    def get_current_session(self) -> DiagnosticSession:
        """Get the current session."""
        pass
    
    def reset(self):
        """Reset to DEFAULT_SESSION."""
        pass
```

#### 5.1.3 Validation Logic

A request is valid if:
- Session parameter is one of: 0x01, 0x02, 0x03
- No other constraints (simple MVP)

#### 5.1.4 State Transition Rules

| Current | Requested | Allowed | Outcome |
|---------|-----------|---------|---------|
| Any | Default | Yes | → Default |
| Any | Programming | Yes | → Programming |
| Any | Extended | Yes | → Extended |

**Transition Atomicity:** Session changes only after successful validation.

---

### 5.2 UDS Handler Module

**Purpose:** Protocol-level request parsing and response coordination  
**Dependencies:** SessionManager, ResponseBuilder

#### 5.2.1 Key Responsibilities

1. Parse incoming UDS requests
2. Extract service ID and sub-function
3. Delegate to appropriate handler
4. Coordinate session manager and response builder
5. Return formatted response

#### 5.2.2 API Design

**C API:**
```c
// Process a raw UDS request
bool UdsHandler_ProcessRequest(
    const uint8_t* pRequest,
    uint16_t requestLength,
    uint8_t* pResponse,
    uint16_t* pResponseLength
);
```

**Python API:**
```python
class UdsHandler:
    def __init__(self, session_mgr: SessionManager):
        self.session_mgr = session_mgr
    
    def process_request(self, request: bytes) -> bytes:
        """Process UDS request and return response bytes."""
        pass
```

#### 5.2.3 Request Processing Flow

```
Input: Raw request bytes
    ↓
Validate Length (≥2 bytes)
    ↓
Extract Service ID (Byte 0)
    ↓
Is Service 0x10?
    ├─ YES → Extract sub-function
    │         ↓
    │         Is sub-function valid (0x01-0x03)?
    │         ├─ YES → Call SessionManager_RequestSession()
    │         │        ↓
    │         │        Build Positive Response (0x50 + subfn)
    │         └─ NO → Build Negative Response (0x7F 0x10 0x12)
    │
    └─ NO → Build Negative Response (0x7F ServiceID 0x11)
    ↓
Output: Response bytes
```

---

### 5.3 Response Builder Module

**Purpose:** Construct properly formatted UDS responses  
**Dependencies:** None (utility module)

#### 5.3.1 API Design

**C API:**
```c
// Build positive response
void ResponseBuilder_BuildPositiveResponse(
    uint8_t serviceId,
    uint8_t subFunction,
    uint8_t* pResponse,
    uint16_t* pResponseLength
);

// Build negative response
void ResponseBuilder_BuildNegativeResponse(
    uint8_t originalServiceId,
    uint8_t nrc,
    uint8_t* pResponse,
    uint16_t* pResponseLength
);
```

**Python API:**
```python
class ResponseBuilder:
    @staticmethod
    def build_positive_response(
        service_id: int,
        sub_function: int
    ) -> bytes:
        """Build positive UDS response."""
        pass
    
    @staticmethod
    def build_negative_response(
        original_service_id: int,
        nrc: int
    ) -> bytes:
        """Build negative UDS response."""
        pass
```

#### 5.3.2 Response Format Rules

- **Positive:** [SID+0x40] [SubFunction] [Optional Data]
- **Negative:** [0x7F] [Original SID] [NRC]

---

## 6. Data Flow Diagrams

### 6.1 Valid Request Flow

```
Request: 10 02
    ↓
UDS Handler receives bytes
    ↓
Validate: Length ≥ 2 ✓, SID = 0x10 ✓, SubFn = 0x02 ✓
    ↓
SessionManager.request_session(0x02) → SUCCESS
    ↓
Current Session: DEFAULT → PROGRAMMING ✓
    ↓
ResponseBuilder.build_positive_response(0x10, 0x02)
    ↓
Response: 50 02
```

### 6.2 Invalid SubFunction Flow

```
Request: 10 05
    ↓
UDS Handler receives bytes
    ↓
Validate: Length ≥ 2 ✓, SID = 0x10 ✓, SubFn = 0x05 ✗
    ↓
SessionManager.request_session(0x05) → FAIL
    ↓
Current Session: UNCHANGED
    ↓
ResponseBuilder.build_negative_response(0x10, 0x12)
    ↓
Response: 7F 10 12
```

### 6.3 Invalid Service ID Flow

```
Request: 22 01
    ↓
UDS Handler receives bytes
    ↓
Validate: Length ≥ 2 ✓, SID = 0x22 ✗
    ↓
SID not recognized
    ↓
ResponseBuilder.build_negative_response(0x22, 0x11)
    ↓
Response: 7F 22 11
```

---

## 7. State Machine Design

### 7.1 Session State Machine

```
┌─────────────────────────────────────────────────────┐
│                   DEFAULT_SESSION                   │
│                   (Initial State)                   │
└────────┬────────────────────────────┬───────────────┘
         │                            │
   [10 02]                       [10 03]
         │                            │
         v                            v
   ┌─────────────────┐          ┌──────────────────┐
   │ PROGRAMMING_    │   [10 01]│  EXTENDED_       │
   │ SESSION         ├◄─────────┤  SESSION         │
   └────────┬────────┘          └────────┬─────────┘
         │                            │
         └───────────────────┬────────┘
                      [10 02] or [10 03]
                             │
                             ↓
                      Valid session change


Invalid Request → NRC 0x11, 0x12, or 0x13 (State Unchanged)
```

---

## 8. Error Handling Strategy

### 8.1 Error Categories

| Error Type | Trigger | NRC | Handling |
|-----------|---------|-----|----------|
| **Length Error** | Request < 2 bytes | 0x13 | Return negative response, state unchanged |
| **Service Error** | SID ≠ 0x10 | 0x11 | Return negative response, state unchanged |
| **SubFunction Error** | Session not in [0x01, 0x02, 0x03] | 0x12 | Return negative response, state unchanged |
| **Null Pointer** | Invalid request buffer | N/A | Return error, state unchanged |

### 8.2 Error Response Generation

All errors result in a negative response without modifying session state:

```
7F <Original_SID> <NRC>
```

### 8.3 Application Robustness

- No unhandled exceptions/segmentation faults
- All error paths return defined response
- State rollback on validation failure (implicit - no change made)
- Logging of all error conditions

---

## 9. API Contracts

### 9.1 Initialization Contract

**Precondition:** Application startup  
**Operation:** `SessionManager_Init()` or `SessionManager().__init__()`  
**Postcondition:** 
- Current session = DEFAULT_SESSION
- Ready to accept requests

**Example:**
```python
mgr = SessionManager()
assert mgr.get_current_session() == DiagnosticSession.DEFAULT
```

### 9.2 Request Processing Contract

**Precondition:** Valid request buffer, initialized SessionManager  
**Operation:** `UdsHandler_ProcessRequest(req, len, resp, resp_len)`  
**Postcondition:**
- Response buffer populated
- Response length set
- Session state updated only for valid requests

**Example (Valid):**
```python
request = bytes([0x10, 0x02])
response = handler.process_request(request)
assert response == bytes([0x50, 0x02])
assert mgr.get_current_session() == DiagnosticSession.PROGRAMMING
```

**Example (Invalid):**
```python
request = bytes([0x10, 0x05])  # Invalid session
response = handler.process_request(request)
assert response == bytes([0x7F, 0x10, 0x12])
assert mgr.get_current_session() == DiagnosticSession.DEFAULT  # Unchanged
```

---

## 10. Test Strategy

### 10.1 Unit Test Scope

**Modules to test:**
- SessionManager (core logic)
- UdsHandler (integration of validation + session mgr)
- ResponseBuilder (response formatting)

**Excluded from unit testing:**
- AWS Lambda/REST handler (integration tested separately)
- Docker image build (infrastructure tested separately)

### 10.2 Test Categories (12+ Test Cases)

| TC ID | Category | Test Case | Expected Result |
|-------|----------|-----------|-----------------|
| TC-001 | Init | Initialize app | DEFAULT_SESSION |
| TC-002 | Valid | Request Default | 50 01 + DEFAULT |
| TC-003 | Valid | Request Programming | 50 02 + PROGRAMMING |
| TC-004 | Valid | Request Extended | 50 03 + EXTENDED |
| TC-005 | Invalid | Unsupported session 0x05 | 7F 10 12 |
| TC-006 | Invalid | Wrong SID 0x22 | 7F 22 11 |
| TC-007 | Invalid | One-byte request | 7F 10 13 |
| TC-008 | Invalid | Empty request | 7F ?? 13 |
| TC-009 | Transition | Default → Programming | PROGRAMMING |
| TC-010 | Transition | Programming → Extended | EXTENDED |
| TC-011 | Robustness | Invalid after valid | Previous state |
| TC-012 | Robustness | Extra data in request | Defined behavior |

### 10.3 Coverage Goals

- **Statement Coverage:** ≥80%
- **Branch Coverage:** ≥75%
- **Error Path Coverage:** 100%

---

## 11. Logging & Monitoring

### 11.1 Logged Events

```
[INFO] Application initialized
[INFO] Received UDS request: 10 02
[INFO] Request validation: PASS
[INFO] Session transition: DEFAULT_SESSION → PROGRAMMING_SESSION
[INFO] Generated response: 50 02
```

### 11.2 Error Logging

```
[ERROR] Invalid request length: 1 (expected ≥2)
[ERROR] Unsupported service ID: 0x22
[ERROR] Unsupported sub-function: 0x05
[ERROR] Null request buffer
```

### 11.3 Logging Levels

- **INFO:** Request received, session changes, response sent
- **DEBUG:** Internal state transitions, validation steps
- **ERROR:** Invalid requests, processing failures
- **WARN:** Deprecated operations, edge cases

---

## 12. Design Patterns

### 12.1 Patterns Used

| Pattern | Application |
|---------|-------------|
| **Singleton** | SessionManager (one active session at a time) |
| **Strategy** | Response building (positive vs. negative) |
| **State** | Session state management |
| **Facade** | UdsHandler presents unified interface |

### 12.2 Why These Patterns?

- **Singleton:** Ensures single session state across app
- **Strategy:** Flexible response generation
- **State:** Clear session transitions
- **Facade:** Simplifies protocol handling complexity

---

## 13. Interface Design

### 13.1 Programming Language Options

**Option A: Python (Recommended for MVP)**
- Faster development (Copilot-friendly)
- Easy AWS deployment (Lambda/API Gateway)
- Clear syntax for training
- Easier testing and mocking

**Option B: Embedded C**
- More aligned with bootloader work
- Better for hardware integration (future)
- Steeper learning curve for training
- Requires compilation toolchain

### 13.2 Recommended Language: Python

**Rationale:**
1. Complete 16-hour training without infrastructure overhead
2. AWS Lambda/API Gateway deployment simpler
3. GitHub Copilot works excellently with Python
4. Future C integration can wrap this logic

**Framework Options:**
- Flask (lightweight, educational)
- FastAPI (modern, async, performance)

---

## 14. Extensibility Points

### 14.1 Future Enhancements

**Phase 2: Additional UDS Services**
- UDS 0x3E (Tester Present)
- Separate handler for each service

**Phase 3: Session Transition Matrix**
- Add configurable state machine rules
- More complex session controls

**Phase 4: Hardware Integration**
- AUTOSAR integration
- Real CAN communication
- MCAL integration

### 14.2 Design for Extension

```
UdsHandler (Dispatcher)
    ├── Service_0x10_Handler
    ├── Service_0x3E_Handler  [Future]
    ├── Service_0x27_Handler  [Future]
    └── Service_0x34_Handler  [Future]
```

---

## 15. Non-Functional Requirements Mapping

| NFR | Design Aspect | Implementation |
|-----|---|---|
| **Maintainability** | Module isolation, clear APIs | Separate files, documented contracts |
| **Testability** | Pure business logic | No external dependencies in core modules |
| **Error Handling** | All errors return defined NRC | No exceptions escape main loop |
| **Logging** | Every major operation logged | Structured logging format |
| **Code Quality** | Linting standards | PEP8 (Python) or MISRA-C |
| **Performance** | Minimal CPU/memory | No dynamic allocation in MVP |

---

## 16. Build & Integration Points

### 16.1 Build Artifacts

```
bin/
├── diagnostic_session_manager (Standalone executable)
└── libsession_manager.so (Shared library for AWS)

lib/
├── libuds_handler.a
└── libsession_manager.a
```

### 16.2 Integration Points

**Local Testing:**
- Python unittest or pytest
- C: Google Test (gtest) or Check framework

**CI Integration:**
- GitHub Actions workflow
- Automated test execution
- Coverage reporting

**AWS Integration:**
- Lambda function handler
- API Gateway endpoint
- Container image (ECR)

---

## 17. Deployment Architecture (AWS)

### 17.1 Containerized Deployment

```
GitHub Repository
    ↓
GitHub Actions Workflow
    ├─ Build container image
    ├─ Push to Amazon ECR
    └─ Deploy to ECS/Fargate
         ↓
    REST API Endpoint
         ↓
    Consumer (Diagnostic Tester Simulation)
```

### 17.2 REST API Interface (AWS)

**Endpoint:** `POST /uds/session`

**Request:**
```json
{
  "request": "1002"
}
```

**Response:**
```json
{
  "response": "5002",
  "session": "PROGRAMMING_SESSION",
  "status": "SUCCESS"
}
```

---

## 18. Configuration & Constants

### 18.1 Supported Services (MVP)

```python
UDS_SERVICES = {
    0x10: "DiagnosticSessionControl",
    # More services in Phase 2+
}
```

### 18.2 Supported Sessions

```python
SUPPORTED_SESSIONS = {
    0x01: "DEFAULT_SESSION",
    0x02: "PROGRAMMING_SESSION",
    0x03: "EXTENDED_SESSION"
}
```

### 18.3 NRC Mappings

```python
NEGATIVE_RESPONSE_CODES = {
    0x11: "ServiceNotSupported",
    0x12: "SubfunctionNotSupported",
    0x13: "IncorrectMessageLengthOrInvalidFormat"
}
```

---

## 19. Risk Analysis & Mitigations

| Risk | Likelihood | Impact | Mitigation |
|------|-----------|--------|-----------|
| Invalid request causes crash | Low | High | Input validation + error handling |
| Session state corruption | Low | High | Atomic transitions, rollback on error |
| Missing edge cases | Medium | Medium | Comprehensive test suite (TC-001 to TC-012+) |
| Performance degradation | Low | Low | No heavy computation (MVP scope) |
| AWS deployment failure | Low | Medium | Use standard Lambda/ECS patterns |

---

## 20. Design Review Checklist

- [ ] All functional requirements mapped to design
- [ ] All non-functional requirements addressed
- [ ] API contracts clearly defined
- [ ] Error handling complete for all error cases
- [ ] State machine unambiguous
- [ ] Test strategy covers all requirements
- [ ] Extensibility points documented
- [ ] Performance constraints met
- [ ] Security concerns identified
- [ ] Logging strategy sufficient

---

## 21. Next Steps

1. **Code Generation:** Use GitHub Copilot to generate module implementations
2. **Unit Test Development:** Generate test suite based on test strategy
3. **Integration Testing:** Test module interactions
4. **Code Review:** Peer review with Copilot assistance
5. **CI/CD Setup:** GitHub Actions workflow
6. **AWS Deployment:** Container build and deployment
7. **End-to-End Testing:** Live API testing

---

## 22. Document Approvals

| Role | Name | Date | Signature |
|------|------|------|-----------|
| Lead Architect | [TBD] | [TBD] | |
| Training Lead | [TBD] | [TBD] | |
| Tech Lead | [TBD] | [TBD] | |

---

## Appendix A: UDS Service 0x10 Reference

**Service ID:** 0x10  
**Service Name:** DiagnosticSessionControl  
**Positive Response SID:** 0x50  
**Negative Response SID:** 0x7F

**Request Format:**
```
[0x10] [Sub-function] [Optional Parameters]
```

**Positive Response:**
```
[0x50] [Sub-function] [Optional Data]
```

**Negative Response:**
```
[0x7F] [0x10] [NRC]
```

---

## Appendix B: Module Dependency Graph

```
main
 ├── session_manager
 ├── uds_handler
 │   ├── session_manager
 │   └── response_builder
 ├── response_builder
 └── logger (optional)

rest_api (AWS Layer)
 ├── session_manager
 ├── uds_handler
 └── Flask/FastAPI
```

---

## Appendix C: File Structure

```
bootloader-session-manager/
│
├── src/
│   ├── __init__.py
│   ├── session_manager.py
│   ├── uds_handler.py
│   ├── response_builder.py
│   ├── constants.py
│   ├── logger.py (optional)
│   ├── main.py
│   └── rest_api.py (AWS)
│
├── tests/
│   ├── __init__.py
│   ├── test_session_manager.py
│   ├── test_uds_handler.py
│   ├── test_response_builder.py
│   └── conftest.py (pytest fixtures)
│
├── docs/
│   ├── SDD_Diagnostic_Session_Manager.md (this file)
│   ├── architecture.md
│   └── API_reference.md
│
├── .github/
│   └── workflows/
│       └── ci.yml
│
├── Dockerfile
├── requirements.txt
├── setup.py (if Python package)
├── README.md
└── pytest.ini or .coveragerc (test config)
```

---

**End of Software Design Document**
