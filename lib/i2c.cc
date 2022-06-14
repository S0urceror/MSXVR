#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "i2c.hh"

extern "C" {
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
}

namespace openmsx {

I2C::~I2C ()
{
    if (file_id>=0)
        close (file_id);
}

bool I2C::init ()
{
    file_id = -1;
    adapter_nr = 1;
    snprintf(dev_filename, 19, "/dev/i2c-%d", adapter_nr);

    file_id = open(dev_filename, O_RDWR);
    if (file_id < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        std::cerr << "Unable to open I2C port: "<<dev_filename<<"\n" << strerror(errno) << "\n";
        return false;
    }
    return true;
}

uint8_t I2C::get_register (uint8_t reg)
{
    union i2c_smbus_data data;
    if (file_id<0)
        return 0;

    if (i2c_smbus_access (file_id, I2C_SMBUS_READ, reg, I2C_SMBUS_BYTE_DATA, &data))
        return -1 ;
    else
        return data.byte & 0xFF ;
}

bool I2C::set_register (uint8_t reg, uint8_t value)
{
    union i2c_smbus_data data ;
    if (file_id<0)
        return false;

    data.byte = value ;
    return i2c_smbus_access (file_id, I2C_SMBUS_WRITE, reg, I2C_SMBUS_BYTE_DATA, &data)==0;
}

bool I2C::select_device (uint8_t device_id)
{
    if (file_id<0)
        return false;

    if (ioctl(file_id, I2C_SLAVE, device_id) < 0) {
        /* ERROR HANDLING; you can check errno to see what went wrong */
        std::cerr << "Unable to select I2C device 0x21\n" << strerror(errno) << "\n";
        return false;
    }
    return true;
}

}