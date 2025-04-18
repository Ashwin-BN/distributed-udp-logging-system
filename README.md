# Distributed UDP Logging System

A UNIX-based logging system written in C++ that simulates an embedded environment for real-time log transmission and remote log management using UDP sockets. This project is designed for debugging purposes post-deployment and includes a menu-driven log server for centralized log storage, filtering, and display.

## 📌 Project Overview

This project focuses on creating an embedded logging system to simulate field usage for debugging after deployment. Logs are enriched with metadata such as severity, filename, function name, and line number, and are transmitted to a centralized server over UDP. The server collects and manages these logs efficiently, providing options for filtering and live screen output.

## 📁 Project Structure

```
├── client
│   ├── Automobile.cpp          # Test case for logging within a simulation
│   ├── Automobile.h
│   ├── Logger.cpp              # Core logger implementation
│   ├── Logger.h
│   ├── Makefile
│   ├── TravelSimulator.cpp     # Main simulator file
├── server
│   ├── LogServer.cpp           # UDP-based log server
│   ├── Makefile
│   └── server.log              # Centralized log file
```

## ✅ Key Features

### 🔹 Product Logging (Client Side)
- Logs include:
  - Severity level
  - Source file name
  - Function name
  - Line number
  - Log message
- Supported log levels:
  - `DEBUG`
  - `WARNING`
  - `ERROR`
  - `CRITICAL`
- Lightweight logging over UDP to minimize latency

### 🔸 Server Logging
- Maintains a `server.log` file
- Accepts UDP messages from client systems
- Interactive menu interface:
  1. Change log severity filter level
  2. Dump logs to the screen
  3. Graceful shutdown of the server

### ⚙️ Technical Highlights
- Logger API:
  - `InitializeLog()`
  - `SetLogLevel()`
  - `Log()`
  - `ExitLog()`
- Uses:
  - Non-blocking UDP sockets
  - POSIX threads & mutexes
  - Server control via commands (e.g., `Set Log Level=x`)

## 🚀 Getting Started

### 🛠 Prerequisites
- GCC / G++ (C++ compiler)
- POSIX-compliant OS (Linux/macOS)
- `make`

### 🔧 Build Instructions

#### Client
```bash
cd client
make all
./travel  # Start the simulator
```

#### Server
```bash
cd server
make all
./logserver  # Start the UDP log server
```

## 🧪 Testing

The `TravelSimulator.cpp` file simulates multiple cars driving in city and highway conditions, generating logs at different severity levels. Monitor `server.log` or the server screen to observe real-time logging and filtering.


## 📸 Outputs

[Click here to view output screenshots](./Output/output_screenshots.md)

## 👨‍💻 Author

**Ashwin B N**  
[GitHub Profile](https://github.com/Ashwin-BN)  
Email: ashwinbnwork@gmail.com
