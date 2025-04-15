# Smart Suit for Industrial Workers

A **comprehensive monitoring system** designed to enhance **industrial worker safety** and **environmental awareness** through real-time sensor data, control mechanisms, and visual analytics.

---

##  Overview

The **Smart Suit for Industrial Workers** system integrates **hardware sensors** with **software analytics** to monitor environmental and health parameters in industrial settings. This repository includes two distinct simulation approaches to develop and test the smart suit functionalities.

---

##  Features

- ✅ Real-time **gas concentration monitoring** (MQ135 sensor)
- ✅ **Motion and position tracking** (Hall effect sensor)
- ✅ **Environmental condition simulation**
- ✅ **Data visualization and analytics**
- ✅ **Modular design** for extensibility and scalability

---

##  Simulation Options

### 1.  Proteus Simulation

**Hardware-level simulation** using Arduino and Proteus Design Suite:

- Simulates physical components and sensor behavior
- Includes:
  - **MQ135** and **Hall effect sensor** components
  - Required libraries for Proteus
  - Arduino `.hex` firmware for logic
- Outputs real-time data to the **Virtual Terminal**
- Sensor data can be captured and analyzed
- Visualized using the provided **Python script**

---

### 2.  C Module Simulation

**Software-level modular simulation** with socket-based communication:

- Divided into 4 independent, interconnected modules:
  1. **Environment**: Simulates industrial conditions
  2. **Sensor**: Collects data from the environment
  3. **Control**: Processes sensor data and makes decisions
  4. **Actuator**: Executes responses based on control signals
- Modules communicate over **sockets**
- Logs are generated for analysis
- Supports **Python-based data visualization**

> **Run Order:** `Environment → Sensor → Control → Actuator`

---

## Getting Started

###Prerequisites

- **Proteus** Design Suite (for hardware simulation)
- **Python 3.6+** with the following packages:
  - `matplotlib`
  - `seaborn`
- **C runtime environment** (for executing C simulation modules)

## License

Distributed under the MIT License.

---
