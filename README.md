# 7Semi TMAG5273 Arduino Library

Arduino driver for the Texas Instruments TMAG5273 3-axis Hall-effect magnetic sensor.

The TMAG5273 provides high-accuracy magnetic field measurement, temperature sensing, and angle calculation.

## Features

- Full TMAG5273 register control

- Magnetic field measurement (X, Y, Z)

- Magnetic angle calculation

- Temperature measurement

- Magnetic threshold detection

- Interrupt configuration

- Magnetic gain and offset calibration

- Low-power and continuous measurement modes

- Conversion status monitoring
  
---
 ## Connections / Wiring

The TMAG5273 communicates using I²C.

## I²C Connection
| TMAG5273 Pin | MCU Pin         | Notes            |
| ------------ | --------------- | ---------------- |
| VCC          | 3.3V            | **3.3V only**    |
| GND          | GND             | Common ground    |
| SDA          | SDA             | I²C data         |
| SCL          | SCL             | I²C clock        |
| INT          | GPIO (optional) | Interrupt output |

### I²C Notes

- Default I²C address: 0x35

- Supported bus speeds:

  - 100 kHz

  - 400 kHz (recommended)

---

## Installation
- Arduino Library Manager

  1. Open Arduino IDE

  2. Go to Library Manager

  3. Search for 7Semi TMAG5273

  4. Click Install

- Manual Installation

  1. Download this repository as ZIP

  2. Arduino IDE → Sketch → Include Library → Add .ZIP Library

---

## Library Overview

### Reading Magnetic Field
```cpp
float x, y, z;

sensor.readMag(x, y, z);
```

- Returns magnetic field strength in milliTesla (mT).

### Reading Temperature
```cpp
float temperature;

sensor.readTemperatureC(temperature);
```
- Returns temperature in °C.

### Angle Measurement

The TMAG5273 can be used for magnetic rotary encoders.
```cpp

float angle;

sensor.calculateAngleXY(angle);
```
- Returns 0-360° angle based on the magnetic field.

### Operating Modes

The TMAG5273 supports multiple operating modes:

- Continuous measurement
- Low power mode
- Wake-up and sleep mode
- Triggered measurements

These modes allow balancing power consumption and response time depending on the application.

Example:
```cpp
sensor.setOperatingMode(MODE_CONTINUOUS);
```
### Magnetic Range

Magnetic field range can be configured.

Range	Field Strength
XY_8192	±40 mT
XY_16384	±80 mT

Example:
```cpp
sensor.setXYRange(XY_16384);
sensor.setZRange(Z_16384);
```
### Interrupts

The sensor supports interrupt generation for events such as:

Magnetic threshold detection

Conversion completion

System diagnostics

Example:
```cpp
sensor.setInterrupt(true, false, false, INTERRUPT_MODE_INT);
```
### Threshold Detection

Threshold detection allows the sensor to trigger interrupts when magnetic field values exceed a defined level.
```cpp
sensor.setMagThreshold(20, 20, 20);
```
### Conversion Status

Check if a new measurement is ready.

ConvStatus status;
```cpp
sensor.readConversionStatus(status);

if(status.resultReady)
{
  Serial.println("New data available");
}
```
### Device Diagnostics

The sensor provides internal status information.

DeviceStatus status;
```cpp
sensor.readDeviceStatus(status);

if(status.vccUV)
{
  Serial.println("Undervoltage detected");
}
```
