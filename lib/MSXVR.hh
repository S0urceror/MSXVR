#ifndef MSXVR_HH
#define MSXVR_HH

#include "gpio.hh"
#include "i2c.hh"
#include "debug.h"

namespace openmsx {

class MSXVR
{
    private:
        // I2C
        I2C i2c;
        GPIO gpio;

        // Leds
        uint8_t power_status,led_status,relais_status,gpio_0_7,gpio_8_15,gpio_16_23,gpio_24_31;

        // cartridge communication routines
        uint8_t     cartridge_communication (uint8_t slotnr, bool isIO, bool isRead, uint16_t address, uint8_t new_value);
        uint32_t    get_l1_value (uint8_t slotnr, bool isIO, bool isRead, uint16_t address);
        uint32_t    get_l2_value (uint16_t address);
        void        write_l1 (uint32_t value);
        void        write_l2 (uint32_t value);
        uint8_t     get_value ();
        void        set_value (uint8_t new_value);
        void        reset_ls();
        void        set_l1();
        void        reset_l1();
        void        set_l2();
        void        reset_l2();

    public:
        typedef enum  
        {
            CAPS,
            KANA, // same as CODE LED
            PAUSE,
            FDD,
            UD1,
            UD2,
            UD3,
            UD4
        } LEDS;

                    MSXVR ();
                    ~MSXVR ();
        bool        initGPIO ();
        bool        init ();
        void        setLed (LEDS led,bool status);
        uint8_t     readJoystickStatus (int port);
        uint8_t     readButtonStatus ();
        inline bool getCartridgeVCC ();
        void        setCartridgeVCC (bool on);
        void        setCassetteMotor (bool on);
        uint8_t     readIO (uint8_t address);
        void        writeIO (uint8_t address, uint8_t value);
        uint8_t     readMemory (uint8_t slot, uint16_t address);
        void        writeMemory (uint8_t slot, uint16_t address, uint8_t value);
};

}
#endif // MSXVR_HH