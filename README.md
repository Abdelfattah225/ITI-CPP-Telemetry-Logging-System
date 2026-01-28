# ITI-Telemetry-Logging-Sys

A scalable, multi-threaded logging system written in Modern C++17. Designed with best architectural and implementation practices in mind, utilizing the power of Modern C++ alongside industry-standard libraries and tools.

## Features

- ✅ Support for multiple local/remote data sources (File, Socket)
- ✅ Intuitive data formatting with threshold-based severity
- ✅ Support for multiple data storage strategies
- ✅ Extensible sink architecture using design patterns
- ✅ Thread-safe asynchronous logging operations
- ✅ High-performance ThreadPool for parallel sink writes
- ✅ RAII-compliant resource management
- 🔄 SOME/IP integration (Planned)
- 🔄 DLT logging support (Planned)

## System Overview

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                           DATA SOURCES (Phase 2)                             │
│   ┌───────────────┐   ┌───────────────┐   ┌───────────────┐                  │
│   │ CPU Telemetry │   │ RAM Telemetry │   │ GPU Telemetry │                  │
│   │   (File)      │   │   (File)      │   │   (Socket)    │                  │
│   └───────┬───────┘   └───────┬───────┘   └───────┬───────┘                  │
│           │                   │                   │                          │
│           └───────────────────┼───────────────────┘                          │
│                               │                                              │
│                               ▼                                              │
│   ┌──────────────────────────────────────────────────────────────────────┐   │
│   │                    LOG FORMATTER (Phase 3)                           │   │
│   │         Policy-based formatting with threshold severity              │   │
│   │              CpuPolicy | RamPolicy | GpuPolicy                       │   │
│   └──────────────────────────────┬───────────────────────────────────────┘   │
│                                  │                                           │
│                                  ▼                                           │
│   ┌──────────────────────────────────────────────────────────────────────┐   │
│   │                      LOG MESSAGE (Phase 1)                           │   │
│   │           (app_name, context, severity, payload, timestamp)          │   │
│   └──────────────────────────────┬───────────────────────────────────────┘   │
│                                  │                                           │
│                                  ▼                                           │
│   ┌──────────────────────────────────────────────────────────────────────┐   │
│   │                   ASYNC LOG MANAGER (Phase 4)                        │   │
│   │   ┌────────────────────────────────────────────────────────────┐     │   │
│   │   │              ThreadSafeRingBuffer<LogMessage>              │     │   │
│   │   │    ┌─────────────────────────────────────────────────┐     │     │   │
│   │   │    │  mutex + condition_variables + RingBuffer       │     │     │   │
│   │   │    └─────────────────────────────────────────────────┘     │     │   │
│   │   └────────────────────────────┬───────────────────────────────┘     │   │
│   │                                │                                     │   │
│   │                                ▼                                     │   │
│   │   ┌────────────────────────────────────────────────────────────┐     │   │
│   │   │                    THREAD POOL                             │     │   │
│   │   │         (Parallel sink writes for performance)             │     │   │
│   │   └────────────────────────────┬───────────────────────────────┘     │   │
│   └────────────────────────────────┼─────────────────────────────────────┘   │
│                                    │                                         │
│                    ┌───────────────┼───────────────┐                         │
│                    │               │               │                         │
│                    ▼               ▼               ▼                         │
│              ┌──────────┐    ┌──────────┐    ┌──────────┐                    │
│              │ Console  │    │   File   │    │  Socket  │                    │
│              │   Sink   │    │   Sink   │    │   Sink   │                    │
│              └──────────┘    └──────────┘    └──────────┘                    │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
```

## Project Phases

| Phase | Description | Status |
|-------|-------------|--------|
| [Phase 1](docs/01-Core-Synchronous-Logging-Foundation/README.md) | Core Synchronous Logging Foundation | ✅ Complete |
| [Phase 2](docs/02-DataSources&SmartResourceManagement/README.md) | Data Sources & Smart Resource Management | ✅ Complete |
| [Phase 3](docs/03-Formatter&ThresholdLogic/README.md) | Formatter & Threshold Logic | ✅ Complete |
| [Phase 4](docs/04-Asynchronous-Logging/README.md) | Asynchronous Logging with ThreadPool | ✅ Complete |
| Phase 5 | Integration with vsomeip and SOME/IP | 🔄 Planned |
| Phase 6 | DLT (Diagnostic Log and Trace) Integration | 🔄 Planned |
| Phase 7 | Performance Optimization and Benchmarking | 🔄 Planned |

## Topics Covered

### Modern C++17

| Topic | Phase | Description |
|-------|-------|-------------|
| Virtual Functions & Destructors | 1 | Dynamic dispatch for sink implementations |
| Operator Overloading | 1 | `operator<<` for LogMessage |
| Namespaces | 1 | Code organization |
| References | 2 | Parameter passing optimization |
| Move vs Copy Semantics | 2 | Efficient resource transfer |
| Rule of 3/5/0 | 2 | Special member functions |
| RAII | 2 | SafeFile, SafeSocket resource management |
| `enum class` | 3 | Type-safe enumerations |
| Templates | 3, 4 | LogFormatter, RingBuffer, ThreadSafeRingBuffer |
| `constexpr` & `if constexpr` | 3 | Compile-time computations |
| `std::optional` | 3, 4 | Nullable value types |
| `std::thread` | 4 | Multi-threading |
| `std::mutex` & `std::lock_guard` | 4 | Thread synchronization |
| `std::unique_lock` | 4 | Flexible locking |
| `std::condition_variable` | 4 | Thread coordination |
| `std::atomic` | 4 | Lock-free operations |
| `std::function` & Lambdas | 4 | Callables and task encapsulation |
| `std::future` & `std::packaged_task` | 4 | Asynchronous return values |
| Smart Pointers | All | `unique_ptr`, `shared_ptr` |

### Design Patterns

| Category | Pattern | Phase | Usage |
|----------|---------|-------|-------|
| Behavioral | Strategy | 1 | Sink selection |
| Creational | Factory | 3 | LogSinkFactory |
| Creational | Builder | 3 | LogManagerBuilder |
| Concurrency | Producer-Consumer | 4 | Async logging flow |
| Concurrency | Thread Pool | 4 | Parallel sink writes |
| Structural | RAII | 2, 4 | Resource management |

### Design Idioms

| Idiom | Phase | Description |
|-------|-------|-------------|
| RAII | 2 | Resource Acquisition Is Initialization |
| Rule of Zero | 2, 3 | Delegate resource management |
| Rule of Five | 2 | Custom resource management |
| Policy-Based Design | 3 | Template policies for formatting |

## Libraries & Tools

| Library | Purpose | Status |
|---------|---------|--------|
| [GoogleTest](https://github.com/google/googletest) | Unit testing | ✅ Used |
| [magic_enum](https://github.com/Neargye/magic_enum) | Enum reflection | ✅ Used |
| [Bazel](https://bazel.build/) | Build system | ✅ Used |
| [vsomeip](https://github.com/COVESA/vsomeip) | SOME/IP communication | 🔄 Planned |
| [DLT](https://github.com/COVESA/dlt-daemon) | Diagnostic Log and Trace | 🔄 Planned |
| [CommonAPI](https://github.com/COVESA/capicxx-core-runtime) | IPC abstraction | 🔄 TBD |

## Project Structure

```
ITI_Cpp_LogLog_Project/
├── MODULE.bazel
├── WORKSPACE
├── .bazelrc
├── README.md
│
├── inc/
│   ├── logging/                          # Phase 1
│   │   ├── BUILD.bazel
│   │   ├── ILogSink.hpp
│   │   ├── LogMessage.hpp
│   │   ├── LogManager.hpp
│   │   ├── ConsoleSinkImpl.hpp
│   │   └── FileSinkImpl.hpp
│   │
│   ├── SmartDataHub/                     # Phase 2
│   │   ├── BUILD.bazel
│   │   ├── ITelemetrySource.hpp
│   │   ├── FileTelemetrySourceImpl.hpp
│   │   ├── SocketTelemetrySourceImpl.hpp
│   │   ├── SafeFile.hpp
│   │   ├── SafeSocket.hpp
│   │   └── TelemetryParser.hpp
│   │
│   ├── Formatter/                        # Phase 3
│   │   ├── BUILD
│   │   ├── Enums.hpp
│   │   ├── Policies.hpp
│   │   ├── LogFormatter.hpp
│   │   ├── LogSinkFactory.hpp
│   │   ├── LogManagerBuilder.hpp
│   │   ├── Parser.hpp
│   │   └── magic_enum.hpp
│   │
│   └── AsyncLogging/                     # Phase 4
│       ├── BUILD
│       ├── RingBuffer.hpp
│       ├── ThreadSafeRingBuffer.hpp
│       ├── ThreadPool.hpp
│       └── AsyncLogManager.hpp
│
├── src/
│   ├── BUILD.bazel
│   ├── logging/                          # Phase 1
│   │   ├── ConsoleSinkImpl.cpp
│   │   ├── FileSinkImpl.cpp
│   │   └── LogManager.cpp
│   │
│   ├── SmartDataHub/                     # Phase 2
│   │   ├── FileTelemetrySourceImpl.cpp
│   │   ├── SocketTelemetrySourceImpl.cpp
│   │   ├── SafeFile.cpp
│   │   ├── SafeSocket.cpp
│   │   └── TelemetryParser.cpp
│   │
│   ├── Formatter/                        # Phase 3
│   │   ├── LogSinkFactory.cpp
│   │   └── LogManagerBuilder.cpp
│   │
│   └── AsyncLogging/                     # Phase 4
│       ├── ThreadPool.cpp
│       └── AsyncLogManager.cpp
│
├── app/
│   ├── phase1/
│   │   ├── BUILD.bazel
│   │   └── main.cpp
│   ├── phase2/
│   │   ├── BUILD.bazel
│   │   └── main.cpp
│   ├── phase3/
│   │   ├── BUILD
│   │   └── main.cpp
│   └── phase4/
│       ├── BUILD
│       ├── main.cpp
│       ├── test_threadsafe_buffer.cpp
│       ├── test_thread_pool.cpp
│       ├── test_async_log_manager.cpp
│       └── generate_telemetry.sh
│
├── Utest/
│   ├── phase1/
│   │   ├── BUILD.bazel
│   │   ├── LogMessageTest.cpp
│   │   ├── ConsoleSinkTest.cc
│   │   ├── FileSinkTest.cc
│   │   └── LogManagerTest.cc
│   ├── phase2/
│   │   ├── BUILD.bazel
│   │   ├── SafeFileTest.cc
│   │   ├── SafeSocketTest.cc
│   │   ├── FileTelemetrySourceImplTest.cc
│   │   └── SocketTelemetrySourceImplTest.cc
│   ├── phase3/
│   │   ├── BUILD
│   │   ├── Enum_test.cpp
│   │   ├── Policies_test.cpp
│   │   ├── LogFormatterTest.cpp
│   │   ├── LogSinkFactory_test.cpp
│   │   └── LogManagerBuilderTest.cpp
│   └── phase4/
│       ├── BUILD
│       ├── RingBufferTest.cpp
│       ├── ThreadSafeRingBufferTest.cpp
│       ├── ThreadPoolTest.cpp
│       └── AsyncLogManagerTest.cpp
│
└── docs/
    ├── 01-Core-Synchronous-Logging-Foundation/
    │   ├── README.md
    │   └── classes_diagrams.png
    ├── 02-DataSources&SmartResourceManagement/
    │   ├── README.md
    │   └── TelemetrySystem.png
    ├── 03-Formatter&ThresholdLogic/
    │   ├── README.md
    │   └── Phase3_Class_Diagram.png
    └── 04-Asynchronous-Logging/
        ├── README.md
        ├── uml.puml
        


## Phase Highlights

### Phase 1: Core Synchronous Logging Foundation
- LogMessage structure with severity levels
- ILogSink interface with Strategy pattern
- ConsoleSinkImpl and FileSinkImpl
- LogManager for routing messages

### Phase 2: Data Sources & Smart Resource Management
- ITelemetrySource interface
- RAII wrappers: SafeFile, SafeSocket
- File and Socket telemetry sources
- Rule of 5 implementation

### Phase 3: Formatter & Threshold Logic
- Policy-based LogFormatter template
- Threshold-based severity inference
- Factory pattern for sink creation
- Builder pattern for LogManager

### Phase 4: Asynchronous Logging
- RingBuffer<T>: Lock-free circular buffer
- ThreadSafeRingBuffer<T>: Thread-safe wrapper
- ThreadPool: Parallel task execution
- AsyncLogManager: Non-blocking logging


## Performance Characteristics

| Operation | Complexity | Notes |
|-----------|------------|-------|
| log() | O(1) | Non-blocking, push to buffer |
| RingBuffer push/pop | O(1) | Circular buffer operations |
| ThreadPool enqueue | O(1) | Amortized, queue insertion |
| Sink write | O(n) | Parallel via ThreadPool |

## Upcoming Features (Phase 5+)

- [ ] SOME/IP integration with vsomeip
- [ ] Remote logging capabilities
- [ ] DLT (Diagnostic Log and Trace) support
- [ ] Log filtering and routing
- [ ] Performance benchmarking suite
- [ ] Configuration file support

## Author

**Abdelfattah Moawed**

