# Arduino Robotics Projects

A collection of mobile robotics projects developed during an internship at **Robokart**. This repository contains practical Arduino implementations demonstrating ultrasonic distance tracking, wireless Bluetooth serial control, and time-based autonomous routines using differential drive platforms.

---

## Table of Contents

- [Repository Overview](#repository-overview)
- [Repository Structure](#repository-structure)
- [Common Hardware Architecture](#common-hardware-architecture)
  - [Shared Components](#shared-components)
  - [Standard Motor Driver Pinout](#standard-motor-driver-pinout)
- [Project Details](#project-details)
  - [1. Object Following Robot](#1-object-following-robot)
  - [2. Bluetooth Controlled Robot](#2-bluetooth-controlled-robot)
  - [3. Timer Controlled Robot](#3-timer-controlled-robot)
- [Technologies and Tools Used](#technologies-and-tools-used)
- [Skills Gained](#skills-gained)
- [Future Improvements](#future-improvements)

---

## Repository Overview

This repository documents three Arduino-based robotics projects built around the **Arduino UNO** and the **L293D dual H-bridge motor driver**:

1. **Object Following Robot:** Uses an ultrasonic sensor to detect and follow an object within a designated distance range.
2. **Bluetooth Controlled Robot:** Receives serial commands via an HC-05 Bluetooth module for directional movement and rotation.
3. **Timer Controlled Robot:** Executes a fixed, predefined movement sequence using time-based delays.

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

## Common Hardware Architecture

### Shared Components

All three robots share a common differential drive hardware foundation:

| Component | Description | Purpose |
| :--- | :--- | :--- |
| **Arduino UNO** | ATmega328P microcontroller board (5V logic, 16 MHz) | Main controller executing robot firmware |
| **L293D Motor Driver** | Dual H-bridge motor driver IC / board | Drives two DC motors with bidirectional control |
| **DC Gear Motors** | 2 × BO motors configured for differential drive | Left and right wheel propulsion |
| **Robot Chassis** | 2-wheel drive chassis with 1 caster wheel | Mechanical robot base |
| **Power Supply** | Battery pack | Powers microcontroller and motor driver |

---

### Standard Motor Driver Pinout

All three robots share the same physical L293D motor driver connections on the Arduino UNO chassis:

| L293D Pin | Arduino Uno Pin | Pin Type | Assigned Function |
| :--- | :--- | :--- | :--- |
| **ENA** | **D5** | Hardware PWM | Left motor speed control |
| **IN1** | **D8** | Digital Output | Left motor direction input 1 |
| **IN2** | **D9** | Digital Output | Left motor direction input 2 |
| **ENB** | **D6** | Hardware PWM | Right motor speed control |
| **IN3** | **D10** | Digital Output | Right motor direction input 1 |
| **IN4** | **D11** | Digital Output | Right motor direction input 2 |

---

## Project Details

### 1. Object Following Robot

* **Source File:** [`object-following-robot.ino`](./Object-Following-Robot/object-following-robot.ino)
* **Objective:** Track an object in front of the robot and move forward to maintain distance, stopping when the object is too close or out of range.

#### Components
* Arduino UNO
* L293D Motor Driver
* HC-SR04 Ultrasonic Distance Sensor
* 2 × DC Gear Motors + Chassis

#### Confirmed Physical Connections

| Subsystem | Signal / Pin | Arduino Uno Pin | Description |
| :--- | :--- | :--- | :--- |
| **HC-SR04 Sensor** | Trig | **D2** | Sends 10 µs trigger pulse |
| **HC-SR04 Sensor** | Echo | **D3** | Receives echo return pulse |
| **L293D Left Motor** | ENA | **D5** | Hardware PWM speed enable |
| **L293D Left Motor** | IN1 | **D8** | Direction input 1 |
| **L293D Left Motor** | IN2 | **D9** | Direction input 2 |
| **L293D Right Motor**| ENB | **D6** | Hardware PWM speed enable |
| **L293D Right Motor**| IN3 | **D10** | Direction input 1 |
| **L293D Right Motor**| IN4 | **D11** | Direction input 2 |

> [!WARNING]
> **Hardware-to-Software Pin Mismatch (Flagged for Review):**
> * **Physical Wiring:** Wired to `ENA = D5`, `IN1 = D8`, `IN2 = D9`, `ENB = D6`, `IN3 = D10`, `IN4 = D11`.
> * **Current Source Code:** The existing sketch [`object-following-robot.ino`](./Object-Following-Robot/object-following-robot.ino) currently defines `IN1 = 6`, `IN2 = 7`, `IN3 = 10`, `IN4 = 11`, and does not initialize or control `ENA` and `ENB`.
> * **Action Required:** To run this firmware on the physical chassis without rewiring, the code's `#define` assignments should be updated to match the confirmed physical pins, and PWM control for `ENA`/`ENB` should be added in `setup()` and motor functions.

#### Working Principle & Control Logic

The robot calculates distance using the time-of-flight of ultrasonic sound waves emitted by the HC-SR04:
1. Arduino sends a 10 µs pulse on `TRIG_PIN` (preceded by a 2 µs low pulse).
2. The duration of the reflected echo pulse is measured on `ECHO_PIN` using `pulseIn()` with a 30,000 µs timeout.
3. Distance in centimeters is calculated using the speed of sound:
   $$\text{Distance (cm)} = \frac{\text{duration} \times 0.034}{2}$$
4. The robot evaluates the distance against defined thresholds:
   * `STOP_DISTANCE = 8 cm`
   * `FOLLOW_DISTANCE = 20 cm`

#### Decision Table

| Measured Distance ($d$) | Robot Action | Motor Logic (`IN1, IN2, IN3, IN4`) | Reason |
| :--- | :--- | :--- | :--- |
| **$d == 0\,\text{cm}$** | **Stop** | `LOW, LOW, LOW, LOW` | Sensor timeout or out of range |
| **$d \le 8\,\text{cm}$** | **Stop** | `LOW, LOW, LOW, LOW` | Object too close (safety margin) |
| **$8\,\text{cm} < d \le 20\,\text{cm}$** | **Forward** | `HIGH, LOW, HIGH, LOW` | Object detected in follow range |
| **$d > 20\,\text{cm}$** | **Stop** | `LOW, LOW, LOW, LOW` | Object exceeds follow range |

*Note: The code implements forward following and stopping. It does not perform reverse tracking or lateral steering.*

---

### 2. Bluetooth Controlled Robot

* **Source File:** [`bluetooth-robot.ino`](./Bluetooth-Controlled-Robot/bluetooth-robot.ino)
* **Objective:** Teleoperate vehicle translation and directional orientation wirelessly via serial commands transmitted from a paired Bluetooth device.

#### Components
* Arduino UNO
* L293D Motor Driver
* HC-05 Bluetooth Module
* 2 × DC Gear Motors + Chassis
* Resistors for voltage divider (5V to 3.3V)

#### Pin Connections

*Motor driver connections follow the [Standard Motor Driver Pinout](#standard-motor-driver-pinout) (`ENA=D5, IN1=D8, IN2=D9, ENB=D6, IN3=D10, IN4=D11`).*

| Module | Module Pin | Arduino Uno Pin | Description |
| :--- | :--- | :--- | :--- |
| **HC-05 Bluetooth** | TXD | **D2** | Arduino SoftwareSerial RX (receives serial data) |
| **HC-05 Bluetooth** | RXD | **D3** | Arduino SoftwareSerial TX (via voltage divider) |

> **Note on Voltage Divider:** The HC-05 RXD pin uses 3.3V logic. A resistor voltage divider is placed between Arduino pin D3 (5V) and HC-05 RXD to safely step down the voltage.

#### Working Principle & Command Protocol

The robot uses the `SoftwareSerial` library on pins 2 and 3 at 9600 baud to read incoming single-character ASCII commands:

| Command | Action | Left Motor (`ENA, IN1, IN2`) | Right Motor (`ENB, IN3, IN4`) | Behavior |
| :---: | :--- | :--- | :--- | :--- |
| **`F` / `f`** | Forward | `255, HIGH, LOW` | `255, HIGH, LOW` | Both motors drive forward |
| **`B` / `b`** | Backward | `255, LOW, HIGH` | `255, LOW, HIGH` | Both motors drive backward |
| **`L` / `l`** | Turn Left | `0, LOW, LOW` | Continues in `lastDirection` | Left motor stops; right motor continues in `lastDirection` |
| **`R` / `r`** | Turn Right | Continues in `lastDirection` | `0, LOW, LOW` | Right motor stops; left motor continues in `lastDirection` |
| **`S` / `s`** | Stop | `0, LOW, LOW` | `0, LOW, LOW` | Both motors stop completely |

#### Steering Control Behavior (Intentional Design)
* Sending **`L`** or **`R`** stops the inner motor and keeps the outer motor running in the current direction (`lastDirection`), causing the robot to rotate and adjust its heading.
* If the robot was stopped (`'S'`), sending `'L'` or `'R'` does not initiate rotation.
* To move forward in the new direction after turning, the user sends a separate **`F`** command. This decoupled orient-then-drive behavior is intentional.

---

### 3. Timer Controlled Robot

* **Source File:** [`timer-robot.ino`](./Timer-Controlled-Robot/timer-robot.ino)
* **Objective:** Execute an autonomous, repeatable 5-phase movement pattern based on predetermined time delays.

#### Components
* Arduino UNO
* L293D Motor Driver
* 2 × DC Gear Motors + Chassis

#### Pin Connections

*Motor driver connections follow the [Standard Motor Driver Pinout](#standard-motor-driver-pinout) (`ENA=D5, IN1=D8, IN2=D9, ENB=D6, IN3=D10, IN4=D11`). No external sensors are required.*

#### Working Principle & Movement Sequence

The robot uses standard time delays (`delay(5000)`) to execute an autonomous sequence in a continuous loop. Each movement phase runs for exactly **5 seconds**:

```
Step 1: Backward (5s)
   │
Step 2: Forward (5s)
   │
Step 3: Stop (5s)
   │
Step 4: Left Turn (5s)
   │
Step 5: Right Turn (5s)
   │
(Repeats continuously)
```

#### Detailed Phase Breakdown

| Step | Movement | Duration | Left Motor (`IN1, IN2`) | Right Motor (`IN3, IN4`) | Motion Description |
| :---: | :--- | :---: | :--- | :--- | :--- |
| **1** | Backward | 5 seconds | `LOW, HIGH` (Reverse) | `LOW, HIGH` (Reverse) | Both wheels drive backward |
| **2** | Forward | 5 seconds | `HIGH, LOW` (Forward) | `HIGH, LOW` (Forward) | Both wheels drive forward |
| **3** | Stop | 5 seconds | `ENA = 0` | `ENB = 0` | Both motors disabled |
| **4** | Left Turn | 5 seconds | `LOW, HIGH` (Reverse) | `HIGH, LOW` (Forward) | Wheels turn in opposite directions (spins left) |
| **5** | Right Turn | 5 seconds | `HIGH, LOW` (Forward) | `LOW, HIGH` (Reverse) | Wheels turn in opposite directions (spins right) |

---

## Technologies and Tools Used

* **Hardware:**
  * Arduino UNO (Microchip ATmega328P microcontroller)
  * L293D Dual H-Bridge Motor Driver
  * HC-SR04 Ultrasonic Distance Sensor
  * HC-05 Bluetooth Module
  * Dual DC Gear Motors and Chassis Platform
* **Software:**
  * Arduino IDE
  * Embedded C / C++ (Arduino Core)
  * `SoftwareSerial` Arduino Library

---

## Skills Gained

* **Microcontroller Interfacing:** Configuring digital I/O pins, pulse width modulation (PWM) motor speed control, and sensor timing.
* **DC Motor Control:** Implementing bidirectional H-bridge control logic for forward, reverse, pivot, and spin movements.
* **Signal Level Shifting:** Designing a resistor voltage divider to step down 5V Arduino signals to 3.3V logic for the HC-05 module.
* **Serial Communication:** Handling UART data transmission between Arduino and Bluetooth modules using `SoftwareSerial`.
* **State Management:** Tracking robot motion state (`lastDirection`) to coordinate steering and translation.

---

## Future Improvements

* **Firmware Update for Object Follower:** Align `object-following-robot.ino` pin definitions with the physical chassis wiring (`ENA=D5, IN1=D8, IN2=D9, ENB=D6, IN3=D10, IN4=D11`).
* **Non-Blocking Control:** Refactor `delay()` and `pulseIn()` using `millis()` to allow concurrent task handling.
* **Encoder Feedback:** Add wheel encoders for closed-loop speed measurement and straight-line correction.
* **Proportional Speed Control:** Dynamically adjust motor PWM based on distance to object rather than fixed on/off thresholds.
* **Bluetooth Failsafe:** Implement an automatic stop timeout if no Bluetooth packet is received within a set period.
