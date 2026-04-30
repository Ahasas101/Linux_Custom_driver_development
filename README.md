# Linux Custom Driver and Systems Development

This repository documents the development of a custom Embedded Linux system and the implementation of various Linux Kernel Modules (LKMs) on the BeagleBone Black (AM335x) platform. The project covers the entire stack from the bootloader to custom platform drivers.

---

## Project Structure

### /Sources
This directory contains the core system components cross-compiled for the ARM architecture:
* **U-Boot**: Custom compiled bootloader including MLO and SPL for initial hardware initialization.
* **Linux Kernel**: Custom kernel image compiled with specific configurations for the BeagleBone Black.
* **File System (RootFS)**: A custom-built root filesystem generated using Buildroot/BusyBox to manage system utilities and libraries.

### /Custom_Drivers
This section contains various Linux Kernel Modules (LKMs) demonstrating driver development and kernel synchronization:

#### 1. Pseudo Character Drivers
* **Single Device**: A basic character driver implementing standard file operations (open, read, write, llseek, release) for a single device node.
* **Multiple Devices**: An expanded driver managing multiple minor devices under a single major number, utilizing private data structures for each device instance.

#### 2. Platform Drivers
* **Platform Character Device**: A module that defines and registers platform-specific device data, simulating hardware resources like memory and IRQs.
* **Platform Driver**: A robust driver implementation that follows the Linux Device Model, utilizing probe and remove methods to manage devices registered on the Platform Bus.

---

## System Outcomes & Features

* **Boot Process**: Deep understanding of the ROM -> U-Boot -> Kernel boot sequence on Linux-ARM systems.
* **Device Tree (DTS)**: Implementation of Device Tree nodes to define hardware topology and handle device instantiation.
* **Kernel Synchronization**: Proper usage of Mutexes and Spinlocks to prevent race conditions during concurrent device access.
* **Booting Methods**: Support for multiple booting strategies including TFTP, NFS, and SD card (MMC) booting.

---

## Tools & Environment
* **Hardware**: BeagleBone Black (AM335x ARM Cortex-A8).
* **Toolchain**: ARM-Linux-Gueabihf cross-compiler.
* **Debugging**: GDB, I2C-tools, and Serial (UART) console monitoring.

---

## About Me
**Ahasas Yadav** Undergraduate at Indian Institute of Information Technology (IIIT), Una.  
Specializing in Embedded Systems, Firmware, and Embedded Linux.
