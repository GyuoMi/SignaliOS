#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

#include "types.h"
#include "port.h"
#include "gdt.h"


class InterruptManager;

class InterruptHandler
{
    protected:
        uint8_t interruptNumber; 
        InterruptManager* interruptManager;

        InterruptHandler(uint8_t interruptNumber,InterruptManager* interruptManager);
        ~InterruptHandler();
    public:
        uint32_t HandleInterrupt(uint32_t esp);
};

class InterruptManager
{
    friend class InterruptHandler;
	//we have to have a static pointer to the interrupt manager since we're working in a static environment
	protected:
	    static InterruptManager* ActiveInterruptManager;
        InterruptHandler* handlers[256];

    	struct GateDescriptor
	    {
		    uint16_t handlerAddressLowBits;
		    uint16_t gdt_codeSegmentSelector;
		    uint8_t reserved;
		    uint8_t access;
		    uint16_t handlerAddressHighBits;
	    }__attribute__((packed));
    /*When packed attribute is applied to a struct or class, it informs the compiler to minimize the 
    padding between members, ensuring that each member is packed tightly without
    introducing any padding bytes.*/
	    struct InterruptDescriptorTablePointer
	    {
		    uint16_t size;
		    uint32_t base;
	    }__attribute__((packed));

	    static GateDescriptor interruptDescriptorTable[256];

	    static void SetInterruptDescriptorTableEntry(
		    uint8_t interruptNumber,
	    	uint16_t gdt_codeSegmentSelectorOffset,
		    void (*handler)(),
		    // access rights
		    uint8_t DescriptorPrivelegeLevel,
		    uint8_t DescriptorType
	    );


	//ports for communication with PIC (Master & Slave)
	Port8BitSlow picMasterCommand;
	Port8BitSlow picMasterData;
	Port8BitSlow picSlaveCommand;
	Port8BitSlow picSlaveData;

	public:

		InterruptManager(GlobalDescriptorTable* gdt);
		~InterruptManager();

		void Activate();
		void Deactivate();

		static uint32_t handleInterrupt(uint8_t interruptNumber, uint32_t esp);

		uint32_t DoHandleInterrupt(uint8_t interruptNumber, uint32_t esp);

		static void IgnoreInterruptRequest();
		// timer interrupt
		static void HandleInterruptRequest0x00();
		// keyboard
		static void HandleInterruptRequest0x01();

};

#endif
