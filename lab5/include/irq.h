#ifndef _IRQ_H__
#define _IRQ_H__

#include "gpio.h"
#include "task.h"

#define IRQ_BASIC_PENDING ((volatile unsigned int *)(MMIO_BASE + 0x0000B200))
#define IRQ_PENDING_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B204))
#define IRQ_PENDING_2 ((volatile unsigned int *)(MMIO_BASE + 0x0000B208))
#define FIQ_CONTROL ((volatile unsigned int *)(MMIO_BASE + 0x0000B20C))
#define ENABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B210))
#define ENABLE_IRQS_2 ((volatile unsigned int *)(MMIO_BASE + 0x0000B214))
#define ENABLE_BASIC_IRQS ((volatile unsigned int *)(MMIO_BASE + 0x0000B218))
#define DISABLE_IRQS_1 ((volatile unsigned int *)(MMIO_BASE + 0x0000B21C))
#define DISABLE_IRQS_2 ((volatile unsigned int *)(MMIO_BASE + 0x0000B220))
#define DISABLE_BASIC_IRQS ((volatile unsigned int *)(MMIO_BASE + 0x0000B224))

#define SYSTEM_TIMER_IRQ_0 (1 << 0)
#define SYSTEM_TIMER_IRQ_1 (1 << 1)
#define SYSTEM_TIMER_IRQ_2 (1 << 2)
#define SYSTEM_TIMER_IRQ_3 (1 << 3)

// See BCM2537 ARM Periphera(volatile unsigned int*)ls p113 at
// https://github.com/raspbe(volatile unsigned
// int*)rrypi/documentation/files/1888662/BCM2837-ARM-Peripherals.-.Revised.-.V2-1.pdf

#define AUX_IRQ_MSK (1 << 29)

/* void enable_interrupt_controller( void ); */
void irq_handler();
void uart_read_enqueue(Task *task);
/* irq.S */
void init_irq();
void disable_irq();
void enable_irq();

/* irq.c */
void init_uart_irq();

void (*pop_deffered(void))();

#endif /*_P_IRQ_H */
