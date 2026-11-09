# Arduino Robotics Projects

A collection of mobile robotics projects developed during an engineering internship at **Robokart**. This repository contains modular Arduino implementations for an ultrasonic distance-tracking robot, a Bluetooth-controlled teleoperated robot, and an autonomous timer-controlled robot using two-wheel differential drive platforms.

---

## Table of Contents

- [Repository Overview](#repository-overview)
- [Repository Structure](#repository-structure)
- [Hardware Architecture & Pin Configurations](#hardware-architecture--pin-configurations)
  - [Shared Hardware Components](#shared-hardware-components)
  - [Pin Allocation Comparison Across Robots](#pin-allocation-comparison-across-robots)
- [Individual Project Details](#individual-project-details)
  - [1. Object Following Robot](#1-object-following-robot)
  - [2. Bluetooth Controlled Robot](#2-bluetooth-controlled-robot)
  - [3. Timer Controlled Robot](#3-timer-controlled-robot)
- [Technologies and Tools Used](#technologies-and-tools-used)
- [Skills Gained](#skills-gained)
- [Future Improvements](#future-improvements)

---

## Repository Overview

This repository documents three Arduino robotics projects built using the **Arduino UNO** and the **L293D dual H-bridge motor driver**:

1. **Object Following Robot:** Detects obstacles and maintains a following distance using an HC-SR04 ultrasonic distance sensor.
2. **Bluetooth Controlled Robot:** Teleoperated over wireless serial communication via an HC-05 module with differential rotation.
3. **Timer Controlled Robot:** Executes an autonomous, predetermined 5-phase movement sequence using time-based delays.

---

## Repository Structure

```
bots/
│
├── README.md
│
├── Object-Following-Robot/
│   └── object-following-robot.ino
│
├── Bluetooth-Controlled-Robot/
│   └── bluetooth-robot.ino
│
└── Timer-Controlled-Robot/
    └── timer-robot.ino
```

---

## Hardware Architecture & Pin Configurations

### Shared Hardware Components

All three robots share the same fundamental embedded hardware platform:

| Component | Specification | Function |
| :--- | :--- | :--- |
| **Arduino UNO** | Microchip ATmega328P microcontroller (16 MHz, 5V logic) | Central processing and I/O controller |
| **L293D Motor Driver** | Dual H-bridge driver IC / board | Drives two DC motors with bidirectional control |
| **DC Gear Motors** | 2 × BO geared DC motors | Left and right drive wheels (differential locomotion) |
| **Chassis** | 2-wheel drive chassis + 1 passive caster wheel | Physical robotics chassis |
| **Power Supply** | Battery pack | DC power source for microcontroller and motors |

---

### Pin Allocation Comparison Across Robots

Each robot utilizes a distinct pin configuration tailored to the specific sensors and communication modules connected to the Arduino UNO. 

The table below outlines the exact pin assignments defined in each project's source code:

| Arduino Uno Pin | Object Following Robot | Bluetooth Controlled Robot | Timer Controlled Robot |
| :---: | :--- | :--- | :--- |
| **D2** | Ultrasonic `TRIG_PIN` | Bluetooth SoftwareSerial RX | — |
| **D3** | Ultrasonic `ECHO_PIN` | Bluetooth SoftwareSerial TX | — |
| **D5** | — | Left Motor Enable (`ENA` - PWM) | Left Motor Enable (`ENA` - PWM) |
| **D6** | Left Motor Input 1 (`IN1`) | Right Motor Enable (`ENB` - PWM) | Right Motor Enable (`ENB` - PWM) |
| **D7** | Left Motor Input 2 (`IN2`) | — | — |
| **D8** | — | Left Motor Input 1 (`IN1`) | Left Motor Input 1 (`IN1`) |
| **D9** | — | Left Motor Input 2 (`IN2`) | Left Motor Input 2 (`IN2`) |
| **D10** | Right Motor Input 1 (`IN3`) | Right Motor Input 1 (`IN3`) | Right Motor Input 1 (`IN3`) |
| **D11** | Right Motor Input 2 (`IN4`) | Right Motor Input 2 (`IN4`) | Right Motor Input 2 (`IN4`) |

> **Key Pinout Differences:**
> * **Object Following Robot:** Pins **D2** and **D3** are dedicated to the ultrasonic sensor. The motor driver direction inputs are connected to digital pins **D6, D7, D10, and D11**. Enable pins (`ENA`/`ENB`) are not driven via software.
> * **Bluetooth Controlled Robot:** Pins **D2** and **D3** serve as the SoftwareSerial UART interface for the HC-05 module. Motor speed is actively controlled via PWM on pins **D5** and **D6**, and direction is driven via pins **D8, D9, D10, and D11**.
> * **Timer Controlled Robot:** Shares the exact motor driver pinout as the Bluetooth robot (**D5, D6, D8, D9, D10, D11**), requiring no external sensor or communication pins.

---

## Individual Project Details

### 1. Object Following Robot

* **Source File:** [`object-following-robot.ino`](./Object-Following-Robot/object-following-robot.ino)
* **Objective:** Maintain a configured distance range behind an object detected along the forward path using ultrasonic acoustic ranging.

#### Hardware Components
* Arduino UNO
* L293D Motor Driver
* HC-SR04 Ultrasonic Distance Sensor
* 2 × DC Gear Motors + Chassis
* Battery power source

#### Exact Pin Connections (from Source Code)

| Subsystem | Signal Name | Arduino Uno Pin | Description |
| :--- | :--- | :--- | :--- |
| **HC-SR04 Sensor** | `TRIG_PIN` | **D2** | Ultrasonic pulse trigger output |
| **HC-SR04 Sensor** | `ECHO_PIN` | **D3** | Ultrasonic echo pulse input capture |
| **L293D Left Motor** | `IN1` | **D6** | Left motor direction input 1 |
| **L293D Left Motor** | `IN2` | **D7** | Left motor direction input 2 |
| **L293D Right Motor**| `IN3` | **D10** | Right motor direction input 1 |
| **L293D Right Motor**| `IN4` | **D11** | Right motor direction input 2 |

#### Working Principle & Distance Calculation

The robot measures distance to the nearest target object using the speed of sound:
1. Arduino issues a 10 µs pulse on `TRIG_PIN` (preceded by a 2 µs baseline low).
2. The width of the return echo pulse is measured on `ECHO_PIN` using `pulseIn()` with a 30,000 µs cutoff.
3. Distance in centimeters is calculated using the standard time-of-flight acoustic equation:
   $$\text{Distance (cm)} = \frac{\text{duration} \times 0.034}{2}$$
4. Distance values are printed to the serial monitor at 9600 baud and evaluated against configured operational thresholds:
   * `STOP_DISTANCE = 8 cm`
   * `FOLLOW_DISTANCE = 20 cm`

#### Control Logic & Decision Table

| Measured Distance ($d$) | Vehicle Response | Motor Logic (`IN1, IN2, IN3, IN4`) | Functional Rationale |
| :--- | :--- | :--- | :--- |
| **$d == 0\,\text{cm}$** | **Stop** | `LOW, LOW, LOW, LOW` | Invalid echo reading or sensor timeout; failsafe halt |
| **$d \le 8\,\text{cm}$** | **Stop** | `LOW, LOW, LOW, LOW` | Minimum safety buffer reached; prevents physical collision |
| **$8\,\text{cm} < d \le 20\,\text{cm}$** | **Forward** | `HIGH, LOW, HIGH, LOW` | Object detected in follow zone; drives forward |
| **$d > 20\,\text{cm}$** | **Stop** | `LOW, LOW, LOW, LOW` | Object beyond following range; motors halt |

> **Operational Scope:** The robot is purely a forward-following robot with automated stopping. It does not reverse, steer laterally, or perform obstacle avoidance.

---

### 2. Bluetooth Controlled Robot

* **Source File:** [`bluetooth-robot.ino`](./Bluetooth-Controlled-Robot/bluetooth-robot.ino)
* **Objective:** Remotely teleoperate vehicle translation and directional orientation wirelessly using single-character commands transmitted over Bluetooth.

#### Hardware Components
* Arduino UNO
* L293D Motor Driver
* HC-05 Bluetooth Module
* 2 × DC Gear Motors + Chassis
* Resistor voltage divider (for 5V to 3.3V logic level shifting)
* Battery power source

#### Exact Pin Connections (from Source Code)

| Subsystem | Signal Name | Arduino Uno Pin | Description |
| :--- | :--- | :--- | :--- |
| **HC-05 Bluetooth** | `SoftwareSerial RX` | **D2** | Connected to HC-05 TXD (receives commands) |
| **HC-05 Bluetooth** | `SoftwareSerial TX` | **D3** | Connected to HC-05 RXD via resistor voltage divider |
| **L293D Left Motor** | `ENA` | **D5** | Left motor speed control (PWM, `SPEED = 255`) |
| **L293D Left Motor** | `IN1` | **D8** | Left motor direction input 1 |
| **L293D Left Motor** | `IN2` | **D9** | Left motor direction input 2 |
| **L293D Right Motor**| `ENB` | **D6** | Right motor speed control (PWM, `SPEED = 255`) |
| **L293D Right Motor**| `IN3` | **D10** | Right motor direction input 1 |
| **L293D Right Motor**| `IN4` | **D11** | Right motor direction input 2 |

> **Signal Level Shifting:** The HC-05 RXD pin accepts 3.3V logic. Connecting Arduino pin D3 (5V) through a resistor voltage divider steps down the transmit signal to protect the module.

#### Working Principle & Command Protocol

The firmware creates a software UART port using `SoftwareSerial BT(2, 3)` running at 9600 baud. Incoming characters are parsed in a `switch-case` block:

| Command | Action | Left Motor (`ENA, IN1, IN2`) | Right Motor (`ENB, IN3, IN4`) | Operational Behavior |
| :---: | :--- | :--- | :--- | :--- |
| **`F` / `f`** | Forward | `255, HIGH, LOW` | `255, HIGH, LOW` | Both motors drive forward at full speed |
| **`B` / `b`** | Backward | `255, LOW, HIGH` | `255, LOW, HIGH` | Both motors drive in reverse at full speed |
| **`L` / `l`** | Turn Left | `0, LOW, LOW` | Active in `lastDirection` | Left motor stops; right motor continues running |
| **`R` / `r`** | Turn Right | Active in `lastDirection` | `0, LOW, LOW` | Right motor stops; left motor continues running |
| **`S` / `s`** | Stop | `0, LOW, LOW` | `0, LOW, LOW` | All motor drive signals disabled |

#### Steering Kinematics (Intentional Design)
* The steering system uses **single-wheel differential pivot turns**: sending **`L`** or **`R`** stops the inner motor while the outer motor continues running in the direction established by `lastDirection` (`'F'` or `'B'`).
* If the robot was in a stopped state (`lastDirection == 'S'`), steering commands will not initiate rotation.
* Sending a turn command re-orients the vehicle in place; pressing **`F`** separately is required to drive forward in the new direction. This is intentional operational design, not a malfunction.

---

### 3. Timer Controlled Robot

* **Source File:** [`timer-robot.ino`](./Timer-Controlled-Robot/timer-robot.ino)
* **Objective:** Execute an autonomous, repeatable 5-phase movement routine based strictly on predetermined time delays.

#### Hardware Components
* Arduino UNO
* L293D Motor Driver
* 2 × DC Gear Motors + Chassis
* Battery power source

#### Exact Pin Connections (from Source Code)

| Subsystem | Signal Name | Arduino Uno Pin | Description |
| :--- | :--- | :--- | :--- |
| **L293D Left Motor** | `ENA` | **D5** | Left motor speed control (PWM, `SPEED = 255`) |
| **L293D Left Motor** | `IN1` | **D8** | Left motor direction input 1 |
| **L293D Left Motor** | `IN2` | **D9** | Left motor direction input 2 |
| **L293D Right Motor**| `ENB` | **D6** | Right motor speed control (PWM, `SPEED = 255`) |
| **L293D Right Motor**| `IN3` | **D10** | Right motor direction input 1 |
| **L293D Right Motor**| `IN4` | **D11** | Right motor direction input 2 |

#### Working Principle & Sequence Execution

The robot operates as an open-loop state machine using blocking delays (`delay(5000)`). Locomotion is handled by `moveBot(left1, left2, right1, right2)` with full PWM (`SPEED = 255`) and `stopBot()`.

The main loop continuously repeats the following **25-second cycle** (5 seconds per phase):

```
Phase 1: Backward (5.0s)
   │
Phase 2: Forward (5.0s)
   │
Phase 3: Stop (5.0s)
   │
Phase 4: Left Spin Turn (5.0s)
   │
Phase 5: Right Spin Turn (5.0s)
   │
(Cycle repeats)
```

#### Detailed Phase Breakdown

| Step | Movement Phase | Interval | Left Motor (`IN1, IN2`) | Right Motor (`IN3, IN4`) | Motion Characteristics |
| :---: | :--- | :---: | :--- | :--- | :--- |
| **1** | Backward | 5.0 s | `LOW, HIGH` (Reverse) | `LOW, HIGH` (Reverse) | Straight reverse motion |
| **2** | Forward | 5.0 s | `HIGH, LOW` (Forward) | `HIGH, LOW` (Forward) | Straight forward motion |
| **3** | Stop | 5.0 s | `ENA = 0` | `ENB = 0` | Full motor brake / halt |
| **4** | Left Turn | 5.0 s | `LOW, HIGH` (Reverse) | `HIGH, LOW` (Forward) | Dual-wheel zero-radius spin turn CCW |
| **5** | Right Turn | 5.0 s | `HIGH, LOW` (Forward) | `LOW, HIGH` (Reverse) | Dual-wheel zero-radius spin turn CW |

> **Turning Comparison:** Unlike the Bluetooth robot (which halts one wheel to pivot), the Timer robot drives both wheels in opposite directions simultaneously to perform **zero-radius point turns (spin turns)**.

---

## Technologies and Tools Used

* **Hardware:**
  * Arduino UNO (Microchip ATmega328P 8-bit microcontroller)
  * L293D Dual H-Bridge Motor Driver
  * HC-SR04 Ultrasonic Distance Sensor
  * HC-05 Bluetooth Transceiver Module
  * Dual DC Gear Motors and Chassis Platform
* **Software:**
  * Arduino IDE
  * Embedded C / C++ (Arduino Core)
  * `SoftwareSerial` Arduino Library

---

## Skills Gained

* **Microcontroller I/O Interfacing:** Mapping discrete GPIOs and hardware PWM lines across distinct robotic hardware configurations.
* **DC Motor Control:** Implementing bidirectional H-bridge control logic for forward, reverse, differential pivot, and dual-wheel spin maneuvers.
* **Signal Level Conditioning:** Applying a resistor voltage divider to step down 5V microcontroller logic to 3.3V for transceiver protection.
* **Serial Telemetry:** Parsing incoming UART serial commands using `SoftwareSerial` to control actuators in real time.
* **Time-of-Flight Ranging:** Converting microsecond acoustic echo pulse widths to metric distance measurements.

---

## Future Improvements

* **Non-Blocking Architecture:** Replace blocking delays (`delay()` and `pulseIn()`) with non-blocking timing routines using `millis()` to allow concurrent execution.
* **Closed-Loop Wheel Odometry:** Add optical wheel encoders to measure real-time wheel speed and correct mechanical drift during straight-line travel.
* **Proportional Distance Regulation:** Implement proportional control on the Object Follower to smoothly scale motor speed as the distance to the target changes.
* **Communication Watchdog:** Add an automatic stop timeout on the Bluetooth robot to halt motion if no keep-alive signal is received within a set duration.
