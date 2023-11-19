void printf(char* str)
{
    // anything written at this address will be printed on screen
    // each byte in between are used for colour information
    // 4 high bits + 4 low bits, foreground and background of char
    // on start up these are already at a kind of default (white text on black) so it's not needed to set manually
    // note to change for future when doing bg art
    const int screenWidth = 80;
    const int screenHeight = 25;
    const int centerX = screenWidth / 2;
    const int centerY = screenHeight / 2;

    // Calculate the starting position for the ASCII art
    unsigned short* VideoMemory = (unsigned short*)(0xb8000 + centerY * screenWidth + centerX);

    // Set the color attribute for red text on black background
    unsigned short color = 0x4F00;

    for (int i = 0; str[i] != '\0'; ++i)
    {
        // Check for newline character and update position accordingly
        if (str[i] == '\n')
        {
            // Move to the next line
            VideoMemory += screenWidth - centerX;
        }
        else
        {
            // Print the ASCII art letter with red color
            VideoMemory[i] = color | str[i];
        }
    }
} 


// magic number could be used for error checking
extern "C" void kernelMain(const void* multiboot_structure, unsigned int magic_number)
{
    printf("A C H T U N G");

    while(1);
}
