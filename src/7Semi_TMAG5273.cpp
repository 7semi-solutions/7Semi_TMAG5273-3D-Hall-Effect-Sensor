/**
 * 7Semi TMAG5273 Arduino Library
 *
 * Arduino driver for the Texas Instruments TMAG5273
 * 3-axis Hall-effect magnetic sensor.
 *
 * Copyright (c) 2025-2026 7Semi
 *
 * Licensed under the MIT License.
 */
#include "7Semi_TMAG5273.h"

TMAG5273_7Semi::TMAG5273_7Semi() {}

/**
 * Initialize TMAG5273 sensor
 *
 * - Configures I2C communication with the sensor
 * - Supports custom SDA/SCL pins for ESP32 boards
 * - Sets the I2C clock speed
 * - Verifies the sensor by reading the manufacturer ID
 * - Applies default sensor configuration using initSensor()
 *
 * Initialization Flow
 * - Store I2C interface and device address
 * - Start I2C bus
 * - Set I2C clock frequency
 * - Read manufacturer ID register
 * - Verify device identity
 * - Apply default sensor configuration
 *
 * Manufacturer ID
 * - Expected value: 0x4954
 *
 * Return
 * - true  → Sensor detected and initialized successfully
 * - false → I2C communication failed or wrong device detected
 */
bool TMAG5273_7Semi::begin(uint8_t i2cAddress,
                           TwoWire &i2cPort,
                           uint32_t i2cClock,
                           uint8_t sda,
                           uint8_t scl)
{
    i2c = &i2cPort;
    address = i2cAddress;

#ifdef ARDUINO_ARCH_ESP32

    /**
     * ESP32 supports configurable I2C pins
     *
     * - If SDA and SCL are provided, use custom pins
     * - Otherwise use default board I2C pins
     */
    if (sda != 255 && scl != 255)
        i2c->begin(sda, scl);
    else
        i2c->begin();

#else

    (void)sda;
    (void)scl;
    i2c->begin();

#endif

    i2c->setClock(i2cClock);

    /**
     * Check if device responds on I2C
     */
    i2c->beginTransmission(address);
    if (i2c->endTransmission() != 0)
        return false;

    int16_t id;

    /**
     * Read sensor manufacturer ID
     *
     * - Used to confirm correct device is connected
     * - Communication failure returns false
     */
    if (!readManufacturerID(id))
        return false;

    if (id != 0x4954)
        return false;

    /**
     * Apply default sensor configuration
     *
     * Includes:
     * - Magnetic channel enable
     * - Temperature channel enable
     * - Continuous measurement mode
     * - Range configuration
     * - Noise averaging
     */
    return initSensor();
}

// -------------------- Low-level I2C --------------------
/**
 * Write single byte to register
 *
 * - Convenience wrapper for register write
 * - Writes one byte to the specified register
 *
 * - Return
 *   - true  → write successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::writeReg(uint8_t reg, uint8_t value)
{
    return writeReg(reg, &value, 1);
}

/**
 * Write multiple bytes to register
 *
 * - Sends register address followed by data bytes
 * - Used for writing configuration values
 *
 * - Return
 *   - true  → write successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::writeReg(uint8_t reg, uint8_t *data, size_t len)
{
    i2c->beginTransmission(address);
    i2c->write(reg);

    for (size_t i = 0; i < len; i++)
        i2c->write(data[i]);

    return (i2c->endTransmission() == 0);
}

/**
 * Read multiple bytes from register
 *
 * - Reads sequential bytes starting from register address
 * - Stores received data into provided buffer
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readReg(uint8_t reg, uint8_t *data, size_t len)
{
    i2c->beginTransmission(address);
    i2c->write(reg);

    if (i2c->endTransmission(false) != 0)
        return false;

    if (i2c->requestFrom(address, (uint8_t)len) < len)
        return false;

    for (size_t i = 0; i < len; i++)
        data[i] = i2c->read();

    return true;
}

/**
 * Read single byte from register
 *
 * - Convenience wrapper for register read
 * - Reads one byte from the specified register
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readReg(uint8_t reg, uint8_t &value)
{
    return readReg(reg, &value, 1);
}

/**
 * Read 16-bit value from register pair
 *
 * - Reads two bytes from sensor registers
 * - Combines MSB and LSB into signed value
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readReg16(uint8_t reg, int16_t &value)
{
    uint8_t buf[2];
    if (!readReg(reg, buf, 2))
        return false;

    value = ((int16_t)buf[0] << 8) | buf[1];
    return true;
}

/**
 * Write 16-bit value to register
 *
 * - Splits 16-bit value into MSB and LSB bytes
 * - Writes both bytes to consecutive sensor registers
 *
 * - Return
 *   - true  → write successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::writeReg16(uint8_t reg, int16_t value)
{
    uint8_t buf[2];
    buf[0] = (value >> 8) & 0xFF;
    buf[1] = value & 0xFF;
    return writeReg(reg, buf, 2);
}

/**
 * Write bit field to register
 *
 * - Updates selected bits inside the register
 * - Other register bits remain unchanged
 *
 * - Return
 *   - true  → write successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::writeBits(uint8_t reg, uint8_t pos, uint8_t len, uint8_t value)
{
    uint8_t v;
    if (!readReg(reg, v))
        return false;

    uint8_t mask = ((1 << len) - 1) << pos;
    v = (v & ~mask) | ((value << pos) & mask);

    return writeReg(reg, v);
}

/**
 * Read bit field from register
 *
 * - Read selected bits from  the register
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readBits(uint8_t reg, uint8_t pos, uint8_t len, uint8_t &value)
{
    uint8_t v;
    if (!readReg(reg, v))
        return false;

    uint8_t mask = ((1 << len) - 1) << pos;
    value = (v & mask) >> pos;

    return true;
}

/**
 * Read bit from register
 *
 * - Read selected bits from  the register
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readBit(uint8_t reg, uint8_t pos, bool &value)
{
    uint8_t v;
    if (!readReg(reg, v))
        return false;

    value = (v >> pos) & 0x01;
    return true;
}

/**
 * Write bit to register
 *
 * - Write selected bit to  the register
 *
 * - Return
 *   - true  → write successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::writeBit(uint8_t reg, uint8_t pos, bool value)
{
    uint8_t v;
    if (!readReg(reg, v))
        return false;

    if (value)
        v |= (1 << pos);
    else
        v &= ~(1 << pos);

    return writeReg(reg, v);
}

bool TMAG5273_7Semi::initSensor()
{
    /**
     * Magnetic channel configuration
     * - Enable X, Y, Z measurements
     */
    if (!setMagChannel(MAG_XYZ))
        return false;

    /**
     * Enable temperature measurement
     */
    if (!enableTemperatureChannel(true))
        return false;

    /**
     * Continuous measurement mode
     */
    if (!setOperatingMode(MODE_CONTINUOUS))
        return false;

    /**
     * Disable internal angle engine
     * (we calculate angle in software)
     */
    if (!setAngleEnable(ANGLE_DISABLE))
        return false;

    /**
     * Normal power mode (lowest noise)
     */
    if (!setLowPower(POWER_NORMAL))
        return false;

    /**
     * Magnetic range configuration
     * 80 mT range
     */
    if (!setXYRange(XY_16384))
        return false;

    if (!setZRange(Z_16384))
        return false;

    /**
     * Averaging for noise reduction
     * 32x gives very stable readings
     */
    if (!setSampling(AVG_32X))
        return false;

    /**
     * Temperature compensation for magnets
     */
    if (!setTemperatureCoefficient(TEMPCO_CERAMIC))
        return false;

    /**
     * Enable I2C glitch filter
     * improves noise immunity
     */
    if (!setI2CGlitchFilter(FILTER_ON))
        return false;

    /**
     * Trigger mode disabled
     * conversion starts automatically
     */
    if (!setTriggerMode(TRIGGER_DISABLE))
        return false;

    /**
     * Default threshold settings
     */
    if (!setThresholdDirection(THR_ABOVE))
        return false;

    if (!setThresholdCount(THR_1COUNT))
        return false;

    /**
     * Disable interrupts by default
     */
    if (!setInterrupt(false, false, false, INTERRUPT_MODE_NONE))
        return false;

    return true;
}
// -------------------- Device Configuration --------------------
/**
 * configureDeviceConfig1
 *
 * - Create register value for DEVICE_CONFIG_1
 * - Combines CRC mode, temperature coefficient, averaging and read mode
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → invalid parameter or I2C communication failed
 */
bool TMAG5273_7Semi::configureDeviceConfig1(MagTempco tempco, ConvAvg avg, I2CReadMode readMode)
{
    if (tempco > TEMPCO_CERAMIC || avg > AVG_32X || readMode > I2C_RESERVED)
        return false;
    uint8_t v = (tempco << 5) | (avg << 2) | readMode;
    return writeReg(TMAG5273_DEVICE_CONFIG_1, &v, 1);
}

// bool TMAG5273_7Semi::setCRC(bool enable)
// {
//     return writeBit(TMAG5273_DEVICE_CONFIG_1, 1, enable);
// }

// bool TMAG5273_7Semi::getCRC(bool &enable)
// {
//     return readBit(TMAG5273_DEVICE_CONFIG_1, 1, enable);
// }

/**
 * Set magnet temperature coefficient
 *
 * - Configures temperature compensation for magnet material
 * - Updates TEMPCO bits in DEVICE_CONFIG_1 register
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setTemperatureCoefficient(MagTempco tempco)
{
    return writeBits(TMAG5273_DEVICE_CONFIG_1, 5, 2, tempco);
}
/**
 * Read magnet temperature coefficient
 *
 * - Reads temperature compensation setting from sensor
 * - Converts register value to percentage per °C
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed or invalid value
 */
bool TMAG5273_7Semi::getTemperatureCoefficient(float &tempco)
{
    uint8_t v = 0;

    if (!readBits(TMAG5273_DEVICE_CONFIG_1, 5, 2, v))
        return false;

    switch (v)
    {
    case 0:
        tempco = 0.00f;
        break;
    case 1:
        tempco = 0.12f;
        break;
    case 2:
        tempco = 0.20f;
        break;
    default:
        tempco = 0.00f;
        return false;
    }

    return true;
}

/**
 * Set conversion averaging
 *
 * - Configures measurement averaging inside sensor
 * - Higher averaging reduces noise but increases conversion time
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setSampling(ConvAvg avg)
{
    return writeBits(TMAG5273_DEVICE_CONFIG_1, 2, 3, avg);
}
/**
 * Read conversion averaging
 *
 * - Reads averaging configuration from sensor
 * - Converts register value to averaging factor
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getSampling(uint8_t &avg)
{
    uint8_t raw = 0;

    if (!readBits(TMAG5273_DEVICE_CONFIG_1, 2, 3, raw))
        return false;

    avg = 1u << raw;
    return true;
}

/**
 * configureDeviceConfig2
 *
 * - Create register value for DEVICE_CONFIG_2
 * - Combines threshold hysteresis, power mode, glitch filter,
 *   trigger mode and operating mode
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → invalid parameter or I2C communication failed
 */
bool TMAG5273_7Semi::configureDeviceConfig2(ThrHyst hyst, PowerMode power, GlitchFilter filter, TriggerMode trigger, OperatingMode mode)
{
    if (hyst > THR_RESERVED_7 || mode > MODE_WAKE_SLEEP)
        return false;
    uint8_t v = (hyst << 5) | (power << 4) | (filter << 3) | (trigger << 2) | mode;
    return writeReg(TMAG5273_DEVICE_CONFIG_2, &v, 1);
}

// -------------------- Sensor Configuration --------------------
/**
 * Set power mode
 *
 * - Configures sensor power behavior
 * - Selects between low active current mode and low noise mode
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setLowPower(PowerMode mode)
{
    return writeBit(TMAG5273_DEVICE_CONFIG_2, 4, static_cast<bool>(mode));
}

/**
 * Enable or disable I2C glitch filter
 *
 * - Improves communication reliability in noisy environments
 * - Controls glitch filter bit in DEVICE_CONFIG_2 register
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setI2CGlitchFilter(GlitchFilter filter)
{
    return writeBit(TMAG5273_DEVICE_CONFIG_2, 3, static_cast<bool>(filter));
}
/**
 * Configure trigger mode
 *
 * - Selects how a measurement conversion starts
 * - Can be triggered by I2C command or external INT pin
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setTriggerMode(TriggerMode mode)
{
    return writeBit(TMAG5273_DEVICE_CONFIG_2, 2, static_cast<bool>(mode));
}

/**
 * Set sensor operating mode
 *
 * - Controls sensor measurement behavior
 * - Modes include standby, continuous and wake-sleep
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setOperatingMode(OperatingMode mode)
{
    return writeBits(TMAG5273_DEVICE_CONFIG_2, 0, 2, static_cast<uint8_t>(mode));
}

/**
 * Read sensor operating mode
 *
 * - Reads operating mode from DEVICE_CONFIG_2 register
 * - Returns mode value defined by sensor configuration
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getOperatingMode(uint8_t &mode)
{
    uint8_t val = 0;
    if (!readBits(TMAG5273_DEVICE_CONFIG_2, 0, 2, val))
        return false;

    mode = (val & 0x03);
    return true;
}

// -------------------- Sensor Configuration --------------------
/**
 * setMagChannel
 *
 * - Select magnetic measurement channels
 * - Enables X, Y, Z axis combinations for magnetic sensing
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → invalid parameter or I2C communication failed
 */
bool TMAG5273_7Semi::setMagChannel(MagChannel channel)
{
    if (channel > MAG_XZX)
        return false;
    return writeBits(TMAG5273_SENSOR_CONFIG_1, 4, 4, static_cast<uint8_t>(channel));
}

/**
 * getMagChannel
 *
 * - Read configured magnetic measurement channels
 * - Returns active axis configuration from sensor register
 *
 * - Return
 *   - true  → value read successfully
 *   - false → invalid value or I2C communication failed
 */
bool TMAG5273_7Semi::getMagChannel(uint8_t &channel)
{
    uint8_t val = 0;

    if (!readBits(TMAG5273_SENSOR_CONFIG_1, 4, 4, val))
        return false;

    if (val > MAG_XZX)
        return false;
    channel = val;
    return true;
}

/**
 * setSleepTime
 *
 * - Configure sleep interval in wake-sleep operating mode
 * - Controls delay between sensor measurements
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → invalid parameter or I2C communication failed
 */
bool TMAG5273_7Semi::setSleepTime(SleepTime time)
{
    if (time > SLEEP_20000MS)
        return false;
    return writeBits(TMAG5273_SENSOR_CONFIG_1, 0, 4, time);
}
/**
 * getSleepTime
 *
 * - Read configured sleep interval
 * - Returns wake-sleep timing value from sensor
 *
 * - Return
 *   - true  → value read successfully
 *   - false → invalid value or I2C communication failed
 */
bool TMAG5273_7Semi::getSleepTime(uint8_t &time)
{

    uint8_t v = 0;

    if (!readBits(TMAG5273_SENSOR_CONFIG_1, 0, 4, v))
        return false;

    if (v > SLEEP_20000MS)
        return false;
    time = v;
    return true;
}

/**
 * setThresholdCount
 *
 * - Configure threshold detection count
 * - Defines number of threshold crossings required
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setThresholdCount(ThresholdCount count)
{
    return writeBit(TMAG5273_SENSOR_CONFIG_2, 6, count);
}

/**
 * getThresholdCount
 *
 * - Read threshold detection count setting
 * - Returns number of measurements required to trigger threshold
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getThresholdCount(uint8_t &count)
{
    bool v = 0;
    if (!readBit(TMAG5273_SENSOR_CONFIG_2, 6, v))
        return false;

    if (v)
        count = 4;
    else
        count = 1;

    return true;
}
/**
 * setThresholdDirection
 *
 * - Configure threshold comparison direction
 * - Selects above or below threshold detection
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setThresholdDirection(ThresholdDirection dir)
{
    return writeBit(TMAG5273_SENSOR_CONFIG_2, 5, dir);
}
/**
 * getThresholdDirection
 *
 * - Read configured threshold comparison direction
 * - Indicates whether detection is above or below threshold
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getThresholdDirection(bool &dir)
{
    bool b;
    if (!readBit(TMAG5273_SENSOR_CONFIG_2, 5, b))
        return false;

    dir = b;
    return true;
}

/**
 * setMagGainChannel
 *
 * - Select channel used for gain adjustment
 * - Chooses first or second magnetic measurement channel
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setMagGainChannel(uint8_t channel)
{
    return writeBit(TMAG5273_SENSOR_CONFIG_2, 4, channel);
}
/**
 * getMagGainChannel
 *
 * - Read selected gain adjustment channel
 * - Indicates which magnetic channel controls gain
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getMagGainChannel(bool &channel)
{
    return readBit(TMAG5273_SENSOR_CONFIG_2, 4, channel);
}

/**
 * setAngleEnable
 *
 * - Enable internal sensor angle calculation
 * - Selects axis pair used for angle computation
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setAngleEnable(AngleEnable enable)
{
    return writeBits(TMAG5273_SENSOR_CONFIG_2, 2, 2, enable);
}

/**
 * getAngleEnable
 *
 * - Read internal angle engine configuration
 * - Returns enabled axis pair used for angle measurement
 *
 * - Return
 *   - true  → value read successfully
 *   - false → invalid value or I2C communication failed
 */
bool TMAG5273_7Semi::getAngleEnable(uint8_t &enable)
{

    uint8_t v = 0;

    if (!readBits(TMAG5273_SENSOR_CONFIG_2, 2, 2, v))
        return false;

    if (v > ANGLE_ENABLE_XZ)
        return false;
    enable = v;
    return true;
}
/**
 * setXYRange
 *
 * - Configure magnetic measurement range for X and Y axes
 * - Updates internal scaling used for field conversion
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setXYRange(XYRange range)
{
    if (!writeBit(TMAG5273_SENSOR_CONFIG_2, 1, range))
        return false;

    xy_range_mT = range ? 80.0f : 40.0f;
    return true;
}

/**
 * getXYRange
 *
 * - Read configured magnetic range for X and Y axes
 * - Indicates selected measurement range
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getXYRange(bool &range)
{
    return readBit(TMAG5273_SENSOR_CONFIG_2, 1, range);
}
/**
 * setZRange
 *
 * - Configure magnetic measurement range for Z axis
 * - Updates internal scaling used for field conversion
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setZRange(ZRange range)
{
    if (!writeBit(TMAG5273_SENSOR_CONFIG_2, 0, range))
        return false;

    z_range_mT = range ? 80.0f : 40.0f;
    return true;
}

/**
 * getZRange
 *
 * - Read configured magnetic range for Z axis
 * - Indicates selected measurement range
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getZRange(bool &range)
{
    return readBit(TMAG5273_SENSOR_CONFIG_2, 0, range);
}

// -------------------- Thresholds --------------------
/**
 * setMagThreshold
 *
 * - Configure magnetic threshold values for X, Y and Z axes
 * - Each threshold defines the magnetic field level required
 *   to trigger the sensor threshold event
 *
 * - Return
 *   - true  → thresholds written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setMagThreshold(uint8_t x, uint8_t y, uint8_t z)
{
    if (!writeReg(TMAG5273_X_THR_CONFIG, x))
        return false;
    if (!writeReg(TMAG5273_Y_THR_CONFIG, y))
        return false;
    if (!writeReg(TMAG5273_Z_THR_CONFIG, z))
        return false;
    return true;
}

/**
 * getMagThreshold
 *
 * - Read magnetic threshold values for X, Y and Z axes
 * - Returns currently configured threshold levels from sensor
 *
 * - Return
 *   - true  → thresholds read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getMagThreshold(uint8_t &x, uint8_t &y, uint8_t &z)
{
    if (!readReg(TMAG5273_X_THR_CONFIG, x))
        return false;
    if (!readReg(TMAG5273_Y_THR_CONFIG, y))
        return false;
    if (!readReg(TMAG5273_Z_THR_CONFIG, z))
        return false;
    return true;
}


// -------------------- Temperature --------------------
/**
 * enableTemperatureChannel
 *
 * - Enable or disable internal temperature measurement
 * - Controls temperature channel bit in T_CONFIG register
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::enableTemperatureChannel(bool enable)
{
    return writeBit(TMAG5273_T_CONFIG, 0, enable);
}

/**
 * setTemperatureThresholdRaw
 *
 * - Configure raw temperature threshold value
 * - Updates threshold bits in T_CONFIG register
 *
 * - Return
 *   - true  → threshold written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setTemperatureThresholdRaw(uint8_t thresholdRaw) {
    uint8_t v; readReg(TMAG5273_T_CONFIG, v);
    v &= 0x01;
    v |= (thresholdRaw << 1) & 0xFE;
    return writeReg(TMAG5273_T_CONFIG, &v, 1);
}

/**
 * setTemperatureThresholdCelsius
 *
 * - Configure temperature threshold using Celsius value
 * - Converts temperature to sensor raw threshold format
 *
 * - Return
 *   - true  → threshold written successfully
 *   - false → invalid value or communication failure
 */
bool TMAG5273_7Semi::setTemperatureThresholdCelsius(int16_t tempC)
{
    if (tempC < -41 || tempC > 170)
        return false;
    uint8_t thresholdRaw = (tempC + 41) / 8;
    return setTemperatureThresholdRaw(thresholdRaw);
}
/**
 * getTemperatureThresholdRaw
 *
 * - Read configured raw temperature threshold value
 * - Extracts threshold bits from T_CONFIG register
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getTemperatureThresholdRaw(uint8_t &thresholdRaw)
{
    uint8_t v;
    if (!readReg(TMAG5273_T_CONFIG, v))
        return false;

    thresholdRaw = (v >> 1) & 0x7F;
    return true;
}

/**
 * setInterrupt
 *
 * - Configure interrupt behavior
 * - Controls enable, threshold detection, latch mode and interrupt source
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setInterrupt(bool enable, bool thresholCross, bool latch, interrupt_mode mode)
{
    uint8_t v = 0;
    v |= enable << 7;
    v |= thresholCross << 6;
    v |= latch << 5;
    v |= mode << 2;
    return writeReg(TMAG5273_INT_CONFIG_1, v);
}
/**
 * getInterrupt
 *
 * - Read interrupt configuration
 * - Returns interrupt enable state, threshold crossing flag,
 *   latch configuration and interrupt mode
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getInterrupt(bool &enable, bool &thresholdCross, bool &latch, uint8_t &mode)
{
    uint8_t v;
    if (!readReg(TMAG5273_INT_CONFIG_1, v))
        return false;

    enable = (v >> 7) & 0x01;
    thresholdCross = (v >> 6) & 0x01;
    latch = (v >> 5) & 0x01;
    mode = (v >> 2) & 0x07;

    return true;
}
/**
 * setInterruptMode
 *
 * - Configure interrupt source mode
 * - Updates interrupt mode bits in INT_CONFIG_1 register
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setInterruptMode(interrupt_mode mode)
{
    return writeBits(TMAG5273_INT_CONFIG_1, 2, 3, mode);
}
/**
 * getInterruptMode
 *
 * - Read configured interrupt mode
 * - Extracts interrupt source bits from INT_CONFIG_1 register
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getInterruptMode(uint8_t &mode)
{
    return readBits(TMAG5273_INT_CONFIG_1, 2, 3, mode);
}

/**
 * getMagGain
 *
 * - Read configured magnetic gain value
 * - Retrieves gain setting from MAG_GAIN_CONFIG register
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getMagGain(uint8_t &gain)
{
    return readReg(TMAG5273_MAG_GAIN_CONFIG, gain);
}

/**
 * setMagOffset
 *
 * - Configure magnetic offset correction
 * - Writes offset value to MAG_OFFSET_CONFIG register
 *
 * - Return
 *   - true  → configuration written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setMagOffset(int16_t offset)
{
    return writeReg16(TMAG5273_MAG_OFFSET_CONFIG_1, offset);
}
/**
 * getMagOffset
 *
 * - Read configured magnetic offset correction
 * - Retrieves offset value from MAG_OFFSET_CONFIG register
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getMagOffset(int16_t &offset)
{
    return readReg16(TMAG5273_MAG_OFFSET_CONFIG_1, offset);
}

/**
 * setI2CAddress
 *
 * - Configure I2C address stored in the sensor register
 * - Optionally update the active device address
 *
 * - Return
 *   - true  → address written successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::setI2CAddress(uint8_t address, bool updateAddress)
{
    uint8_t v = 0;
    v |= address << 1;
    v |= updateAddress & 1;
    return writeReg(TMAG5273_I2C_ADDRESS, v);
}

/**
 * getI2CAddress
 *
 * - Read current I2C address configuration
 * - Returns stored address and update flag
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getI2CAddress(uint8_t &address, bool &updateAddress)
{
    uint8_t v = 0;

    if (!readReg(TMAG5273_I2C_ADDRESS, v))
        return false;

    address = v >> 1;
    updateAddress = v & 1;

    return true;
}
/**
 * getSensorVersion
 *
 * - Read sensor silicon version
 * - Extract version bits from DEVICE_ID register
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getSensorVersion(uint8_t &version)
{
    uint8_t v = 0;

    if (!readReg(TMAG5273_DEVICE_ID, v))
        return false;

    version = v & 0x03;
    return true;
}
// -------------------- Read Device Info --------------------
/**
 * readManufacturerID
 *
 * - Read 16-bit manufacturer identifier
 * - Used during initialization to verify device identity
 *
 * - Return
 *   - true  → read successful
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readManufacturerID(int16_t &id)
{
    if (!readReg16(TMAG5273_MANUFACTURER_ID_LSB, id))
        return false;
    return true;
}

/**
 * readTemperatureRaw
 *
 * - Read raw temperature ADC value from sensor
 * - Combines MSB and LSB register values
 *
 * - Return
 *   - true  → data read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readTemperatureRaw(int16_t &tempRaw)
{
    uint8_t msb, lsb;

    if (!readReg(TMAG5273_T_MSB_RESULT, msb))
        return false;
    if (!readReg(TMAG5273_T_LSB_RESULT, lsb))
        return false;

    tempRaw = ((int16_t)msb << 8) | lsb;
    return true;
}
/**
 * readTemperatureC
 *
 * - Read temperature and convert to Celsius
 * - Applies conversion formula from sensor datasheet
 *
 * - Return
 *   - true  → temperature read successfully
 *   - false → sensor read failed
 */
bool TMAG5273_7Semi::readTemperatureC(float &temperatureC)
{
    int16_t raw;

    if (!readTemperatureRaw(raw))
        return false;

    const float TSENS_T0 = 25.0;
    const int16_t TADC_T0 = 17508;
    const float TADC_RES = 256.0;

    temperatureC = TSENS_T0 + ((raw - TADC_T0) / TADC_RES);

    return true;
}
/**
 * readRaw
 *
 * - Read raw magnetic measurements for X, Y and Z axes
 * - Returns signed 16-bit sensor values
 *
 * - Return
 *   - true  → data read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readRaw(int16_t &x, int16_t &y, int16_t &z)
{
    if (!readReg16(TMAG5273_X_MSB_RESULT, x))
        return false;
    if (!readReg16(TMAG5273_Y_MSB_RESULT, y))
        return false;
    if (!readReg16(TMAG5273_Z_MSB_RESULT, z))
        return false;

    return true;
}
/**
 * readMag
 *
 * - Read magnetic field values for X, Y and Z axes
 * - Converts raw sensor data to milliTesla
 *
 * - Return
 *   - true  → measurement successful
 *   - false → sensor read failed
 */
bool TMAG5273_7Semi::readMag(float &x, float &y, float &z)
{
    int16_t rx, ry, rz;

    if (!readRaw(rx, ry, rz))
        return false;

    x = (rx / 32768.0f) * xy_range_mT;
    y = (ry / 32768.0f) * xy_range_mT;
    z = (-1 * rz / 32768.0f) * z_range_mT;

    return true;
}


/**
 * calculateAngleXY
 *
 * - Calculate magnetic angle using X and Y field values
 * - Uses atan2 function and converts result to degrees
 *
 * - Return
 *   - true  → angle calculated successfully
 *   - false → sensor read failed
 */
bool TMAG5273_7Semi::calculateAngleXY(float &angleDeg)
{
    float x, y, z;

    if (!readMag(x, y, z))
        return false;

    angleDeg = atan2(y, x) * 57.29578f;

    if (angleDeg < 0)
        angleDeg += 360.0f;

    return true;
}

/**
 * readConversionStatus
 *
 * - Read conversion status register
 * - Provides measurement readiness and diagnostic flags
 *
 * - Return
 *   - true  → status read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readConversionStatus(ConvStatus &status)
{
    uint8_t val;

    if (!readReg(TMAG5273_CONV_STATUS, val))
        return false;

    status.setCount = (val >> 5) & 0x07;
    status.por = (val >> 4) & 0x01;
    status.diagFail = (val >> 1) & 0x01;
    status.resultReady = val & 0x01;

    return true;
}
/**
 * readAngle
 *
 * - Read internally calculated sensor angle
 * - Converts raw angle register value to degrees
 *
 * - Return
 *   - true  → angle read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readAngle(float &angle)
{
    int16_t raw;

    if (!readReg16(TMAG5273_ANGLE_RESULT_MSB, raw))
        return false;

    angle = (raw * 360.0) / 16384.0;

    return true;
}
/**
 * readAngleRaw
 *
 * - Read raw angle measurement value
 * - Combines MSB and LSB registers into 13-bit angle value
 *
 * - Return
 *   - true  → data read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readAngleRaw(uint16_t &angleRaw)
{
    uint8_t msb, lsb;

    if (!readReg(TMAG5273_ANGLE_RESULT_MSB, msb))
        return false;

    if (!readReg(TMAG5273_ANGLE_RESULT_LSB, lsb))
        return false;

    angleRaw = ((uint16_t)msb << 5) | (lsb >> 3);

    return true;
}

// -------------------- Device Status --------------------
/**
 * readDeviceStatus
 *
 * - Read device diagnostic status
 * - Reports interrupt state, oscillator errors and voltage faults
 *
 * - Return
 *   - true  → status read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::readDeviceStatus(DeviceStatus &status)
{
    uint8_t val;

    if (!readReg(TMAG5273_DEVICE_STATUS, val))
        return false;

    status.intb = (val >> 4) & 0x01;
    status.oscError = (val >> 3) & 0x01;
    status.intError = (val >> 2) & 0x01;
    status.otpCrcError = (val >> 1) & 0x01;
    status.vccUV = val & 0x01;

    return true;
}

/**
 * getTemperatureThresholdCelsius
 *
 * - Read temperature threshold in Celsius
 * - Converts raw threshold register value to temperature
 *
 * - Return
 *   - true  → value read successfully
 *   - false → I2C communication failed
 */
bool TMAG5273_7Semi::getTemperatureThresholdCelsius(int16_t &tempC)
{
    uint8_t thresholdRaw;

    if (!getTemperatureThresholdRaw(thresholdRaw))
        return false;

    tempC = (thresholdRaw * 8) - 41;

    return true;
}



// uint8_t calculateCRC(uint8_t *data, uint8_t length)
// {
//     uint8_t crc = 0xFF;

//     for (uint8_t i = 0; i < length; i++)
//     {
//         crc ^= data[i];

//         for (uint8_t j = 0; j < 8; j++)
//         {
//             if (crc & 0x80)
//                 crc = (crc << 1) ^ 0x07;
//             else
//                 crc <<= 1;
//         }
//     }

//     return crc;
// }
