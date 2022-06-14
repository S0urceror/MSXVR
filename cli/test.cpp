#include <iostream>
#include <unistd.h>
#include <chrono>
#include "lib/MSXVR.hh"

openmsx::MSXVR msxvr;
    
uint8_t check_ch376 (uint8_t value)
{
    msxvr.writeIO (0x21,0x06);
    msxvr.writeIO (0x20,value);
    return msxvr.readIO (0x20);
}

uint32_t checksum (uint8_t slot, uint16_t start, uint16_t end)
{
    uint32_t check = 0;
    for (uint16_t address = start;address<end;address++)
        check += msxvr.readMemory (slot,address);
    return check;
}

int main ()
{
    std::cout << "OpenMSXVR test program\n";
    
    if (!msxvr.init ())
        return -1;
    std::cout << "MSXVR initialized\n";

    msxvr.setLed (openmsx::MSXVR::CAPS,true);
    msxvr.setLed (openmsx::MSXVR::KANA,true);
    msxvr.setLed (openmsx::MSXVR::FDD,true);
    msxvr.setLed (openmsx::MSXVR::PAUSE,true);
    msxvr.setLed (openmsx::MSXVR::UD1,true);
    msxvr.setLed (openmsx::MSXVR::UD2,true);
    msxvr.setLed (openmsx::MSXVR::UD3,true);
    msxvr.setLed (openmsx::MSXVR::UD4,true);
    sleep (1);
    msxvr.setLed (openmsx::MSXVR::CAPS,false);
    msxvr.setLed (openmsx::MSXVR::KANA,false);
    msxvr.setLed (openmsx::MSXVR::FDD,false);
    msxvr.setLed (openmsx::MSXVR::PAUSE,false);
    msxvr.setLed (openmsx::MSXVR::UD1,false);
    msxvr.setLed (openmsx::MSXVR::UD2,false);
    msxvr.setLed (openmsx::MSXVR::UD3,false);
    msxvr.setLed (openmsx::MSXVR::UD4,false);     

    for (int i=0;i<2;i++)
    {
        std::cout << "joystick A: 0x" << std::hex << int(msxvr.readJoystickStatus (0)) << "\n";
        std::cout << "joystick B: 0x" << std::hex << int(msxvr.readJoystickStatus (1)) << "\n";
    }
    for (int i=0;i<2;i++)
    {
        std::cout << "buttons: 0x" << std::hex << int(msxvr.readButtonStatus ()) << "\n";
    }
    std::cout << "VCC On\n";
    msxvr.setCartridgeVCC (true);

    std::cout << "Cassette motor On\n";
    msxvr.setCassetteMotor (true);
    sleep (1);
    std::cout << "Cassette motor Off\n";
    msxvr.setCassetteMotor (false);
    
    for (int i=1;i<=4;i++)
    {
        if (msxvr.readMemory (i,0x4000)==0x41 && 
            msxvr.readMemory (i,0x4001)==0x42)
        {
            std::cout << "ROM found in slot: "<<i<<"\n";
            auto t1 = std::chrono::high_resolution_clock::now();
            std::cout << "checksum: " << checksum (i,0x4000,0x8000) << "\n";
            auto t2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
            std::cout << "Read speed: " << 0x4000 / (duration/1000000.0) << " bytes/second" << std::endl;
        }
        else
            std::cout << "No ROM in slot: "<<i<<"\n";
    }

    if (check_ch376 (0xaa)==0x55)
    {
        std::cout << "MSXUSB cartridge inserted\n";
        for (int i=0;i<=0xff;i++)
        {
            if (check_ch376 (i)!=((i&0xff)^0xff))
                std::cout << "Error inverse check CH376s: " << i << "\n";
        }
    }
    else
        std::cout << "No MSXUSB cartridge inserted\n";

    std::cout << "VCC Off\n";
    msxvr.setCartridgeVCC (false);

    return 0;
}