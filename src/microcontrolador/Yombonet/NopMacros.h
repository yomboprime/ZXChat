
#ifndef __NOP_MACROS__
#define __NOP_MACROS__


#define nop1() __asm__("nop\n\t")
#define nop10() __asm__("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t")
#define nop100() nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();nop10();
#define nopn( n ) for ( int i = 0; i < n; i++ ) { __asm__("nop\n\t"); }


#endif //__NOP_MACROS__