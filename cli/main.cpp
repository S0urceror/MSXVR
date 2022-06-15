#include  <iomanip>
#include <algorithm>
#include <string>
#include <iostream>
#include "../lib/MSXVR.hh"

openmsx::MSXVR msxvr;

void printMemory (int slot, int address, int range)
{
    int value;
    uint8_t checksum;
    const int bytecount = 16;
    for (int cnt=0;cnt<range;address++,cnt++)
    {
        if (cnt%bytecount==0)
        {
            if (cnt!=0)
            {
                checksum &= 0xff;
                checksum ^= 0xff;
                checksum++;
                std::cout << std::setfill('0') << std::setw(2) << std::uppercase << int(checksum) << std::endl;
            }
            std::cout << std::hex << ":" << std::setfill('0') << std::setw(2) << bytecount << std::setfill('0') << std::setw(4) << std::uppercase << address << "00";
            checksum = bytecount + (address>>8) + (address&0xff);
        }
        value = msxvr.readMemory (slot,address);
        //value = cnt;
        checksum+=value;
        std::cout << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << value;
    }
    checksum &= 0xff;
    checksum ^= 0xff;
    checksum++;
    std::cout << std::setfill('0') << std::setw(2) << std::uppercase << int(checksum) << std::endl;
    std::cout << ":00000001FF" << std::endl; 
}

int main (int argc, char *argv[]) 
{ 
    int         slot = 0;
    std::string argument;
    int         argidx = 1;             

    if (argc==1)
    {
        std::cout << "OpenMSXVR - command line tool to interact with MSXVR hardware" << std::endl;
        std::cout << "-------------------------------------------------------------" << std::endl;
        std::cout << "Usage: " << std::endl;
        std::cout << " /s    [slot]             - which slot is targeted by print, peek and poke" << std::endl;
        std::cout << " /p    [address],[range]  - print memory contents from address for range bytes" << std::endl;
        std::cout << " /peek [address]          - get contents of memory address" << std::endl;
        std::cout << " /poke [address],[value]  - set contents of memory address to value" << std::endl;
        std::cout << " /inp  [port]             - get contents of IO port" << std::endl;
        std::cout << " /out  [port],[value]     - set contents of IO port to value" << std::endl;
        std::cout << " /on                      - set VCC on" << std::endl;
        std::cout << " /off                     - set VCC off" << std::endl;
        std::cout << " /joystick [n]            - get joystick 1 or 2 status" << std::endl;
        std::cout << " /led  [name],[on/off]    - set LED value" << std::endl;
        std::cout << std::endl;
        return EXIT_SUCCESS;
    } 

    if (!msxvr.init ())
        return EXIT_FAILURE;

    while (argidx < argc)
    {
        argument = argv[argidx];
        std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);

        if (argument[0]=='/')
        {
            std::string command = argument.substr(1);
            if (command=="S")
            {
                // check next argument [slot]
                argidx++;
                if (argidx<argc)
                {
                    argument = argv[argidx];
                    try
                    {
                        slot = std::stoi (argument,nullptr,10);
                    }
                    catch (...)
                    {
                        return EXIT_FAILURE;
                    }
                }
            }
            
            if (command=="P")
            {
                std::string::size_type sz;
                int address,range;

                if (slot==0)
                {
                    std::cout << "specify slot first" << std::endl;
                    return EXIT_FAILURE;
                }
                // check next argument [address],[range]
                argidx++;
                if (argidx<argc)
                {
                    argument = argv[argidx];
                    try
                    {
                        address = std::stoi (argument,&sz,16);
                        range = std::stoi (argument.substr(sz+1),nullptr,16);
                    }
                    catch (...)
                    {
                        return EXIT_FAILURE;
                    }
                }
                printMemory (slot,address,range);
            }
            if (command=="PEEK")
            {
                int address;

                if (slot==0)
                {
                    std::cout << "specify slot first" << std::endl;
                    return EXIT_FAILURE;
                }
                // check next argument [address]
                argidx++;
                if (argidx<argc)
                {
                    argument = argv[argidx];
                    try
                    {
                        address = std::stoi (argument,nullptr,16);
                        std::cout << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << "0x" << int(msxvr.readMemory (slot, address)) << std::endl;
                        std::cout << "OK" << std::endl;
                    }
                    catch (...)
                    {
                        return EXIT_FAILURE;
                    }
                }
            }
            if (command=="POKE")
            {
                int address,value;
                std::string::size_type sz;

                if (slot==0)
                {
                    std::cout << "specify slot first" << std::endl;
                    return EXIT_FAILURE;
                }
                // check next argument [address],[value]
                argidx++;
                if (argidx<argc)
                {
                    argument = argv[argidx];
                    try
                    {
                        address = std::stoi (argument,&sz,16);
                        value = std::stoi (argument.substr(sz+1),nullptr,16);
                        msxvr.writeMemory (slot,address,value);
                        std::cout << "OK" << std::endl;
                    }
                    catch (...)
                    {
                        return EXIT_FAILURE;
                    }
                    
                }
            }
            if (command=="INP")
            {
                int port;
                // check next argument [port]
                argidx++;
                if (argidx<argc)
                {
                    argument = argv[argidx];
                    try
                    {
                        port = std::stoi (argument,nullptr,10);
                        std::cout << std::hex << std::setfill('0') << std::setw(2) << std::uppercase << "0x" << int(msxvr.readIO (port)) << std::endl;
                        std::cout << "OK" << std::endl;
                    }
                    catch (...)
                    {
                        return EXIT_FAILURE;
                    }
                }
            }
            if (command=="OUT")
            {
                int port,value;
                std::string::size_type sz;
                // check next argument [port],[value]
                argidx++;
                if (argidx<argc)
                {
                    argument = argv[argidx];
                    try
                    {
                        port = std::stoi (argument,&sz,16);
                        value = std::stoi (argument.substr(sz+1),nullptr,16);
                        msxvr.writeIO (port,value);
                        std::cout << "OK" << std::endl;
                    }
                    catch (...)
                    {
                        return EXIT_FAILURE;
                    }
                }
            }
            if (command=="ON")
            {
                msxvr.setCartridgeVCC(true);
                std::cout << "OK" << std::endl;
            }
            if (command=="OFF")
            {
                msxvr.setCartridgeVCC(false);
                std::cout << "OK" << std::endl;
            }
            if (command=="JOYSTICK")
            {
                int joystick;
                argidx++;
                if (argidx<argc)
                {
                    argument = argv[argidx];
                    try
                    {
                        joystick = std::stoi (argument,nullptr,10);
                        if (joystick==1 || joystick==2)
                        {
                            std::cout << std::setfill('0') << std::setw(2) << std::uppercase << "0x" << int(msxvr.readJoystickStatus (joystick)) << std::endl;
                            std::cout << "OK" << std::endl;
                        }
                        else
                            return EXIT_FAILURE;
                    }
                    catch (...)
                    {
                        return EXIT_FAILURE;
                    }
                }
            }
            if (command=="LED")
            {
                std::string::size_type sz;
                bool ledstate;
                openmsx::MSXVR::LEDS ledid;
                std::string ledname,ledvalue;
                // check next argument [name],[value]
                argidx++;
                if (argidx<argc)
                {
                    argument = argv[argidx];
                    std::transform(argument.begin(), argument.end(),argument.begin(), ::toupper);
                    try
                    {
                        ledname = argument.substr (0,argument.find_first_of(','));
                        ledvalue = argument.substr (argument.find_first_of(',')+1);
                        std::cout << ledname << ":" << ledvalue;

                        ledstate = ledvalue=="ON"?true:false;
                        if (ledname=="CAPS")
                            ledid = msxvr.CAPS;
                        if (ledname=="KANA")
                            ledid = msxvr.KANA;
                        if (ledname=="PAUSE")
                            ledid = msxvr.PAUSE;
                        if (ledname=="FDD")
                            ledid = msxvr.FDD;
                        if (ledname=="UD1")
                            ledid = msxvr.UD1;
                        if (ledname=="UD2")
                            ledid = msxvr.UD2;
                        if (ledname=="UD3")
                            ledid = msxvr.UD3;
                        if (ledname=="UD4")
                            ledid = msxvr.UD4;
                        msxvr.setLed (ledid,ledstate);
                        std::cout << "OK" << std::endl;
                    }
                    catch (...)
                    {
                        return EXIT_FAILURE;
                    }
                }
            }
        }
        argidx++;
    }

    return EXIT_SUCCESS;
}