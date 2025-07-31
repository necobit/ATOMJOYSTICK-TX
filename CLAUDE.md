# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a PlatformIO project for creating an ESP-NOW transmitter using M5Stack ATOM S3. The project broadcasts incrementing counter values wirelessly.

**Target Hardware:** M5Stack ATOM S3
**Framework:** Arduino
**Platform:** Espressif ESP32

## Development Commands

### Build and Upload
- `pio run` - Build the project
- `pio run -t upload` - Build and upload to device
- `pio run -t clean` - Clean build files

### Monitoring and Debugging
- `pio device monitor` - Open serial monitor
- `pio run -t monitor` - Build, upload, and monitor in one command

### Testing
- `pio test` - Run unit tests

### Static Analysis
- `pio check` - Run static code analysis

## Project Structure

- `src/main.cpp` - Main application code (currently template)
- `platformio.ini` - PlatformIO configuration targeting m5stack-atoms3 board
- `include/` - Project header files
- `lib/` - Project-specific libraries
- `test/` - Unit tests

## Project Goal (from README)

Create an ESP-NOW transmitter using M5Stack ATOM S3 that broadcasts incrementing counter values. The current main.cpp contains only template code and needs to be implemented with ESP-NOW functionality.