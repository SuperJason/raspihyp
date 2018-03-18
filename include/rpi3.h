/* 
 * BCM: Broadcom
 * LP: Local Peripherals
 */
#define	BCM_LP_CONTRL_REGISTER		0x40000000
#define	BCM_LP_CORE_TIMER_PRESCALER	0x40000008
#define	BCM_LP_GPU_INT_ROUTINT		0x4000000C 
#define	BCM_LP_PMI_ROUTING_SET		0x40000010 
#define	BCM_LP_PMI_ROUTING_RDCLR	0x40000014 
#define	BCM_LP_CORE_TIMER_LS		0x4000001C 
#define	BCM_LP_CORE_TIMER_MS		0x40000020 
#define	BCM_LP_CORE_INT_ROUTING		0x40000024 
#define	BCM_LP_AXI_CNT			0x4000002C 
#define	BCM_LP_AXI_IRQ			0x40000030 
#define	BCM_LP_LOCAL_TIMER_REG		0x40000034 
#define	BCM_LP_LOCAL_TIMER_FLG		0x40000038 
/* 
 * Timers interrupt control registers
 */
#define	BCM_LP_CORE0_TIMER_IRQCNTL	0x40000040
#define	BCM_LP_CORE1_TIMER_IRQCNTL	0x40000044
#define	BCM_LP_CORE2_TIMER_IRQCNTL	0x40000048
#define	BCM_LP_CORE3_TIMER_IRQCNTL	0x4000004C
/*
 * Where to route timer interrupt to, IRQ/FIQ
 * Setting both the IRQ and FIQ bit gives an FIQ
 */
#define	BCM_LP_TIMER0_IRQ	0x01
#define	BCM_LP_TIMER1_IRQ	0x02
#define	BCM_LP_TIMER2_IRQ	0x04
#define	BCM_LP_TIMER3_IRQ	0x08
#define	BCM_LP_TIMER0_FIQ	0x10
#define	BCM_LP_TIMER1_FIQ	0x20
#define	BCM_LP_TIMER2_FIQ	0x40
#define	BCM_LP_TIMER3_FIQ	0x80
/*
 * Mailbox interrupt control registers
 */
#define	BCM_LP_CORE0_MBOX_IRQCNTL	0x40000050
#define	BCM_LP_CORE1_MBOX_IRQCNTL	0x40000054
#define	BCM_LP_CORE2_MBOX_IRQCNTL	0x40000058
#define	BCM_LP_CORE3_MBOX_IRQCNTL	0x4000005C
/*
 * Where to route mailbox interrupt to, IRQ/FIQ
 * Setting both the IRQ and FIQ bit gives an FIQ
 */
#define	BCM_LP_MBOX0_IRQ	0x01
#define	BCM_LP_MBOX1_IRQ	0x02
#define	BCM_LP_MBOX2_IRQ	0x04
#define	BCM_LP_MBOX3_IRQ	0x08
#define	BCM_LP_MBOX0_FIQ	0x10
#define	BCM_LP_MBOX1_FIQ	0x20
#define	BCM_LP_MBOX2_FIQ	0x40
#define	BCM_LP_MBOX3_FIQ	0x80
/*
 * IRQ & FIQ source registers
 */
#define	BCM_LP_CORE0_IRQ_SOURCE	0x40000060
#define	BCM_LP_CORE1_IRQ_SOURCE	0x40000064
#define	BCM_LP_CORE2_IRQ_SOURCE	0x40000068
#define	BCM_LP_CORE3_IRQ_SOURCE	0x4000006C
#define	BCM_LP_CORE0_FIQ_SOURCE	0x40000070
#define	BCM_LP_CORE1_FIQ_SOURCE	0x40000074
#define	BCM_LP_CORE2_FIQ_SOURCE	0x40000078
#define	BCM_LP_CORE3_FIQ_SOURCE	0x4000007C
/*
 * Interrupt source bits
 * IRQ and FIQ are the same
 * GPU bits can be set for one core only
 */
#define	BCM_LP_INT_SRC_TIMER0	0x00000001
#define	BCM_LP_INT_SRC_TIMER1	0x00000002
#define	BCM_LP_INT_SRC_TIMER2	0x00000004
#define	BCM_LP_INT_SRC_TIMER3	0x00000008
#define	BCM_LP_INT_SRC_MBOX0	0x00000010
#define	BCM_LP_INT_SRC_MBOX1	0x00000020
#define	BCM_LP_INT_SRC_MBOX2	0x00000040
#define	BCM_LP_INT_SRC_MBOX3	0x00000080
#define	BCM_LP_INT_SRC_GPU	0x00000100
#define	BCM_LP_INT_SRC_PMU	0x00000200
/*
 * Mailbox write-set registers (Write only)
 */
#define	BCM_LP_CORE0_MBOX0_SET	0x40000080
#define	BCM_LP_CORE0_MBOX1_SET	0x40000084
#define	BCM_LP_CORE0_MBOX2_SET	0x40000088
#define	BCM_LP_CORE0_MBOX3_SET	0x4000008C
#define	BCM_LP_CORE1_MBOX0_SET	0x40000090
#define	BCM_LP_CORE1_MBOX1_SET	0x40000094
#define	BCM_LP_CORE1_MBOX2_SET	0x40000098
#define	BCM_LP_CORE1_MBOX3_SET	0x4000009C
#define	BCM_LP_CORE2_MBOX0_SET	0x400000A0
#define	BCM_LP_CORE2_MBOX1_SET	0x400000A4
#define	BCM_LP_CORE2_MBOX2_SET	0x400000A8
#define	BCM_LP_CORE2_MBOX3_SET	0x400000AC
#define	BCM_LP_CORE3_MBOX0_SET	0x400000B0
#define	BCM_LP_CORE3_MBOX1_SET	0x400000B4
#define	BCM_LP_CORE3_MBOX2_SET	0x400000B8
#define	BCM_LP_CORE3_MBOX3_SET	0x400000BC

// Mailbox write-clear registers (Read & Write)
#define	BCM_LP_CORE0_MBOX0_RDCLR	0x400000C0
#define	BCM_LP_CORE0_MBOX1_RDCLR	0x400000C4
#define	BCM_LP_CORE0_MBOX2_RDCLR	0x400000C8
#define	BCM_LP_CORE0_MBOX3_RDCLR	0x400000CC
#define	BCM_LP_CORE1_MBOX0_RDCLR	0x400000D0
#define	BCM_LP_CORE1_MBOX1_RDCLR	0x400000D4
#define	BCM_LP_CORE1_MBOX2_RDCLR	0x400000D8
#define	BCM_LP_CORE1_MBOX3_RDCLR	0x400000DC
#define	BCM_LP_CORE2_MBOX0_RDCLR	0x400000E0
#define	BCM_LP_CORE2_MBOX1_RDCLR	0x400000E4
#define	BCM_LP_CORE2_MBOX2_RDCLR	0x400000E8
#define	BCM_LP_CORE2_MBOX3_RDCLR	0x400000EC
#define	BCM_LP_CORE3_MBOX0_RDCLR	0x400000F0
#define	BCM_LP_CORE3_MBOX1_RDCLR	0x400000F4
#define	BCM_LP_CORE3_MBOX2_RDCLR	0x400000F8
#define	BCM_LP_CORE3_MBOX3_RDCLR	0x400000FC
