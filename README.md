# Traffic Signal System

This project implements a Traffic Signal System using the STM32F4 microcontroller and FreeRTOS. It is designed to simulate and control traffic lights, manage traffic flow, and provide a platform for embedded systems development and experimentation.

## Features
- Traffic light control logic
- Traffic flow simulation and management
- Signal display and hardware abstraction
- Real-time task scheduling using FreeRTOS


## Directory Structure
- `src/` - Main application source code (traffic logic, hardware abstraction, FreeRTOS config)
- `FreeRTOS_Source/` - FreeRTOS kernel source and headers
- `Libraries/` - STM32F4xx and CMSIS peripheral drivers and headers
- `Utilities/` - STM32F4-Discovery board utilities and drivers

## Main Components
- **main.c**: Entry point, system initialization, and main loop
- **trafficlights.c/h**: Traffic light state machine and control logic
- **trafficflow.c/h**: Traffic flow simulation and management
- **signal_display.c/h**: LED and display control for signals
- **hardware.c/h**: Hardware abstraction layer
- **FreeRTOSConfig.h**: FreeRTOS configuration

