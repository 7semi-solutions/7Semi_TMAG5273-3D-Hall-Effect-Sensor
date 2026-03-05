 /* 7Semi TMAG5273 Arduino Library
 *
 * Arduino driver for the Texas Instruments TMAG5273
 * 3-axis Hall-effect magnetic sensor.
 *
 * Copyright (c) 2025-2026 7Semi
 *
 * Licensed under the MIT License.
 */
#ifndef _7SEMI_TMAG5273_H_
#define _7SEMI_TMAG5273_H_
#pragma once
#include <Arduino.h>
#include <Wire.h>

/**
 * 7Semi TMAG5273
 *
 * - Driver for Texas Instruments TMAG5273 3-Axis Hall Effect Sensor
 * - Supports magnetic field measurement, angle calculation and temperature
 * - Uses I2C communication
 *
 * Features
 * - Magnetic X / Y / Z measurement
 * - Temperature sensor
 * - Angle calculation
 * - Threshold interrupt support
 * - Configurable averaging and power modes
 */
// -------------------- Register Addresses --------------------
#define TMAG5273_DEVICE_CONFIG_1       0x00
#define TMAG5273_DEVICE_CONFIG_2       0x01
#define TMAG5273_SENSOR_CONFIG_1       0x02
#define TMAG5273_SENSOR_CONFIG_2       0x03
#define TMAG5273_X_THR_CONFIG          0x04
#define TMAG5273_Y_THR_CONFIG          0x05
#define TMAG5273_Z_THR_CONFIG          0x06
#define TMAG5273_T_CONFIG              0x07
#define TMAG5273_INT_CONFIG_1          0x08
#define TMAG5273_MAG_GAIN_CONFIG       0x09
#define TMAG5273_MAG_OFFSET_CONFIG_1   0x0A
#define TMAG5273_MAG_OFFSET_CONFIG_2   0x0B
#define TMAG5273_I2C_ADDRESS           0x0C
#define TMAG5273_DEVICE_ID             0x0D
#define TMAG5273_MANUFACTURER_ID_LSB   0x0E
#define TMAG5273_MANUFACTURER_ID_MSB   0x0F
#define TMAG5273_T_MSB_RESULT          0x10
#define TMAG5273_T_LSB_RESULT          0x11
#define TMAG5273_X_MSB_RESULT          0x12
#define TMAG5273_X_LSB_RESULT          0x13
#define TMAG5273_Y_MSB_RESULT          0x14
#define TMAG5273_Y_LSB_RESULT          0x15
#define TMAG5273_Z_MSB_RESULT          0x16
#define TMAG5273_Z_LSB_RESULT          0x17
#define TMAG5273_CONV_STATUS           0x18
#define TMAG5273_ANGLE_RESULT_MSB      0x19
#define TMAG5273_ANGLE_RESULT_LSB      0x1A
#define TMAG5273_MAGNITUDE_RESULT      0x1B
#define TMAG5273_DEVICE_STATUS         0x1C
// -------------------- Enumerations --------------------
// CRC enable/disable
enum CRCMode
{
    CRC_DISABLE = 0,
    CRC_ENABLE = 1
};

// Magnetic temperature compensation
enum MagTempco
{
    TEMPCO_NONE = 0,   // No compensation
    TEMPCO_LOW = 1,    // Low %/°C (NdFeB)
    TEMPCO_MEDIUM = 2, // Medium %/°C
    TEMPCO_CERAMIC = 3 // Ceramic %/°C
};

// Conversion averaging
enum ConvAvg
{
    AVG_1X = 0,
    AVG_2X = 1,
    AVG_4X = 2,
    AVG_8X = 3,
    AVG_16X = 4,
    AVG_32X = 5
};

// I2C read mode
enum I2CReadMode
{
    I2C_MODE_0 = 0,
    I2C_MODE_1 = 1,
    I2C_MODE_2 = 2,
    I2C_RESERVED = 3
};

// Threshold hysteresis
enum ThrHyst
{
    THR_NONE = 0,
    THR_1 = 1,
    THR_2 = 2,
    THR_3 = 3,
    THR_4 = 4,
    THR_5 = 5,
    THR_6 = 6,
    THR_RESERVED_7 = 7
};

// Power mode
enum PowerMode
{
    POWER_NORMAL = 0,
    POWER_LOW = 1
};

// Glitch filter
enum GlitchFilter
{
    FILTER_ON = 0,
    FILTER_OFF = 1
};

// Trigger mode
enum TriggerMode
{
    TRIGGER_DISABLE = 0,
    TRIGGER_ENABLE = 1
};

// Operating mode
enum OperatingMode
{
    STANDBY_MODE = 0,
    MODE_SLEEP = 1,
    MODE_CONTINUOUS = 2,
    MODE_WAKE_SLEEP = 3
};

// Magnetic channel selection
enum MagChannel
{
    MAG_DISABLE = 0,
    MAG_X = 1,
    MAG_Y = 2,
    MAG_XY = 3,
    MAG_Z = 4,
    MAG_ZX = 5,
    MAG_YZ = 6,
    MAG_XYZ = 7,
    MAG_XYX = 8,
    MAG_YXY = 9,
    MAG_YZY = 10,
    MAG_XZX = 11
};

// Sleep timing for low-power mode
enum SleepTime
{
    SLEEP_1MS = 0,
    SLEEP_5MS = 1,
    SLEEP_10MS = 2,
    SLEEP_15MS = 3,
    SLEEP_20MS = 4,
    SLEEP_30MS = 5,
    SLEEP_50MS = 6,
    SLEEP_100MS = 7,
    SLEEP_500MS = 8,
    SLEEP_1000MS = 9,
    SLEEP_2000MS = 10,
    SLEEP_5000MS = 11,
    SLEEP_20000MS = 12
};

// Threshold count
enum ThresholdCount
{
    THR_1COUNT = 0,
    THR_4COUNT = 1
};

// Threshold direction
enum ThresholdDirection
{
    THR_ABOVE = 0,
    THR_BELOW = 1
};

// Angle measurement enable
enum AngleEnable
{
    ANGLE_DISABLE = 0,
    ANGLE_ENABLE_XY = 1,
    ANGLE_ENABLE_YZ = 2,
    ANGLE_ENABLE_XZ = 3
};

// XY measurement range
enum XYRange
{
    XY_8192 = 0,
    XY_16384 = 1
};

// Z measurement range
enum ZRange
{
    Z_8192 = 0,
    Z_16384 = 1
};

// Interrupt routing mode
enum interrupt_mode
{
    INTERRUPT_MODE_NONE = 0x0,                // No interrupt
    INTERRUPT_MODE_INT = 0x1,                 // Interrupt through INT
    INTERRUPT_MODE_INT_EXCEPT_I2C_BUSY = 0x2, // INT except when I2C bus is busy
    INTERRUPT_MODE_SCL = 0x3,                 // Interrupt through SCL
    INTERRUPT_MODE_SCL_EXCEPT_I2C_BUSY = 0x4, // SCL except when I2C bus is busy
};
// -------------------- Status Structs --------------------
struct ConvStatus
{
    uint8_t setCount;
    bool por;
    bool diagFail;
    bool resultReady;
};

struct DeviceStatus
{
    bool intb;
    bool oscError;
    bool intError;
    bool otpCrcError;
    bool vccUV;
};

// -------------------- TMAG5273 Class --------------------
class TMAG5273_7Semi
{
public:
    TMAG5273_7Semi();

    /**
     * begin
     *
     * Initialize TMAG5273 sensor
     *
     * - Configures I2C communication
     * - Verifies manufacturer ID
     * - Applies default configuration using initSensor()
     *
     * Parameters
     * - i2cAddress : Sensor I2C address
     * - i2cPort    : Wire interface
     * - i2cClock   : I2C speed (example 400000)
     * - sda / scl  : Optional ESP32 pin override
     *
     * Return
     * - true  → sensor detected and initialized
     * - false → communication failure or wrong device
     */
    bool begin(uint8_t i2cAddress = 0x35,
               TwoWire &i2cPort = Wire,
               uint32_t i2cClock = 400000,
               uint8_t sda = 255,
               uint8_t scl = 255);

    /**
     * initSensor
     *
     * Apply default sensor configuration
     *
     * Default configuration
     * - Enable X, Y, Z magnetic channels
     * - Enable temperature channel
     * - Continuous measurement mode
     * - 80mT measurement range
     * - 32x averaging
     * - Interrupt disabled
     *
     * Return
     * - true  → configuration successful
     * - false → communication failure
     */
    bool initSensor();

    // -------------------- Configuration --------------------
    /**
     * configureDeviceConfig1
     *
     * Configure DEVICE_CONFIG_1 register
     *
     * Fields
     * - CRC enable
     * - Magnetic temperature coefficient
     * - Conversion averaging
     * - I2C read mode
     *
     * Return
     * - true  → configuration written
     * - false → invalid parameter or I2C error
     */
    bool configureDeviceConfig1(MagTempco tempco, ConvAvg avg, I2CReadMode readMode);
    // bool setCRC(bool enable);
    // bool getCRC(bool &enable);

    /**
     * setTemperatureCoefficient
     *
     * - Configure magnetic temperature compensation coefficient
     * - Used to compensate magnet strength variation with temperature
     *
     * Parameters
     * - tempco : Temperature compensation setting
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setTemperatureCoefficient(MagTempco tempco);

    /**
     * getTemperatureCoefficient
     *
     * - Read configured magnetic temperature compensation value
     * - Returns the temperature coefficient used for compensation
     *
     * Parameters
     * - tempco : Temperature coefficient value (%/°C)
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getTemperatureCoefficient(float &tempco);

    /**
     * setSampling
     *
     * - Configure sensor conversion averaging
     * - Averaging improves measurement stability and noise reduction
     *
     * Parameters
     * - avg : Conversion averaging setting
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setSampling(ConvAvg avg);

    /**
     * getSampling
     *
     * - Read configured sensor conversion averaging
     * - Returns number of samples used for averaging
     *
     * Parameters
     * - avg : Averaging factor
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getSampling(uint8_t &avg);

    /**
     * configureDeviceConfig2
     *
     * Configure DEVICE_CONFIG_2 register
     *
     * Fields
     * - Threshold hysteresis
     * - Power mode
     * - I2C glitch filter
     * - Trigger mode
     * - Operating mode
     *
     * Return
     * - true  → configuration successful
     * - false → invalid parameter or I2C failure
     */
    bool configureDeviceConfig2(ThrHyst hyst, PowerMode power, GlitchFilter filter, TriggerMode trigger, OperatingMode mode);
   
    /**
     * setLowPower
     *
     * - Configure sensor power mode
     * - Controls trade-off between noise performance and power consumption
     *
     * Parameters
     * - mode : Power mode selection
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setLowPower(PowerMode mode);

    /**
     * setI2CGlitchFilter
     *
     * - Enable or disable I2C glitch filter
     * - Helps reject noise on the I2C bus
     *
     * Parameters
     * - filter : Glitch filter configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setI2CGlitchFilter(GlitchFilter filter);

    /**
     * setTriggerMode
     *
     * - Configure sensor trigger mode
     * - Selects how a conversion is started
     *
     * Parameters
     * - mode : Trigger mode configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setTriggerMode(TriggerMode mode);

    /**
     * setOperatingMode
     *
     * - Configure sensor operating mode
     * - Controls measurement behavior such as standby, sleep or continuous
     *
     * Parameters
     * - mode : Operating mode selection
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setOperatingMode(OperatingMode mode);

    /**
     * getOperatingMode
     *
     * - Read current sensor operating mode
     * - Returns mode configured in DEVICE_CONFIG_2 register
     *
     * Parameters
     * - mode : Current operating mode
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getOperatingMode(uint8_t &mode);

    /**
     * setMagChannel
     *
     * - Configure magnetic channel selection
     * - Selects which axes are measured by the sensor
     *
     * Parameters
     * - channel : Magnetic channel configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setMagChannel(MagChannel channel);

    /**
     * getMagChannel
     *
     * - Read magnetic channel configuration
     * - Returns which axes are enabled for measurement
     *
     * Parameters
     * - channel : Magnetic channel configuration
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getMagChannel(uint8_t &channel);

    /**
     * setSleepTime
     *
     * - Configure sleep interval used in wake-sleep operating mode
     * - Determines delay between measurements in low power mode
     *
     * Parameters
     * - time : Sleep interval configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setSleepTime(SleepTime time);

    /**
     * getSleepTime
     *
     * - Read configured sleep interval
     * - Returns sleep timing used in wake-sleep mode
     *
     * Parameters
     * - time : Sleep interval configuration
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getSleepTime(uint8_t &time);

    /**
     * setThresholdCount
     *
     * - Configure number of consecutive threshold events required
     * - Used for threshold detection filtering
     *
     * Parameters
     * - count : Threshold event count configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setThresholdCount(ThresholdCount count);

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
    bool getThresholdCount(uint8_t &count);

   

    /**
     * setThresholdDirection
     *
     * - Configure threshold detection direction
     * - Determines whether threshold triggers above or below limit
     *
     * Parameters
     * - dir : Threshold direction configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setThresholdDirection(ThresholdDirection dir);

     /**
     * getThresholdDirection
     *
     * - Read configured threshold direction
     * - Indicates whether threshold detection is above or below
     *
     * Parameters
     * - dir : Threshold direction configuration
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getThresholdDirection(bool &dir);   
    
    /**
     * setMagGainChannel
     *
     * - Select magnetic channel used for gain adjustment
     * - Determines which axis is used for gain calibration
     *
     * Parameters
     * - channel : Gain adjustment channel
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setMagGainChannel(uint8_t channel);

    /**
     * getMagGainChannel
     *
     * - Read magnetic gain adjustment channel
     * - Returns which channel is used for gain calibration
     *
     * Parameters
     * - channel : Gain adjustment channel
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getMagGainChannel(bool &channel);

    /**
     * setAngleEnable
     *
     * - Enable internal angle calculation engine
     * - Select axes used for angle calculation
     *
     * Parameters
     * - enable : Angle engine configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setAngleEnable(AngleEnable enable);

    /**
     * getAngleEnable
     *
     * - Read angle engine configuration
     * - Returns axes used for internal angle calculation
     *
     * Parameters
     * - enable : Angle engine configuration
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getAngleEnable(uint8_t &enable);

    /**
     * setXYRange
     *
     * - Configure magnetic measurement range for X and Y axes
     * - Selects full scale magnetic field range
     *
     * Parameters
     * - range : XY magnetic range configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setXYRange(XYRange range);

    /**
     * getXYRange
     *
     * - Read configured magnetic range for X and Y axes
     *
     * Parameters
     * - range : XY magnetic range configuration
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getXYRange(bool &range);

    /**
     * setZRange
     *
     * - Configure magnetic measurement range for Z axis
     * - Selects full scale magnetic field range
     *
     * Parameters
     * - range : Z magnetic range configuration
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setZRange(ZRange range);

    /**
     * getZRange
     *
     * - Read configured magnetic range for Z axis
     *
     * Parameters
     * - range : Z magnetic range configuration
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getZRange(bool &range);

    /**
     * setMagThreshold
     *
     * - Configure magnetic threshold levels for X, Y and Z axes
     * - Threshold values are used for magnetic event detection
     *
     * Parameters
     * - x : X-axis threshold value
     * - y : Y-axis threshold value
     * - z : Z-axis threshold value
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setMagThreshold(uint8_t x, uint8_t y, uint8_t z);

    /**
     * getMagThreshold
     *
     * - Read magnetic threshold values for X, Y and Z axes
     * - Returns currently configured threshold levels
     *
     * Parameters
     * - x : X-axis threshold value
     * - y : Y-axis threshold value
     * - z : Z-axis threshold value
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getMagThreshold(uint8_t &x, uint8_t &y, uint8_t &z);

    /**
     * enableTemperatureChannel
     *
     * - Enable or disable the internal temperature sensor
     * - Allows temperature measurement and temperature threshold detection
     *
     * Parameters
     * - enable : Temperature channel enable state
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool enableTemperatureChannel(bool enable);

    /**
     * setTemperatureThresholdRaw
     *
     * - Configure temperature threshold using raw register value
     * - Used for temperature event detection
     *
     * Parameters
     * - thresholdRaw : Raw temperature threshold value
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setTemperatureThresholdRaw(uint8_t thresholdRaw);

    /**
     * setTemperatureThresholdCelsius
     *
     * - Configure temperature threshold in Celsius
     * - Converts Celsius value to sensor register format
     *
     * Parameters
     * - thresholdRaw : Temperature threshold in Celsius
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → invalid value or I2C communication failed
     */
    bool setTemperatureThresholdCelsius(int16_t thresholdRaw);

    /**
     * getTemperatureThresholdRaw
     *
     * - Read raw temperature threshold register value
     * - Used to determine configured temperature event level
     *
     * Parameters
     * - thresholdRaw : Raw temperature threshold value
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getTemperatureThresholdRaw(uint8_t &thresholdRaw);

    /**
     * setInterrupt
     *
     * - Configure interrupt behavior of the sensor
     * - Allows enabling interrupt output and selecting interrupt mode
     *
     * Parameters
     * - enable         : Enable or disable interrupt generation
     * - thresholdCross : Enable interrupt on threshold crossing
     * - latch          : Latch interrupt until status is cleared
     * - mode           : Interrupt routing mode
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setInterrupt(bool enable, bool thresholCross, bool latch, interrupt_mode mode);
   
    /**
     * getInterrupt
     *
     * - Read interrupt configuration from the sensor
     * - Returns interrupt enable state, threshold trigger and mode
     *
     * Parameters
     * - enable         : Current interrupt enable state
     * - thresholdCross : Threshold interrupt status
     * - latch          : Interrupt latch configuration
     * - mode           : Interrupt routing mode
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getInterrupt(bool &enable, bool &thresholdCross, bool &latch, uint8_t &mode);
  
    /**
     * setInterruptMode
     *
     * - Configure interrupt routing mode
     * - Selects where interrupt signal is generated
     *
     * Parameters
     * - mode : Interrupt routing mode
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */

    bool setInterruptMode(interrupt_mode mode);

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
    bool getInterruptMode(uint8_t &mode);
   
    /**
     * setMagGain
     *
     * - Configure magnetic gain adjustment
     * - Used to calibrate magnetic measurement sensitivity
     *
     * Parameters
     * - gain : Gain configuration value
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setMagGain(uint8_t gain);

    /**
     * getMagGain
     *
     * - Read magnetic gain configuration
     * - Returns the current gain adjustment value
     *
     * Parameters
     * - gain : Gain configuration value
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getMagGain(uint8_t &gain);
 
    /**
     * setMagOffset
     *
     * - Configure magnetic offset calibration
     * - Used to compensate sensor bias
     *
     * Parameters
     * - offset : Magnetic offset value
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setMagOffset(int16_t offset);
  
    /**
     * getMagOffset
     *
     * - Read magnetic offset calibration value
     * - Returns current offset stored in the sensor
     *
     * Parameters
     * - offset : Magnetic offset value
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getMagOffset(int16_t &offset);

    /**
     * setI2CAddress
     *
     * - Configure sensor I2C address
     * - Optionally update the address stored in the device
     *
     * Parameters
     * - address        : New 7-bit I2C address
     * - updateAddress  : Apply the new address immediately
     *
     * Return
     *   - true  → configuration written successfully
     *   - false → I2C communication failed
     */
    bool setI2CAddress(uint8_t address, bool updateAddress);

    /**
     * getI2CAddress
     *
     * - Read the current sensor I2C address
     * - Also returns whether address update is enabled
     *
     * Parameters
     * - address       : Current device I2C address
     * - updateAddress : Address update flag
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */
    bool getI2CAddress(uint8_t &address, bool &updateAddress);

    /**
     * getSensorVersion
     *
     * Read sensor silicon revision
     *
     * Output
     * - version : sensor version bits
     *
     * Return
     * - true  → read successful
     * - false → I2C failure
     */
    bool getSensorVersion(uint8_t &version);
   
    /**
     * readManufacturerID
     *
     * Read manufacturer ID from device
     *
     * Expected value
     * - 0x4954 → Texas Instruments
     *
     * Return
     * - true  → read successful
     * - false → communication error
     */
    bool readManufacturerID(int16_t &id);

    /**
     * readTemperatureRaw
     *
     * Read raw temperature values from sensor
     *
     * Output
     * - tempRaw : raw temperature 
     *
     * Return
     * - true  → read successful
     * - false → communication failure
     */

    bool readTemperatureRaw(int16_t &tempRaw);

    /**
     * readTemperatureC
     *
     * Read temperature from sensor
     *
     * Output
     * - temperatureC : temperature in Celsius
     *
     * Return
     * - true  → read successful
     * - false → communication failure
     */

    bool readTemperatureC(float &temperatureC);

    /**
     * readRaw
     *
     * Read raw magnetic data from sensor
     *
     * Output
     * - x : raw X axis
     * - y : raw Y axis
     * - z : raw Z axis
     *
     * Return
     * - true  → read successful
     * - false → I2C communication failure
     */
    bool readRaw(int16_t &x, int16_t &y, int16_t &z);

    /**
     * readMag
     *
     * Read magnetic field values in milliTesla
     *
     * Output
     * - x : magnetic field X axis
     * - y : magnetic field Y axis
     * - z : magnetic field Z axis
     *
     * Return
     * - true  → measurement successful
     * - false → I2C communication error
     */

    bool readMag(float &x, float &y, float &z);
    /**
     * calculateAngleXY
     *
     * Calculate magnetic angle using X and Y axes
     *
     * - Uses atan2(Y, X)
     * - Output angle range : 0 → 360 degrees
     *
     * Return
     * - true  → angle calculated
     * - false → sensor read failure
     */

    bool calculateAngleXY(float &angleDeg);
    /**
     * readConversionStatus
     *
     * - Read sensor conversion status register
     * - Provides information about measurement state and diagnostics
     *
     * Parameters
     * - status : Structure containing conversion status fields
     *
     * Return
     *   - true  → status read successfully
     *   - false → I2C communication failed
     */

    bool readConversionStatus(ConvStatus &status);
    /**
     * readAngle
     *
     * - Read internally calculated magnetic angle from sensor
     * - Converts raw angle register value to degrees
     *
     * Parameters
     * - angle : Calculated angle in degrees
     *
     * Return
     *   - true  → angle read successfully
     *   - false → I2C communication failed
     */

    bool readAngle(float &angle);
    /**
     * readAngleRaw
     *
     * - Read raw angle measurement value from sensor
     * - Combines MSB and LSB registers into a raw angle value
     *
     * Parameters
     * - angleRaw : Raw angle register value
     *
     * Return
     *   - true  → read successful
     *   - false → I2C communication failed
     */

    bool readAngleRaw(uint16_t &angleRaw);
    /**
     * readDeviceStatus
     *
     * Read device diagnostic status
     *
     * Reports
     * - interrupt state
     * - oscillator error
     * - CRC error
     * - undervoltage condition
     *
     * Return
     * - true  → status read successfully
     * - false → I2C communication failed
     */

    bool readDeviceStatus(DeviceStatus &status);
    bool getTemperatureThresholdCelsius(int16_t &tempC);

private:
    TwoWire *i2c;
    uint8_t address;
    float xy_range_mT = 40.0f; // default A1 low range
    float z_range_mT = 40.0f;

    // Low-level helpers

    bool writeReg(uint8_t reg, uint8_t value);
    bool writeReg(uint8_t reg, uint8_t *data, size_t len);
    bool readReg(uint8_t reg, uint8_t *data, size_t len);
    bool readReg(uint8_t reg, uint8_t &value);
    bool readReg16(uint8_t reg, int16_t &value);
    bool writeReg16(uint8_t reg, int16_t value);
    bool writeBits(uint8_t reg, uint8_t pos, uint8_t len, uint8_t value);
    bool readBits(uint8_t reg, uint8_t pos, uint8_t len, uint8_t &value);
    bool readBit(uint8_t reg, uint8_t pos, bool &value);
    bool writeBit(uint8_t reg, uint8_t pos, bool value);
};
#endif