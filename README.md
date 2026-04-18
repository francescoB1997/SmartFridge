# 🧊 SmartFridge

Embedded smart fridge controller built on ESP platform, featuring temperature sensing, closed-loop control, and a real-time web interface powered by WebSocket.

---

## 📖 Overview

SmartFridge is an embedded controller designed to manage a cooling/heating system based on a Peltier module.

The system continuously monitors the internal temperature and automatically adjusts its behavior (cooling, heating, or idle) according to a configurable setpoint.

A built-in web interface allows real-time monitoring and control via WebSocket, without requiring page refresh.

---

## ⚙️ Features

- 🌡️ **Temperature monitoring**
  - Digital sensor support (e.g. DS18B20)
  - Continuous readings with real-time updates

- ❄️🔥 **Cooling / Heating control**
  - Operating modes:
    - OFF
    - COOL
    - HEAT
  - Automatic control based on setpoint

- 🧠 **Automatic mode**
  - Hysteresis-based control (configurable offset)
  - Dynamic Peltier regulation

- 🌬️ **Fan management**
  - Independent automatic modes for:
    - cooling
    - heating
  - Configurable timing (on/off + intervals)

- 🌐 **Web interface**
  - Browser-accessible dashboard
  - Remote configuration and control

- ⚡ **Real-time updates**
  - WebSocket-based communication
  - No polling required

- 📡 **WiFi management**
  - Automatic connection to saved network
  - Fallback Access Point mode for setup

- 🔄 **OTA updates**
  - Over-the-air firmware updates

- 💾 **Persistent configuration**
  - Stores:
    - setpoint
    - mode
    - offset
    - fan settings

---

## 🧱 Architecture

The project is designed with a modular architecture to separate application logic from hardware handling:

- **Hardware abstraction layer**
  - Relay / actuator management
  - Peltier and fan control

- **Sensor layer**
  - Temperature acquisition interface

- **Control logic**
  - Mode handling (OFF / COOL / HEAT)
  - Automatic temperature-based behavior

- **Networking layer**
  - WiFi (station + AP fallback)
  - Web server

- **Communication layer**
  - WebSocket for real-time synchronization

---

## 🌿 Development Branches

Different hardware control strategies are being developed in separate branches:

- `feature/relay-driver`
  - Uses mechanical relays for Peltier and fan control
  - Simple ON/OFF logic (time-window based)

- `feature/bts7960-driver`
  - Uses BTS7960 high-power driver
  - Enables more advanced control and potential bidirectional operation (heating/cooling)

This separation allows experimenting with different hardware approaches while keeping the main branch stable.

---

## 🖥️ Web Interface

The web interface allows you to:

- View current temperature
- Set target temperature
- Change operating mode (OFF / COOL / HEAT)
- Enable/disable automatic mode
- Configure:
  - temperature offset
  - fan behavior
  - timing parameters

All updates are pushed in real time via WebSocket.

---

## 🔌 Hardware (concept)

The system is designed to drive:

- Peltier module (cooling/heating)
- Cooling fans
- Power drivers (relay modules or H-bridge)

The architecture is flexible and can be adapted to different hardware implementations.

---

## 🚀 Use Cases

- DIY smart fridge
- Fermentation chamber
- Temperature-controlled enclosures
- Embedded thermal control systems

---

## 📌 Notes

The project is designed to be:
- modular
- extensible
- easily adaptable to different thermal control applications