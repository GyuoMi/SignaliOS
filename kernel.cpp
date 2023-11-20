#include "types.h"

void printf(char* str)
{
    // anything written at this address will be printed on screen
    // each byte in between are used for colour information
    // 4 high bits + 4 low bits, foreground and background of char
    // on start up these are already at a kind of default (white text on black) so it's not needed to set manually
    // note to change for future when doing bg art
    // Video memory address for VGA text mode
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;

    // Constants for screen dimensions
    int screenWidth = 80;  // Assuming 80 columns in VGA text mode
    int screenHeight = 25; // Assuming 25 rows in VGA text mode

    // Calculate the center position
    int centerRow = screenHeight / 2;
    int centerCol = (screenWidth - 13) / 2;

    // Loop through each character in the input string
    for (int i = 0; str[i] != '\0'; ++i) {
        // Calculate the position in the video memory
        int position = (centerRow * screenWidth) + centerCol + i;

        // Set the ASCII art character at the center of the screen
        VideoMemory[position] = (VideoMemory[position] & 0xFF00) | str[i];

        // Set the colour (alternating between shades of red and white)
        // colours: 0x04 (dark red), 0x0C (light red), 0x07 (white)
        unsigned short colour = ((i % 3) == 0 ? 0x0400 : ((i % 3) == 1 ? 0x0C00 : 0x0700));
        VideoMemory[position] |= colour;
    }
} 
typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
  for (constructor* i = &start_ctors; i != &end_ctors; i++)  
    (*i)();
}



// magic number could be used for error checking
extern "C" void kernelMain(const void* multiboot_structure, uint32_t magic_number)
{
    while(1){
        printf("A C H T U N G");
    }
}
