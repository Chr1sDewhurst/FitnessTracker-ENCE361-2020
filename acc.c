#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "acc.h"
#include "i2c_driver.h"
#include "math.h"

/*********************************************************
 * initAccl
 *********************************************************/
void
initAccl (void)
{
    char    toAccl[] = {0, 0};  // parameter, value

    /*
     * Enable I2C Peripheral
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /*
     * Set I2C GPIO pins
     */
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    /*
     * Setup I2C
     */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    //Initialize ADXL345 Acceleromter

    // set +-16g, 13 bit resolution, active low interrupts
    toAccl[0] = ACCL_DATA_FORMAT;
    toAccl[1] = (ACCL_RANGE_16G | ACCL_FULL_RES);
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_PWR_CTL;
    toAccl[1] = ACCL_MEASURE;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);


    toAccl[0] = ACCL_BW_RATE;
    toAccl[1] = ACCL_RATE_100HZ;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_INT;
    toAccl[1] = 0x00;       // Disable interrupts from accelerometer.
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_X;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Y;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Z;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);
}

/********************************************************
 * Function to read accelerometer
 ********************************************************/
vector3_t
getAcclData (void)
{
    char    fromAccl[] = {0, 0, 0, 0, 0, 0, 0}; // starting address, placeholders for data to be read.
    vector3_t acceleration;
    uint8_t bytesToRead = 6;

    fromAccl[0] = ACCL_DATA_X0;
    I2CGenTransmit(fromAccl, bytesToRead, READ, ACCL_ADDR);

    acceleration.x = (fromAccl[2] << 8) | fromAccl[1]; // Return 16-bit acceleration readings.
    acceleration.y = (fromAccl[4] << 8) | fromAccl[3];
    acceleration.z = (fromAccl[6] << 8) | fromAccl[5];

    return acceleration;
}

/*********************************************************************
 * Function to calculate mean accelerometer data from a circ buffer
 *  for fitness tracker assignment
 *********************************************************************/
int32_t accel_mean(circBuf_t * axis_buff) {

    int32_t sum;
    int32_t mean;
    uint16_t i;
    int32_t buffer_size;

    buffer_size = (*axis_buff).size;
    sum = 0;
    for (i = 0; i < buffer_size; i++) {
        sum = sum + readCircBuf (axis_buff);
    }
    //Calculate and display the rounded mean of the buffer contents
    mean = (2 *sum + buffer_size) / 2 / buffer_size;
    return mean;
}

/*********************************************************************
 * Function to calculate norm of accelerometer data from a circ buffer
 *  for fitness tracker assignment
 *********************************************************************/
int32_t accelNorm (circBuf_t * axis_buff_x, circBuf_t * axis_buff_y ,circBuf_t * axis_buff_z) {
    vector3_t accel_vector;
    accel_vector.x= accel_mean(axis_buff_x);
    accel_vector.y = accel_mean(axis_buff_y);
    accel_vector.z = accel_mean(axis_buff_z);
    int16_t normal;
    int16_t x = accel_vector.x *100 / 236;
    int16_t y = accel_vector.y *100 / 236;
    int16_t z = accel_vector.z *100 / 236;

    normal = sqrt(x*x + y*y + z*z);
    return normal;
}
