---
name: uds-diagnostic-session-manager
description: 'Design, implement, refactor, and unit test the automotive bootloader UDS Diagnostic Session Manager (UDS service 0x10 Diagnostic Session Control: Default/Programming/Extended session validation, transitions, positive response and NRC generation). Use when working on session_manager, uds_handler, response_builder, or constants modules, or when writing unit tests, CI checks, or AWS deployment steps for this component.'
argument-hint: 'Describe the task (implement session logic, add validation rule, generate response, write unit tests, fix CI/deployment) and the target module.'
---

# UDS Diagnostic Session Manager

## When to Use
- Implementing or modifying UDS service `0x10` (Diagnostic Session Control) logic.
- Adding/validating session transition rules for Default, Programming, or Extended Diagnostic Session.
- Generating positive responses or Negative Response Codes (NRCs) for invalid requests.
- Writing or updating unit tests for `session_manager`, `uds_handler`, or `response_builder`.
- Reviewing changes for compliance with the MVP scope, CI pipeline, or containerized AWS deployment.

Reference documents: [Bootloader_GitHub_Copilot_MVP_Requirement.md](../../../Bootloader_GitHub_Copilot_MVP_Requirement.md), [SDD_Diagnostic_Session_Manager.md](../../../SDD_Diagnostic_Session_Manager.md).

## Module Structure
| Module | Responsibility | Dependencies |
|--------|-----------------|--------------|
| `session_manager` | Core session state, validation, transitions | None (pure logic) |
| `uds_handler` | UDS protocol handling and request routing | `session_manager` |
| `response_builder` | Positive/negative response generation | None (utility) |
| `constants` | UDS constants, service IDs, NRCs | None |

## Procedure
1. Identify the target module and confirm the change stays within its single responsibility (keep `session_manager` free of I/O/hardware/network code).
2. Validate the requested session against current session state and the allowed transition rules in the SDD.
3. On success, update session state in `session_manager` and build a positive response via `response_builder`.
4. On failure, return the correct NRC (e.g., subFunctionNotSupported, conditionsNotCorrect) via `response_builder`, using `constants` for IDs.
5. Add/update unit tests using the test case design table (testcase ID, name, description, input, type, expected output) — see the [ut-test-design prompt](../../Instructions/prompt/ut-test-design.prompt.md). Target ≥80% coverage including valid transitions, invalid sessions, and boundary/error cases.
6. Verify static/code-quality checks pass and the change is compatible with the GitHub Actions CI pipeline and containerized AWS deployment.

## Out of Scope
Do not introduce: real CAN/CAN FD communication, AUTOSAR BSW/PduR/DCM/CanTp integration, real ECU hardware, flash erase/programming, SecurityAccess (`0x27`), RequestDownload (`0x34`), TransferData (`0x36`), RequestTransferExit (`0x37`), authentication, HSM integration, watchdog handling, or NVM persistence.
