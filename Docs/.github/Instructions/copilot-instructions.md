# GitHub Copilot Instructions for Bootloader Development

## Overview
This file contains comprehensive instructions and guidelines for using GitHub Copilot effectively within this embedded systems bootloader project. It establishes standards, best practices, and workflows to ensure consistent, high-quality code generation and integration with the project's technical requirements.

## Project Description
This project develops a complete bootloader implementation with a focus on efficient hardware initialization, low-level system management, and bootloader architecture design. The project aims to create robust, optimized bootloader code that follows industry best practices and provides clear guidelines for GitHub Copilot usage throughout development.

## Technology Stack

### Programming Language
- **Embedded C**: Core language for bootloader development
  - C99/C11 standard compliance required
  - Hardware-specific optimizations and direct register manipulation
  - Minimal dependencies and maximum performance

### Development Tools
- **GitHub Copilot**: AI-assisted code generation and documentation
- **Git**: Version control and collaboration
- **Compiler**: GCC (ARM-specific toolchain for embedded targets)
- **Debugger**: GDB or platform-specific debuggers
- **Version Control**: Git with GitHub

## Code Guidelines for Embedded C

### 1. Code Style and Formatting
- Use consistent 4-space indentation (no tabs)
- Maximum line length: 100 characters for readability
- Follow MISRA-C guidelines where applicable
- Use snake_case for variables and functions
- Use UPPER_CASE for macros and constants
- Opening braces on the same line (1TBS style): `if (condition) {`

### 2. Naming Conventions
- Use descriptive, meaningful names: `uint32_t boot_status` instead of `bs`
- Prefix static variables and functions with underscore: `static void _init_uart(void)`
- Use descriptive type names for custom types: `typedef struct { ... } boot_config_t`

### 3. Memory Management
- Declare memory usage explicitly in comments
- Avoid dynamic memory allocation (malloc/free) in bootloader
- Pre-allocate all memory at compile-time
- Document memory layout and sections clearly

### 4. Hardware-Specific Code
- Isolate hardware abstraction layer (HAL) in separate files
- Use clear comments to explain hardware-specific operations
- Include register addresses and bit field definitions
- Add timing comments for critical operations

### 5. Error Handling
- Always check return values and status codes
- Define clear error codes or enums for failures
- Implement proper error recovery mechanisms
- Log or indicate errors through appropriate channels

### 6. Comments and Documentation
- Add comments explaining "why", not just "what"
- Document register usage and bit fields
- Include function headers with parameters and return values
- Explain initialization sequences and state transitions
- Add TODO/FIXME comments for future improvements

### 7. Security Considerations
- Validate all inputs before processing
- Implement bounds checking for arrays and buffers
- Avoid buffer overflows and stack overflows
- Clear sensitive data when no longer needed

### 8. Performance and Optimization
- Minimize boot time for critical paths
- Use inline functions for small, frequently-called functions
- Optimize interrupt handlers for speed
- Document performance-critical sections

## GitHub Copilot Usage Guidelines

### When Using Copilot
1. **Provide Context**: Always include comments and context before requesting code generation
2. **Specify Requirements**: Be explicit about hardware platform, constraints, and requirements
3. **Verify Suggestions**: Always review generated code for correctness and compliance
4. **Test Thoroughly**: Validate generated code on actual hardware or simulators
5. **Refine Iteratively**: If suggestions don't meet requirements, refine your prompts

### Copilot Prompt Best Practices
- Include relevant context about the embedded system and hardware
- Specify error handling requirements
- Mention performance constraints or timing requirements
- Reference relevant standards (MISRA-C, AUTOSAR, etc.)
- Ask for code with comments and documentation

### Code Review Process
- All Copilot-generated code must be reviewed by team members
- Verify functionality against bootloader specifications
- Check for memory safety and security issues
- Ensure compliance with project coding standards
- Test on target hardware before merging

## Project-Specific Instructions

### Version Control
- Create feature branches for new bootloader features
- Use descriptive commit messages explaining changes
- Reference issue numbers when applicable
- Keep commits atomic and logically separated

### Documentation
- Maintain README files in each subsystem directory
- Document bootloader flow diagrams and state machines
- Keep architecture documentation current
- Document any hardware-specific workarounds

### Testing and Validation
- Test bootloader on target hardware
- Verify boot sequence and timing
- Test error recovery paths
- Document test procedures and results

## Team Collaboration
- Share Copilot prompts and techniques with team members
- Discuss generated code quality and improvements
- Contribute to continuous improvement of these guidelines
- Participate in code reviews with focus on Copilot-generated content

## Continuous Improvement
- Regularly review and update these instructions
- Gather feedback from team members on Copilot effectiveness
- Share best practices discovered during development
- Adapt guidelines based on project experience and lessons learned
- Evaluate new Copilot features for bootloader development applicability