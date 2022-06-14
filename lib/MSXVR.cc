#include <stdint.h>
#include <iostream>
#include <ios>
#include "MSXVR.hh"
#include <unistd.h>

namespace openmsx {

    MSXVR::MSXVR ()
    {
        led_status = 0;
        relais_status = 0b11100000;
        power_status = 0b11111110;
        gpio_0_7 = gpio_8_15 = gpio_16_23 = gpio_24_31 = 0x00;
    }
    MSXVR::~MSXVR ()
    {

    }

    bool MSXVR::init ()
    {
        // GPIO
        if (!gpio.init ())
            return false;

        // set pin modes
        gpio.set_register (0,0b00000000000000000000100100000000); // PIN10-19: all INPUTs except pin 2 and 3 are ALT0 (I2C)
        gpio.set_register (1,0b00001001001001001001001001001001); // PIN10-19: all OUTPUTs
        gpio.set_register (2,0b00001000001001001001001001001001); // PIN20-29: all OUTPUTs except pin 28 is INPUT

        // I2C
        if (!i2c.init ())
            return false;

        // Joysticks - MCP23017
        if (!i2c.select_device (0x20))
            return false;
        i2c.set_register (MCP23x17_IOCON, IOCON_INIT);
        i2c.set_register (MCP23x17_IODIRA, 0x3F);
        i2c.set_register (MCP23x17_IODIRB, 0x3F);
        i2c.set_register (MCP23x17_GPPUA, 0x3F);
        i2c.set_register (MCP23x17_GPPUB, 0x3F);

        // LEDs & power - MCP23017
        if (!i2c.select_device (0x21))
            return false;
        i2c.set_register (MCP23x17_IOCON, IOCON_INIT);
        i2c.set_register (MCP23x17_IODIRA, 0x00);
        i2c.set_register (MCP23x17_IODIRB, 0x0f);
        i2c.set_register (MCP23x17_GPPUA, 0x00);
        i2c.set_register (MCP23x17_GPPUB, 0x0f);
        i2c.set_register (MCP23x17_GPIOA, led_status);    // all leds off
        i2c.set_register (MCP23x17_GPIOB, relais_status); // EEPROM write protection on , VCC + cassette motor off

        // General purpose 32-bits input/output port - MCP23017
        if (!i2c.select_device (0x22))
            return false;
        i2c.set_register (MCP23x17_IOCON, IOCON_INIT);
        i2c.set_register (MCP23x17_IODIRA, 0x00);
        i2c.set_register (MCP23x17_IODIRB, 0x00);
        i2c.set_register (MCP23x17_GPPUA, 0x00);
        i2c.set_register (MCP23x17_GPPUB, 0x00);
        i2c.set_register (MCP23x17_GPIOA, gpio_0_7);  
        i2c.set_register (MCP23x17_GPIOB, gpio_8_15); 
        if (!i2c.select_device (0x23))
            return false;
        i2c.set_register (MCP23x17_IOCON, IOCON_INIT);
        i2c.set_register (MCP23x17_IODIRA, 0x00);
        i2c.set_register (MCP23x17_IODIRB, 0x00);
        i2c.set_register (MCP23x17_GPPUA, 0x00);
        i2c.set_register (MCP23x17_GPPUB, 0x00);
        i2c.set_register (MCP23x17_GPIOA, gpio_16_23);  
        i2c.set_register (MCP23x17_GPIOB, gpio_24_31); 

        // More LEDs & power - MCP23017
        if (!i2c.select_device (0x24))
            return false;
        i2c.set_register (MCP23x17_IOCON, IOCON_INIT);
        i2c.set_register (MCP23x17_IODIRA, 0xff); // all input
        i2c.set_register (MCP23x17_IODIRB, 0x00); // output
        i2c.set_register (MCP23x17_GPPUA, 0xff);
        i2c.set_register (MCP23x17_GPPUB, 0x00);   
        i2c.set_register (MCP23x17_GPIOB, power_status);      

        // Cartridge audio amplifier - TPA2016D2
        if (!i2c.select_device (0x58))
            return false;
        i2c.set_register (1, 0b11000011); // SPK_EN_R, SPK_EN_L, SWS (IC on), NOISE_GATE_EN
        i2c.set_register (2, 0b00000001); // ATK_TIME   0b000001
        i2c.set_register (3, 0b00000001); // REL_TIME   0b000001
        i2c.set_register (4, 0b00000000); // HOLD TIME  0b000000 (disabled)
        i2c.set_register (5, 0b00100010); // FIXED GAIN 0b000000 -20dB
        i2c.set_register (6, 0b00100000); // OUTPUT LIMIT 0b0 (disabled) + NOISE THRESHOLD 0b01 (4mV) + OUTPUT LIMIT 0b00000
        i2c.set_register (7, 0b00000011); // MAX GAIN 0b0000 (18dB) + COMPRESSION RATIO 0b11 (8:1)

        return true;
    }

    void MSXVR::setLed (LEDS led,bool status)
    {
        i2c.select_device (0x21);
        switch (led)
        {
            case MSXVR::PAUSE: status?led_status |= 0b00000001: led_status &= 0b11111110; break;
            case MSXVR::UD3:   status?led_status |= 0b00000010: led_status &= 0b11111101; break;
            case MSXVR::UD2:   status?led_status |= 0b00000100: led_status &= 0b11111011; break;
            case MSXVR::UD1:   status?led_status |= 0b00001000: led_status &= 0b11110111; break;
            case MSXVR::FDD:   status?led_status |= 0b00010000: led_status &= 0b11101111; break;
            case MSXVR::KANA:  status?led_status |= 0b00100000: led_status &= 0b11011111; break;
            case MSXVR::CAPS:  status?led_status |= 0b01000000: led_status &= 0b10111111; break;
            case MSXVR::UD4:   status?led_status |= 0b10000000: led_status &= 0b01111111; break;
       }
        i2c.set_register (MCP23x17_GPIOA,led_status);
    }

    /*
    bit 0 - UP 
    bit 1 - DOWN 
    bit 2 - LEFT 
    bit 3 - RIGHT 
    bit 4 - TRG1 
    bit 5 - TRG2
    */
    uint8_t MSXVR::readJoystickStatus (int port)
    {
        i2c.select_device (0x20);
        if (port==0)
            return i2c.get_register (MCP23x17_GPIOA);
        else
            return i2c.get_register (MCP23x17_GPIOB);
    }

    /*
    bit 0 - VCC
    bit 1 - PAUSE
    bit 2 - RESET
    bit 3 - POWER
    */
    uint8_t MSXVR::readButtonStatus ()
    {
        i2c.select_device (0x21);
        return i2c.get_register (MCP23x17_GPIOB)&0x0f;
    }

    inline bool MSXVR::getCartridgeVCC ()
    {
        return (power_status & 0b01000000);
    }

    void MSXVR::setCartridgeVCC (bool on)
    {
        uint8_t prev_power_status = power_status;
        on ? (relais_status &= 0b11101111) : (relais_status |= 0b00010000);
        on ? (power_status |= 0b01000000) : (power_status &= 0b10111111);
        if (on)
        {
            // switch on 12V
            i2c.select_device (0x21);
            i2c.set_register (MCP23x17_GPIOB,relais_status);
            // switch on 5V
            i2c.select_device (0x24);
            i2c.set_register (MCP23x17_GPIOB,prev_power_status);
            i2c.set_register (MCP23x17_GPIOB,power_status);
            // gpio stuff
            reset_ls ();
            // joysticks in input mode
            i2c.select_device (0x20);
            i2c.set_register (MCP23x17_IODIRA,0x3f);
            i2c.set_register (MCP23x17_IODIRB,0x3f);
            i2c.set_register (MCP23x17_GPPUA,0x3f);
            i2c.set_register (MCP23x17_GPPUB,0x3f);
            i2c.set_register (MCP23x17_GPIOA,0x30);
            i2c.set_register (MCP23x17_GPIOB,0x30);
        }
        else
        {
            // joysticks in output mode
            i2c.select_device (0x20);
            i2c.set_register (MCP23x17_GPIOA,0);
            i2c.set_register (MCP23x17_GPIOB,0);
            i2c.set_register (MCP23x17_GPPUA,0);
            i2c.set_register (MCP23x17_GPPUB,0);
            i2c.set_register (MCP23x17_IODIRA,0xff);
            i2c.set_register (MCP23x17_IODIRB,0xff);
            // gpio stuff
            gpio.set_pins (0x0ff3fc00);
            gpio.clr_pins (0x00000000);
            gpio.set_pins (0x00080000);
            gpio.clr_pins (0x00000000);
            gpio.set_pins (0x00000000);
            gpio.clr_pins (0x0ff3fc00);
            gpio.set_pins (0x00040000);
            gpio.clr_pins (0x00000000);
            gpio.set_pins (0x00000000);
            gpio.clr_pins (0x0ff3fc00);
            gpio.clr_pins (0x00080000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.set_pins (0x0ff3fc00);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00040000);
            gpio.clr_pins (0x00000000);
            gpio.set_pins (0x00400000);
            gpio.clr_pins (0x0fb3fc00);
            gpio.set_pins (0x00080000);
            gpio.clr_pins (0x00000000);
            gpio.set_pins (0x00000000);
            gpio.clr_pins (0x0ff3fc00);
            gpio.set_pins (0x00040000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            // switch off 5V
            i2c.select_device (0x24);
            i2c.set_register (MCP23x17_GPIOB,power_status);
            i2c.set_register (MCP23x17_GPIOB,prev_power_status);
            i2c.set_register (MCP23x17_GPIOB,prev_power_status);
            i2c.set_register (MCP23x17_GPIOB,prev_power_status);
            i2c.set_register (MCP23x17_GPIOB,prev_power_status);
            i2c.set_register (MCP23x17_GPIOB,power_status);
            // switch off 12V
            i2c.select_device (0x21);
            i2c.set_register (MCP23x17_GPIOB,relais_status);
        }
    }

    void MSXVR::setCassetteMotor (bool on)
    {
        i2c.select_device (0x21);
        on ? (relais_status &= 0b11010000) : (relais_status |= 0b00100000);
        i2c.set_register (MCP23x17_GPIOB,relais_status);
    }

    // Cartridge slot communication
    uint32_t MSXVR::get_l1_value (uint8_t slotnr, bool isIO, bool isRead, uint16_t address)
    {
        // The following logic applies:
        // /4 is zero when slot 4
        // /3 is zero when slot 3
        // /2 is zero when slot 2
        // /1 is zero when slot 1
        // 4321 is 1111 during IO
        // LL are always zero L1 will be latched high later
        // W is 1 when write
        // R is 1 when read
        // M is 1 when memory request, 0 when io request
        // I is 1 when io request, 0 when memory request
        // CS12,CS1,CS2 is 1 when address <0x4000 and >0xc000
        // CS1 is 1 when address >=0x4000 and <0x8000
        // CS2 is 1 when address >=0x8000 and <0xc000

        // FIRST LATCH:           .4321...LLWMRCCC.I..........  - Order L1,L2,CS12,CS1,CS2
        uint32_t value =        0b1000011100000000100000000000;
        // set slot
        switch (slotnr)
        {
            case 1: value |=    0b0111000000000000000000000000;
                    break;
            case 2: value |=    0b0110100000000000000000000000;
                    break;
            case 3: value |=    0b0101100000000000000000000000;
                break;
            case 4: value |=    0b0011100000000000000000000000;
                break;
        }
        //                        .4321...LLWMRCCC.I..........  - Order L1,L2,CS12,CS1,CS2
        // iorq or memrq
        if (isIO)   value |=    0b0111100000000111010000000000;
        else        value |=    0b0000000000010000000000000000;
        // rd or wr
        if (isRead) value |=    0b0000000000001000000000000000;
        else        value |=    0b0000000000100000000000000000;
        //                        .4321...LLWMRCCC.I..........  - Order L1,L2,CS12,CS1,CS2
        // cs1, cs2 or cs12 signals
        if (address>=0x4000 && address<0x8000)                  //  cs1 + _cs2 + _cs12
                    value |=   0b0000000000000010000000000000;
        else if (address>=0x8000 && address<0xc000)             // _cs1 +  cs2 + _cs12
                    value |=   0b0000000000000001000000000000;
        else                                                    //  cs1 +  cs2 +  cs12
                    value |=   0b0000000000000111000000000000;
        
        return value;
    }
    uint32_t MSXVR::get_l2_value (uint16_t address)
    {
        // SECOND LATCH: AAAA AAAA LLAA AAAA AADD DDDD ssDD
        uint32_t value;
        value = (address & 0xff00) << 12;
        value |= (address & 0xff) << 10;
        return value;
    }
    void MSXVR::write_l1 (uint32_t value)
    {
        uint32_t inverse = value ^ 0x0ff3fc00;
        gpio.set_pins (value);
        debug ("SET: %08X - ",value);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(value));

        gpio.clr_pins (inverse);
        debug ("CLR: %08X - ",inverse);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(inverse));
    }
    void MSXVR::write_l2 (uint32_t value)
    {
        uint32_t inverse = value ^ 0x0ff3fc00;

        gpio.set_pins (value);
        debug ("SET: %08X - ",value);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(value));
        
        gpio.clr_pins (inverse);
        debug ("CLR: %08X - ",inverse);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(inverse));
    }
    uint8_t MSXVR::get_value ()
    {
        uint32_t value = gpio.get_pins();
        debug ("GET: %08X - ",value);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(value));
        return ((value & 0x3f0) >> 2) + (value & 0x3);
    }
    void MSXVR::set_value (uint8_t new_value)
    {
        uint32_t value = new_value << 10;
        uint32_t inverse = value ^ 0x0ff3fc00;
        gpio.set_pins (value);
        debug ("SET: %08X - ",value);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(value));
        gpio.clr_pins (inverse);
        debug ("CLR: %08X - ",inverse);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(inverse));
    }

    void MSXVR::reset_ls()
    {
        // reset L1 and L2
        gpio.clr_pins (0xc0000);
        debug ("CLR: %08X - ",0xc0000);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(0xc0000));
    }
    void MSXVR::set_l1()
    {
        // set L1
        gpio.set_pins (0x80000);
        debug ("SET: %08X - ",0x80000);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(0x80000));
    }
    void MSXVR::reset_l1()
    {
        // reset L1
        gpio.clr_pins (0x80000);
        debug ("CLR: %08X - ",0x80000);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(0x80000));
    }
    void MSXVR::set_l2()
    {
        // set L2
        gpio.set_pins (0x40000);
        debug ("SET: %08X - ",0x40000);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(0x40000));
    }
    void MSXVR::reset_l2()
    {
        // reset L2
        gpio.clr_pins (0x40000);
        debug ("CLR: %08X - ",0x40000);
        debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(0x40000));
    }

    uint8_t MSXVR::cartridge_communication (uint8_t slotnr, bool isIO, bool isRead, uint16_t address, uint8_t new_value)
    {
        uint8_t value=0;

        // set type of transfer
        //////////////////////////////////////////////////////////
        uint32_t L1 = get_l1_value (slotnr,isIO,isRead,address);
        write_l1 (L1);
        set_l1();
        // delay
        gpio.clr_pins(0x00000000);
        // end of delay

        // specify address or port
        //////////////////////////////////////////////////////////
        uint32_t L2 = get_l2_value (address);
        write_l2 (L2);

        // delay, additional pin read and clear nothing
        if (isRead && !isIO)
            gpio.get_pins();
        gpio.clr_pins(0x00000000);
        // end of delay
        
        set_l2();

        // read value
        //////////////////////////////////////////////////////////
        if (isRead)
        {
            // delay, read 6 times the same
            gpio.get_pins();
            gpio.get_pins();
            gpio.get_pins();
            gpio.get_pins();
            gpio.get_pins();
            gpio.get_pins();
            // end of delay

            // now for real
            value = get_value ();
            debug ("GET: %08X - ",value);
            debug (BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(value));
            // reset latches
            reset_ls ();
        }
        // write value
        //////////////////////////////////////////////////////////
        else
        {
            // delay
            gpio.clr_pins (0x00000000);
            // end of delay

            set_value (new_value);
            reset_l1 ();

            // delay, clear nothing for 9 times
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            gpio.clr_pins (0x00000000);
            // end of delay

            write_l1 (L1);
            reset_l2 ();
        }
        
        return value;
    }

    uint8_t MSXVR::readIO (uint8_t address)
    {
        if (!getCartridgeVCC())
            return 0xff;

        return cartridge_communication (1,true,true,address,0);
    }
    void MSXVR::writeIO (uint8_t address, uint8_t value)
    {
        if (!getCartridgeVCC())
            return;
            
        cartridge_communication (1,true,false,address,value);
    }
    uint8_t MSXVR::readMemory (uint8_t slot, uint16_t address)
    {
        if (!getCartridgeVCC())
            return 0xff;

        return cartridge_communication (slot,false,true,address,0);
    }
    void MSXVR::writeMemory (uint8_t slot, uint16_t address, uint8_t value)
    {
        if (!getCartridgeVCC())
            return;
            
        cartridge_communication (slot,false,false,address,value);
    }
}