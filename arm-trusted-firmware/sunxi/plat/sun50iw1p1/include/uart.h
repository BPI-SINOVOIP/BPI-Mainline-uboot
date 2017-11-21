#ifndef _UART_H_
#define _UART_H_

#define CCM_UART_PORT_OFFSET          16
#define CCM_UART_ADDR_OFFSET          0x400

#define DW_UART_RBR	0x00
#define DW_UART_THR	0x00
#define DW_UART_DLL	0x00
#define DW_UART_IER	0x04
#define DW_UART_DLH	0x04
#define DW_UART_FCR	0x08
#define DW_UART_LCR	0x0c
#define DW_UART_MCR	0x10
#define DW_UART_LSR	0x14
#define DW_UART_MSR	0x18
#define DW_UART_SCH	0x1c

#define   UART_BAUD    115200      // Baud rate for UART
                                   // Compute the divisor factor
// UART Line Control Parameter
#define   PARITY       0           // Parity: 0,2 - no parity; 1 - odd parity; 3 - even parity
#define   STOP         0           // Number of Stop Bit: 0 - 1bit; 1 - 2(or 1.5)bits
#define   DLEN         3           // Data Length: 0 - 5bits; 1 - 6bits; 2 - 7bits; 3 - 8bits

#if DEBUG
void sunxi_serial_init(int uart_port);
void sunxi_serial_exit(void);
void sunxi_serial_putc (char c);
char sunxi_serial_getc (void);
int sunxi_serial_tstc (void);
#else
static inline void sunxi_serial_init(int uart_port) {}
static inline void sunxi_serial_exit(void) {}
static inline void sunxi_serial_putc(char c) {}
static inline char sunxi_serial_getc(void) { return 0;}
static inline int sunxi_serial_tstc(void) { return 0;}
#endif


#endif    /*  #ifndef _UART_H_  */
