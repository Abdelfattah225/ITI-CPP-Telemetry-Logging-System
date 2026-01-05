# Phase 1 Unit Tests

Unit tests for the Core Synchronous Logging Foundation using Google Test framework.

## Test Files

| File | Class Tested | Tests |
|------|--------------|-------|
| LogMessageTest.cpp | LogMessage | 7 tests |
| ConsoleSinkTest.cc | ConsoleSinkImpl | 3 tests |
| FileSinkTest.cc | FileSinkImpl | 3 tests |
| LogManagerTest.cc | LogManager | 8 tests |

## Test Coverage

### LogMessageTest

| Test Name | Description |
|-----------|-------------|
| ConstructorSetAppName | Verify app_name is set correctly |
| ConstructorSetContext | Verify context is set correctly |
| SeverityIsInfoWhenPayloadLow | Payload <= 25 sets severity to INFO |
| SeverityIsWarnWhenPayloadMedium | Payload 26-74 sets severity to WARN |
| SeverityIsCriticalWhenPayloadHigh | Payload >= 75 sets severity to CRITICAL |
| GetTextReturnsFormattedString | getText() returns formatted output |
| StreamOperatorWorks | operator<< outputs correctly |

### ConsoleSinkTest

| Test Name | Description |
|-----------|-------------|
| CanBeCreated | ConsoleSinkImpl instantiates successfully |
| WriteOutputsToConsole | write() outputs to stdout |
| ConsoleSinkInheritsFromILogSink | Polymorphism works via ILogSink pointer |

### FileSinkTest

| Test Name | Description |
|-----------|-------------|
| WritesToFile | write() creates and writes to file |
| AppendsMultipleMessages | Multiple messages append to same file |
| FileSinkInheritsFromILogSink | Polymorphism works via ILogSink pointer |

### LogManagerTest

| Test Name | Description |
|-----------|-------------|
| CanBeCreated | LogManager instantiates successfully |
| CanAddSink | addSink() accepts unique_ptr sink |
| CanAddMultipleSinks | Multiple sinks can be added |
| CanLogMessage | log() accepts LogMessage |
| FlushWritesToConsoleSink | flush() writes to console sink |
| FlushWritesToFileSink | flush() writes to file sink |
| FlushWritesToAllSinks | flush() writes to all registered sinks |
| FlushClearsBuffer | Buffer is cleared after flush() |

## Build and Run

```bash
# Build from project root
cmake -S . -B build
cmake --build build

# Run all tests
./build/test/phase1/phase1_tests

# Run with verbose output
./build/test/phase1/phase1_tests --gtest_output=xml:test_results.xml

# Run specific test suite
./build/test/phase1/phase1_tests --gtest_filter=LogMessageTest.*

# Run specific test
./build/test/phase1/phase1_tests --gtest_filter=LogManagerTest.FlushWritesToAllSinks
```

## Test Results

```
[==========] Running 21 tests from 4 test suites.
[  PASSED  ] 21 tests.
```

## Dependencies

- Google Test (fetched automatically via CMake FetchContent)
```

---

Save this as `test/phase1/README.md`:

```bash
nano test/phase1/README.md
```

---

Simple and documents everything needed for the tests.