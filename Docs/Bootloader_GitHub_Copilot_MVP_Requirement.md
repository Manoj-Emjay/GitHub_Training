# GitHub Copilot Training MVP
## Bootloader Development Team: UDS ECU Diagnostic Session Manager

**Training duration:** 16 hours  
**Target audience:** Bootloader / Automotive Embedded Software Development Team  
**Primary goal:** Learn how GitHub Copilot can support the software lifecycle from requirement understanding through development, refactoring, review, CI/CD, and deployment.

---

## 1. MVP Use Case

### Use Case Name
**UDS Diagnostic Session Manager**

### Business Context

An automotive ECU bootloader communicates with a diagnostic tester using UDS (Unified Diagnostic Services). One of the basic bootloader responsibilities is handling diagnostic session transitions.

For this training MVP, we will implement a small software component that receives a UDS Diagnostic Session Control request (`0x10`), validates the requested session, changes the current diagnostic session, and generates the corresponding positive or negative response.

The implementation is intentionally simplified. It does **not** communicate with a real CAN/CAN FD network or an actual ECU. Instead, the UDS request and response are represented using software data structures.

This makes the use case suitable for a short GitHub Copilot training while still being recognizable to a bootloader development team.

---

# 2. Objective

Develop a small **Diagnostic Session Manager** that supports:

1. Default Session
2. Programming Session
3. Extended Diagnostic Session

The component shall:

- Accept a UDS `0x10 Diagnostic Session Control` request.
- Validate the requested session.
- Change the active diagnostic session when valid.
- Return a positive response for a valid request.
- Return an appropriate NRC for an invalid request.
- Provide APIs that can be unit tested.
- Be packaged as a small application that can run locally and in AWS.

---

# 3. Scope

## 3.1 In Scope

- UDS service `0x10`
- Session identification
- Request validation
- Session transition
- Positive response generation
- Negative response generation
- Unit tests
- Static/code-quality checks
- GitHub Actions CI pipeline
- Containerization
- AWS deployment
- Documentation

## 3.2 Out of Scope

The following are deliberately excluded from the MVP:

- Real CAN/CAN FD communication
- AUTOSAR BSW integration
- PduR / DCM / CanTp integration
- Real ECU hardware
- Flash erase/programming
- SecurityAccess (`0x27`)
- RequestDownload (`0x34`)
- TransferData (`0x36`)
- RequestTransferExit (`0x37`)
- Authentication
- HSM integration
- Watchdog handling
- NVM persistence
- Timing implementation
- Actual vehicle communication

These topics can be discussed as future extensions but should not be implemented during the 16-hour training.

---

# 4. Simplified Architecture

```text
             +----------------------+
             | Diagnostic Tester   |
             |   (Simulated Input) |
             +----------+-----------+
                        |
                        | UDS 0x10 Request
                        v
             +----------------------+
             | Diagnostic Session  |
             |      Manager        |
             +----------+-----------+
                        |
              +---------+---------+
              |                   |
              v                   v
       Validate Request     Change Session
              |                   |
              +---------+---------+
                        |
                        v
             +----------------------+
             | UDS Response Builder |
             +----------+-----------+
                        |
                        v
             Positive / Negative
                  Response
```

---

# 5. Functional Requirement

## FR-001: Diagnostic Session Control

The system shall support the UDS Diagnostic Session Control service `0x10`.

### Input

A request shall contain:

```text
Byte 0: Service ID = 0x10
Byte 1: Sub-function
```

Supported sub-functions:

| Sub-function | Session | Description |
|---|---|---|
| `0x01` | Default Session | Normal ECU operation |
| `0x02` | Programming Session | Bootloader programming mode |
| `0x03` | Extended Diagnostic Session | Extended diagnostics |

Example:

```text
10 02
```

means:

```text
Diagnostic Session Control
Programming Session
```

---

# 6. Session State Requirement

The application shall maintain the current diagnostic session.

Initial state:

```text
DEFAULT_SESSION
```

Supported states:

```text
DEFAULT_SESSION
PROGRAMMING_SESSION
EXTENDED_SESSION
```

The current session shall be retrievable through an API.

Example:

```text
get_current_session()
```

---

# 7. Request Validation

The system shall validate the request before changing the session.

### Valid Request

A request is valid when:

- The request contains at least two bytes.
- Byte 0 is `0x10`.
- Byte 1 is one of:
  - `0x01`
  - `0x02`
  - `0x03`

### Invalid Request

The request shall be rejected when:

- The request is empty.
- The request contains fewer than two bytes.
- The service ID is not `0x10`.
- The requested session is unsupported.

---

# 8. Positive Response Requirement

For a valid request, the system shall:

1. Change the current session.
2. Generate a positive response.

The positive response SID for `0x10` shall be:

```text
0x50
```

Example:

### Request

```text
10 02
```

### Response

```text
50 02
```

---

# 9. Negative Response Requirement

For an invalid request, the system shall generate a UDS negative response.

Negative response format:

```text
7F <Original SID> <NRC>
```

The MVP shall support the following NRCs.

| NRC | Name | Usage |
|---|---|---|
| `0x13` | Incorrect Message Length or Invalid Format | Request length is invalid |
| `0x11` | Service Not Supported | SID is not `0x10` |
| `0x12` | Sub-function Not Supported | Session sub-function is unsupported |

Examples:

### Invalid SID

Request:

```text
22 01
```

Response:

```text
7F 22 11
```

### Invalid Length

Request:

```text
10
```

Response:

```text
7F 10 13
```

### Unsupported Session

Request:

```text
10 05
```

Response:

```text
7F 10 12
```

---

# 10. Session Transition Requirement

The system shall update the current session only after successful validation.

Example:

```text
Initial:
DEFAULT_SESSION

Request:
10 02

Result:
PROGRAMMING_SESSION
```

For an invalid request:

```text
Initial:
DEFAULT_SESSION

Request:
10 05

Result:
DEFAULT_SESSION
```

The previous valid session shall not be modified when the request is rejected.

---

# 11. API Requirement

The implementation shall expose a simple interface.

Suggested APIs:

```text
initialize()
process_request(request)
get_current_session()
```

The exact programming language and naming convention may be selected by the training team.

Recommended options:

### Option A: Python

Best for completing the entire 16-hour MVP quickly and deploying a simple API to AWS.

### Option B: C/C++

Best if the training objective is strongly focused on embedded development.

For the AWS deployment portion, a thin Python REST wrapper may be added around the core logic if required.

---

# 12. Suggested Software Structure

A simple structure is recommended:

```text
bootloader-session-manager/
│
├── src/
│   ├── session_manager
│   ├── uds_handler
│   └── response_builder
│
├── tests/
│   ├── test_session_manager
│   ├── test_uds_handler
│   └── test_response_builder
│
├── docs/
│   └── architecture.md
│
├── .github/
│   └── workflows/
│       └── ci.yml
│
├── Dockerfile
├── README.md
└── requirements.txt
```

The structure may be adapted based on the selected programming language.

---

# 13. Non-Functional Requirements

## NFR-001: Maintainability

The implementation shall use clear module boundaries and meaningful names.

## NFR-002: Testability

The core logic shall be independently testable without requiring CAN hardware.

## NFR-003: Error Handling

Invalid input shall not cause an unhandled application crash.

## NFR-004: Logging

The application shall log:

- Received request
- Requested session
- Session transition
- Negative response reason

Example:

```text
INFO: Received UDS request: 10 02
INFO: Session transition: DEFAULT -> PROGRAMMING
INFO: Response: 50 02
```

## NFR-005: Code Quality

The code shall pass the selected linting/static-analysis checks.

## NFR-006: Test Coverage

The team shall target at least **80% unit-test coverage** for the core session-management logic.

---

# 14. Required Test Scenarios

The following minimum scenarios shall be implemented.

| ID | Test Scenario | Expected Result |
|---|---|---|
| TC-001 | Initialize application | Default session |
| TC-002 | Request Default Session | `50 01` |
| TC-003 | Request Programming Session | `50 02` |
| TC-004 | Request Extended Session | `50 03` |
| TC-005 | Unsupported session `0x05` | `7F 10 12` |
| TC-006 | Invalid SID `0x22` | `7F 22 11` |
| TC-007 | One-byte request `10` | `7F 10 13` |
| TC-008 | Empty request | Appropriate negative response |
| TC-009 | Session transition Default -> Programming | Programming session |
| TC-010 | Session transition Programming -> Extended | Extended session |
| TC-011 | Invalid request after valid session | Previous session remains unchanged |
| TC-012 | Malformed request with extra data | Defined and documented behavior |

---

# 15. GitHub Copilot Training Activities

The same MVP should be used throughout the training rather than creating separate examples for each Copilot capability.

## Stage 1: Requirement Understanding

### Training Goal

Use GitHub Copilot to understand the requirement and convert it into:

- Functional requirements
- Acceptance criteria
- Edge cases
- Test scenarios

### Expected Output

A developer should be able to ask Copilot questions such as:

```text
Analyze this requirement and identify:
1. Functional requirements
2. Non-functional requirements
3. Edge cases
4. Acceptance criteria
5. Unit-test scenarios
```

---

# 16. Stage 2: Requirement Design

### Training Goal

Convert the requirements into a simple technical design.

The design should identify:

- Modules
- APIs
- Data structures
- Session state model
- Request/response flow

### Expected Output

A short design document containing:

```text
Requirement
    ↓
UDS Handler
    ↓
Request Validator
    ↓
Session Manager
    ↓
Response Builder
```

---

# 17. Stage 3: Development

### Training Goal

Use GitHub Copilot to implement the MVP.

Suggested sequence:

1. Create project structure.
2. Define session enumeration/constants.
3. Implement request validation.
4. Implement session manager.
5. Implement response builder.
6. Implement UDS handler.
7. Add logging.
8. Create unit tests.

### Copilot Learning Topics

- Inline code completion
- Code generation
- Chat-based development
- Generating functions
- Generating unit tests
- Explaining unfamiliar code
- Fixing errors

---

# 18. Stage 4: Refactoring

### Training Goal

Use Copilot to improve an intentionally working implementation.

Potential refactoring exercises:

### Exercise A

Identify duplicated validation logic.

### Exercise B

Improve naming and readability.

### Exercise C

Separate UDS protocol handling from session-management logic.

### Exercise D

Improve error handling.

### Exercise E

Add type hints/documentation where applicable.

### Exercise F

Ask Copilot to identify possible design/code smells.

Expected outcome:

```text
Before:
Large process_request() function

After:
process_request()
    -> validate_request()
    -> determine_session()
    -> change_session()
    -> build_response()
```

---

# 19. Stage 5: Code Review

### Training Goal

Use GitHub Copilot to review a pull request or selected code.

The review should check:

- Functional correctness
- Edge cases
- Error handling
- Security concerns
- Maintainability
- Test coverage
- Coding standards
- Potential regressions

### Review Exercise

Intentionally introduce defects such as:

```text
1. Accept unsupported session 0x05.
2. Change session before validation.
3. Return 0x50 for an invalid request.
4. Forget to handle empty input.
5. Use duplicated response-generation logic.
```

Ask Copilot to identify the defects.

---

# 20. Stage 6: CI/CD

## Applicability

**Applicable**

This stage is useful for teaching the team how embedded/bootloader software quality checks can be automated.

The actual ECU flashing pipeline is outside this MVP.

### CI Pipeline

A GitHub Actions workflow should perform:

```text
Code Checkout
      ↓
Install Dependencies
      ↓
Static Analysis / Lint
      ↓
Unit Tests
      ↓
Coverage
      ↓
Build
      ↓
Docker Build
```

Pipeline shall fail when:

- Unit tests fail.
- Static analysis fails.
- Build fails.

---

# 21. Stage 7: AWS Deployment

## Applicability

**Applicable for training**

AWS deployment is not normally part of bootloader execution on an ECU, so the cloud component represents a **simulated diagnostic service**, not the actual bootloader.

### Suggested Deployment

Deploy the application as a small REST service.

Example:

```text
POST /uds/session
```

Request:

```json
{
  "request": "1002"
}
```

Response:

```json
{
  "response": "5002",
  "session": "PROGRAMMING"
}
```

### AWS Architecture

```text
GitHub
   |
   | GitHub Actions
   v
Container Build
   |
   v
Amazon ECR
   |
   v
AWS ECS / Fargate
   |
   v
REST API
   |
   v
UDS Session Manager
```

For a 16-hour training, **ECS/Fargate or AWS Lambda** can be selected based on the trainer's AWS coverage.

---

# 22. Stage Applicability Summary

| Lifecycle Stage | Applicable? | Training Activity |
|---|---|---|
| Requirement Understanding | Yes | Analyze UDS session requirements |
| Requirement Design | Yes | Architecture and API design |
| Development | Yes | Implement session manager |
| Unit Testing | Yes | Generate and execute tests |
| Refactoring | Yes | Improve code structure |
| Code Review | Yes | AI-assisted review |
| Integration with ECU Hardware | No | Out of MVP |
| CAN/CAN FD Communication | No | Simulated |
| AUTOSAR Integration | No | Out of MVP |
| HIL Testing | No | Out of MVP |
| CI | Yes | GitHub Actions |
| CD | Yes | Container deployment |
| AWS Deployment | Yes | Deploy simulated diagnostic service |
| ECU Flashing | No | Out of MVP |
| Production Release | No | Training only |

---

# 23. Proposed 16-Hour Training Plan

| Session | Duration | Topic | Hands-on |
|---|---:|---|---|
| 1 | 1.0 hr | GitHub Copilot overview and setup | Yes |
| 2 | 1.5 hr | Requirement understanding | Yes |
| 3 | 1.5 hr | Requirement to technical design | Yes |
| 4 | 3.0 hr | Development using Copilot | Yes |
| 5 | 1.5 hr | Unit testing and test generation | Yes |
| 6 | 1.5 hr | Refactoring with Copilot | Yes |
| 7 | 1.5 hr | Code review and defect detection | Yes |
| 8 | 1.5 hr | GitHub Actions CI/CD | Yes |
| 9 | 1.5 hr | Docker + AWS deployment | Yes |
| 10 | 1.0 hr | Final demo and lessons learned | Yes |
| **Total** | **16.0 hr** | | |

---

# 24. Definition of Done

The MVP is considered complete when:

- [ ] Project repository is created.
- [ ] Requirements are documented.
- [ ] Technical design is documented.
- [ ] `0x10` Diagnostic Session Control is implemented.
- [ ] Default Session is supported.
- [ ] Programming Session is supported.
- [ ] Extended Session is supported.
- [ ] Positive responses are generated.
- [ ] Negative responses are generated.
- [ ] Session state is maintained.
- [ ] Unit tests are implemented.
- [ ] Core logic achieves at least 80% coverage.
- [ ] Static analysis/lint passes.
- [ ] Code is refactored.
- [ ] Code review is performed with Copilot.
- [ ] GitHub Actions CI pipeline is working.
- [ ] Docker image is generated.
- [ ] Application is deployed to AWS.
- [ ] REST endpoint can process a sample UDS request.
- [ ] README contains setup and execution instructions.

---

# 25. Acceptance Criteria

### AC-001

Given the application is initialized, when the current session is requested, then the result shall be `DEFAULT_SESSION`.

### AC-002

Given a valid request `10 02`, when the request is processed, then the response shall be `50 02` and the current session shall become `PROGRAMMING_SESSION`.

### AC-003

Given a valid request `10 03`, when the request is processed, then the response shall be `50 03` and the current session shall become `EXTENDED_SESSION`.

### AC-004

Given an unsupported session `10 05`, when the request is processed, then the response shall be `7F 10 12` and the current session shall remain unchanged.

### AC-005

Given an unsupported service `22 01`, when the request is processed, then the response shall be `7F 22 11`.

### AC-006

Given an invalid-length request `10`, when the request is processed, then the response shall be `7F 10 13`.

### AC-007

Given any invalid request, the application shall not crash.

### AC-008

All automated tests shall pass before the CI pipeline succeeds.

### AC-009

The application shall be deployable as a container.

### AC-010

The deployed AWS service shall return the same functional result as the locally executed application for the defined test cases.

---

# 26. Suggested Copilot Demonstration Flow

The trainer can demonstrate the complete lifecycle using one requirement.

```text
Requirement
    ↓
"Explain this requirement"
    ↓
"Identify missing edge cases"
    ↓
"Create technical design"
    ↓
"Create project structure"
    ↓
"Implement the session manager"
    ↓
"Generate unit tests"
    ↓
"Run/fix failing tests"
    ↓
"Refactor this implementation"
    ↓
"Review this code for defects"
    ↓
"Generate CI workflow"
    ↓
"Create Dockerfile"
    ↓
"Prepare AWS deployment configuration"
    ↓
"Deploy"
```

This gives the trainees one continuous story rather than isolated Copilot demonstrations.

---

# 27. Optional Extension if Time Is Available

If the team completes the MVP early, add **UDS Tester Present (`0x3E`)**.

Supported request:

```text
3E 00
```

Positive response:

```text
7E 00
```

This can introduce:

- Multiple UDS services
- Service routing
- Common response handling
- Additional unit tests
- Refactoring opportunities

A second optional extension is to add a simplified session transition matrix.

Example:

| Current | Requested | Allowed |
|---|---|---|
| Default | Default | Yes |
| Default | Programming | Yes |
| Default | Extended | Yes |
| Programming | Default | Yes |
| Programming | Programming | Yes |
| Programming | Extended | Yes |
| Extended | Default | Yes |
| Extended | Programming | Yes |
| Extended | Extended | Yes |

---

# 28. Important Training Boundary

This project is a **training MVP**, not an AUTOSAR-compliant production bootloader component.

The objective is to preserve familiar bootloader concepts while reducing infrastructure dependencies.

The team should therefore focus on:

**Requirement → Design → Code → Test → Refactor → Review → CI/CD → Deployment**

rather than spending training time on:

**CAN → CAN TP → PduR → DCM → MCAL → ECU hardware → HIL → flashing**

Those production integrations can be mapped back to the MVP after the training.

---

# 29. Expected Final Demonstration

At the end of the training, the team should be able to demonstrate:

1. A GitHub repository containing the complete MVP.
2. A requirement and design document.
3. Copilot-assisted implementation.
4. Automated unit tests.
5. Copilot-assisted refactoring.
6. Copilot-assisted code review.
7. GitHub Actions CI pipeline.
8. Docker image creation.
9. AWS deployment.
10. A live API request such as:

```text
POST /uds/session

Request:
10 02

Response:
50 02
```

The final demonstration should show that the same small bootloader requirement can be taken through an end-to-end software lifecycle using GitHub Copilot.
