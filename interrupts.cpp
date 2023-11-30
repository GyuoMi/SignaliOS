#include "interrupts.h"

void printf(char* str);


InterruptHandler::InterruptHandler(uint8_t interruptNumber,InterruptManager* interruptManager)
{
    this->interruptNumber = interruptNumber;
    this->interruptManager = interruptManager;
    interruptManager->handlers[interruptNumber] = this;
}
InterruptHandler::~InterruptHandler()
{
    if(interruptManager->handlers[interruptNumber] == this)
        interruptManager->handlers[interruptNumber] = 0;

}
uint32_t InterruptHandler::HandleInterrupt(uint32_t esp)
{
    return esp;
}


InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];
InterruptManager* InterruptManager::ActiveInterruptManager = 0;

void InterruptManager::SetInterruptDescriptorTableEntry(
	uint8_t interruptNumber,
	uint16_t codeSegmentSelectorOffset,
	void (*handler)(),
	// access rights
	uint8_t DescriptorPrivelegeLevel,
	uint8_t DescriptorType)
{	
	const uint8_t IDT_DESC_PRESENT = 0x80;
	// we must handle each interrupt otherwise it will crash the OS
	interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
	interruptDescriptorTable[interruptNumber].handlerAddressHighBits = ((uint32_t)handler >> 16) & 0xFFFF;
	interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
	interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivelegeLevel&3) << 5);
	interruptDescriptorTable[interruptNumber].reserved = 0;

}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt)
:picMasterCommand(0x20),
 picMasterData(0x21),
 picSlaveCommand(0xA0),
 picSlaveData(0xA1)
{


	uint16_t CodeSegment = gdt->CodeSegmentSelector();
	const uint8_t IDT_INTERRUPT_GATE = 0xE;

	for (uint16_t i = 0; i < 256; i++)
    {
        handlers[i] = 0;
		SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);
    }

	SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
	/* getting an interrupt 21 will jump to interrupts.s HandleInterrupt function with 0x20 (IRQ) + 0x01 (num)
	 which will be pushed when that jumps to int_bottom*/
	SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

	//communicate with pics in order to use idt
	// pressing kb, we get interrupt 1
	// but cpu uses this interrupt number internally for exceptions
	// so tell master pic to add 0x20 to interrupt, slave is told to add 0x28 (both have 8 interrupts to use)
	picMasterCommand.Write(0x11);
	picSlaveCommand.Write(0x11);

	picMasterData.Write(0x20); //20 - 27
	picSlaveData.Write(0x28); // 28 - 30

	picMasterData.Write(0x04); //tells master it is master
	picSlaveData.Write(0x02); //tells slave it is slave
	
	picMasterData.Write(0x01);
	picSlaveData.Write(0x01);

	picMasterData.Write(0x00);
	picSlaveData.Write(0x00);

	//tell processor to user interruptDescriptorTable via pointer
	InterruptDescriptorTablePointer idt;
	idt.size = 256 * sizeof(GateDescriptor) - 1;
	idt.base = (uint32_t)interruptDescriptorTable;
	asm volatile("lidt %0" : : "m" (idt)) ;
}

InterruptManager::~InterruptManager()
{
}

void InterruptManager::Activate()
{
	// start interrupts, kernel has idt (interrupt manager) so we first instantiate it
	// thereafter we instatiate all hardware (need idt so hardware can tell interrupt manager whichever interrupts to handle)
	// lastly we activate the interrupts
	if(ActiveInterruptManager != 0)
		ActiveInterruptManager->Deactivate();
	ActiveInterruptManager = this;
	asm("sti");
}
void InterruptManager::Deactivate()
{

	if(ActiveInterruptManager == this)
	{
		ActiveInterruptManager = 0;
		asm("cli");
	}
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp)
{	
	if(ActiveInterruptManager != 0)
		ActiveInterruptManager->DoHandleInterrupt(interruptNumber, esp);
	return esp;
}

uint32_t InterruptManager::DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp)
{
    if(handlers[interruptNumber] != 0)
    {
        esp = handlers[interruptNumber]->HandleInterrupt(esp);
    }
    else if(interruptNumber != 0x20)
    {    
        char* foo = "UHNHANDLED  INTERRUPT 0x00";
        char* hex = "0123456789ABCDEF";
        foo[22] = hex[(interruptNumber >> 4) & 0x0F];
        foo[23] = hex[interruptNumber & 0x0F];
        printf(foo);
    }

    //we only need to send answers to hardware interrupts
    if(0x20 <= interruptNumber && interruptNumber < 0x30)
    {
        picMasterCommand.Write(0x20);
    if(0x28 <= interruptNumber )
        picSlaveCommand.Write(0x20);
    }
	return esp;
}
