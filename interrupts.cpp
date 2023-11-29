#include "interrupts.h"

void printf(char* str);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

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
{
	uint16_t CodeSegment = gdt->CodeSegmentSelector();
	const uint8_t IDT_INTERRUPT_GATE = 0xE;

	for (uint16_t i = 0; i < 256; i++)
		SetInterruptDescriptorTableEntry(i, CodeSegment, &IgnoreInterruptRequest, 0, IDT_INTERRUPT_GATE);


	SetInterruptDescriptorTableEntry(0x20, CodeSegment, &HandleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
	/* getting an interrupt 21 will jump to interrupts.s HandleInterrupt function with 0x20 (IRQ) + 0x01 (num)
	 which will be pushed when that jumps to int_bottom*/
	SetInterruptDescriptorTableEntry(0x21, CodeSegment, &HandleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

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
	asm("sti");
}

uint32_t InterruptManager::handleInterrupt(uint8_t interruptNumber, uint32_t esp)
{	
	printf(" INTERRUPT");
	return esp;
}
