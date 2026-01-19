# ITI-Telemetry-Logging-Sys

A scalable, multi-threaded logging system written in Modern C++17. Designed with best architectural and implementation practices in mind, utilizing the power of Modern C++ alongside industry-standard libraries and tools.

## Features

- Support for multiple local/remote sources of data
- Intuitive data formatting
- Support for multiple data storage strategies
- Extensible sink architecture using design patterns
- Thread-safe logging operations

## System Overview

```
    ┌─────────────────────────────────────────────────────────────────┐
    │                        Data Sources                             │
    │              (CPU, GPU, RAM Telemetry Data)                     │
    └─────────────────────────┬───────────────────────────────────────┘
                              │
                              v
    ┌─────────────────────────────────────────────────────────────────┐
    │                       LogMessage                                │
    │         (app_name, context, severity, payload, time)            │
    └─────────────────────────┬───────────────────────────────────────┘
                              │
                              v
    ┌─────────────────────────────────────────────────────────────────┐
    │                       LogManager                                │
    │                (Coordinator / Router)                           │
    └─────────────────────────┬───────────────────────────────────────┘
                              │
              ┌───────────────┼───────────────┐
              │               │               │
              v               v               v
        ┌──────────┐    ┌──────────┐    ┌──ongoing─┐
        │ Console  │    │   File   │    │ Network  │
        │   Sink   │    │   Sink   │    │   Sink   │
        └──────────┘    └──────────┘    └──────────┘
```

## Project Phases

| Phase | Description | Status |
|-------|-------------|--------|
| Phase 1 | Core Synchronous Logging Foundation | Complete |
| Phase 2 | Asynchronous Logging with Threading | Pending |
| Phase 3 | Network Sink Implementation | Pending |
| Phase 4 | Log Filtering and Advanced Formatting | Pending |
| Phase 5 | Integration with vsomeip and DLT | Pending |
| Phase 6 | Performance Optimization and Benchmarking | Pending |

## Topics Covered

### Modern C++

- Dynamic Dispatch (Virtual Functions + Virtual Destructors)
- Casting (Static and Dynamic + RTTI)
- Copy Semantics
- Move Semantics and Ownership Transfer
- Threading, Concurrency, and Callables
- Smart Pointers
- Special Member Functions and Rules of 0/3/5
- Templates and Generic Programming

### Design Patterns

| Category | Patterns |
|----------|----------|
| Creational | Factory, Builder, Singleton |
| Structural | Proxy, Adapter, Facade |
| Behavioral | Strategy, Observer |
| Concurrency | Thread Pool |

### Libraries

| Library | Purpose | Status |
|---------|---------|--------|
| vsomeip | SOME/IP communication | Planned |
| magic_enum | Enum reflection | Planned |
| CommonAPI | IPC abstraction | TBD |
| CommonAPI-SomeIP | SOME/IP binding | TBD |
| dlt | Diagnostic Log and Trace | TBD |

## Project Structure so far...

```
project/
├── CMakeLists.txt
├── README.md
├── inc/
│   └── logging/
│       ├── LogMessage.hpp
│       ├── ILogSink.hpp
│       ├── ConsoleSinkImpl.hpp
│       ├── FileSinkImpl.hpp
│       └── LogManager.hpp
├── src/
│   ├── CMakeLists.txt
│   └── logging/
│       ├── ConsoleSinkImpl.cpp
│       ├── FileSinkImpl.cpp
│       └── LogManager.cpp
├── app/
│   ├── CMakeLists.txt
│   └── main.cpp
├── test/phase1
│   ├── CMakeLists.txt
│   └── phase1.cpp
└── docs/
    └── phase1/
        └── README.md
```

## Phase Documentation

- [Phase 1: Core Synchronous Logging Foundation](docs/01-Core-Synchronous-Logging-Foundation/README.md)
- [Phase 2: Data Sources and Smart Resource Management](docs/02-DataSources&SmartResourceManagement)


#

## Author
**Abdelfattah Moawed**
