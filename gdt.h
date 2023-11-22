#ifndef __GDT_H
#define __GDT_H

    #include "types.h"
    
    class GlobalDescriptorTable
    {
        public:
        // ensures byte perfect, instructs compiler to follow strictly
            class SegmentDescriptor
            {
                private:
                    uint16_t limit_lo;
                    uint16_t base_lo;
                    uint8_t base_hi;
                    uint8_t type;
                    uint8_t limit_hi;
                    uint8_t base_vhi;
                public:
                    SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);
                    // methods to return pointer and limit
                    uint32_t Base();
                    uint32_t Limit();
            } __attribute__((packed)); 

        private:
            SegmentDescriptor nullSegmentSelector;
            SegmentDescriptor unusedSegmentSelector;
            SegmentDescriptor codeSegmentSelector;
            SegmentDescriptor dataSegmentSelector;
        public:
            GlobalDescriptorTable();
            ~GlobalDescriptorTable();

            uint16_t CodeSegmentSelector();
            uint16_t DataSegmentSelector();
    };
#endif