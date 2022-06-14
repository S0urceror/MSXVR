#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include "gpio.hh"

namespace openmsx
{
    bool GPIO::init ()
    {
        int fd ;

        //Obtain handle to physical memory
        if ((fd = open ("/dev/gpiomem", O_RDWR | O_SYNC) ) < 0) 
        {
            std::cerr << "Unable to open /dev/gpiomem\n" << strerror (errno) << "\n";
            return false;
        }

        _gpio = (uint32_t *)mmap(NULL, 0xB4, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        close (fd);

        if (_gpio == MAP_FAILED)
        {
            std::cerr << "Mmap failed\n" << strerror(errno) << "\n";
            return false;
        }
        return true;
    }

    void GPIO::set_register (uint16_t reg_index, uint32_t reg_value)
    {
        _gpio[reg_index] = reg_value;
    }
    uint32_t GPIO::get_register (uint16_t reg_index)
    {
        return _gpio[reg_index];
    }
}