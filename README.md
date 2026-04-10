# 7Semi BMA400 Arduino Library

Arduino driver for the Bosch BMA400 ultra-low power 3-axis accelerometer.

The BMA400 provides precise acceleration measurement, ultra-low power consumption, and an advanced interrupt engine for motion-based applications such as activity detection, step monitoring, and vibration sensing.

## Features

- Full BMA400 register control

- Acceleration measurement (X, Y, Z)

- Configurable Output Data Rate (ODR)

- Selectable measurement range (±2g, ±4g, ±8g, ±16g)

- FIFO buffer support

- Interrupt configuration:
  - Data ready
  - FIFO watermark / full
  - Activity detection (generic interrupt)

- Power modes:
  - Sleep mode
  - Low power mode
  - Normal mode

- Auto low-power and auto wake-up support

- Sensor time reading

- Temperature measurement

---

## Connections / Wiring

The BMA400 supports both **I²C and SPI communication**.

---

## I²C Connection

| BMA400 Pin | MCU Pin         | Notes            |
| ---------- | --------------- | ---------------- |
| VCC        | 3.3V            | **3.3V only**    |
| GND        | GND             | Common ground    |
| SDA        | SDA             | I²C data         |
| SCL        | SCL             | I²C clock        |
| INT1       | GPIO (optional) | Interrupt output |
| INT2       | GPIO (optional) | Interrupt output |

### I²C Notes

- Default I²C address: `0x14` (SDO = GND) or `0x15` (SDO = VCC)

- Supported bus speeds:
  - 100 kHz
  - 400 kHz (recommended)

---

## SPI Connection

| BMA400 Pin | MCU Pin |
| ---------- | ------- |
| VCC        | 3.3V    |
| GND        | GND     |
| SCK        | SCK     |
| MISO       | MISO    |
| MOSI       | MOSI    |
| CS         | GPIO    |
| INT1       | GPIO (optional) |
| INT2       | GPIO (optional) |

---

## Installation

- Arduino Library Manager

  1. Open Arduino IDE  
  2. Go to Library Manager  
  3. Search for **7Semi BMA400**  
  4. Click Install  

- Manual Installation

  1. Download this repository as ZIP  
  2. Arduino IDE → Sketch → Include Library → Add .ZIP Library  

---

## Library Overview

### Reading Acceleration

```cpp
float x, y, z;

sensor.readAccel(x, y, z);

```
- Reading Temperature

## Reading Temperature

```cpp
float temperature;

sensor.readTemperatureC(temperature);
```

- Returns temperature in °C.

## FIFO Data Reading

```cpp
float x, y, z;

sensor.readFifo(x, y, z);
```
- Reads acceleration data from FIFO buffer.

## Power Modes

The BMA400 supports multiple power modes:

- Sleep mode
- Low power mode
- Normal mode

```cpp
sensor.setPowerMode(NORMAL_MODE);
```

## Output Data Rate (ODR)

Configure accelerometer sampling rate:

```cpp
sensor.setAccelODR(ODR_100Hz);
```

## Measurement Range

Set acceleration range:

| Range     | Description |
| --------- | ----------- |
| RANGE_2G  | ±2g         |
| RANGE_4G  | ±4g         |
| RANGE_8G  | ±8g         |
| RANGE_16G | ±16g        |

```cpp
sensor.setAccelRange(RANGE_4G);
```

## Interrupts

The BMA400 supports multiple interrupt sources:

- Data ready
- FIFO watermark
- FIFO full

```cpp
sensor.setIntEnable(BMA400_INT_DATA_READY, true);
```

## Sensor Time

```cpp
uint32_t sensorTime;

sensor.readSensorTime(sensorTime);
```
- Returns internal sensor timestamp.

## Device Status

```cpp
bool power_on;

sensor.checkPowerStatus(power_on);
```

- Provides device power and status information.

### Notes
- Advanced interrupts require Normal mode
- Recommended ODR for interrupts: 100 Hz
- FIFO and interrupt features can be combined for eficient data handling
