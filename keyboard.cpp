#include "keyboard.h"


KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
dataport(0x60),
commandport(0x64)
{
    while(commandport.Read() & 0x1)
        dataport.Read();
    //tells kb to use interrupts?
    commandport.Write(0xAE); 
    //give curr state
    commandport.Write(0x20);
    //set righmost bit to 1 and clear 5th bit
    uint8_t status = (dataport.Read() | 1) & ~0x10;
    //set/change curr state
    commandport.Write(0x60);
    dataport.Write(status);

    dataport.Write(0xF4);
}

KeyboardDriver::~KeyboardDriver()
{
}

void printf(char*);

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp)
{
    uint8_t key = dataport.Read();
    char* foo = "KEYBOARD 0x00";
    char* hex = "0123456789ABCDEF";
    foo[11] = hex[(key >> 4) & 0x0F];
    foo[12] = hex[key & 0x0F];
    printf(foo);
    return esp;
}
