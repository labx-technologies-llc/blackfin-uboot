/*
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	cpu.h
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#ifndef _CPU_H_
#define _CPU_H_

#include <command.h>

#define INTERNAL_IRQS (32)
#define NUM_IRQ_NODES 16
#define DEF_INTERRUPT_FLAGS 1

void frio_irq_panic(int reason, struct pt_regs * reg);
extern void dump(struct pt_regs * regs);
void display_excp(void);
asmlinkage void evt_nmi(void);
asmlinkage void evt_exception(void);
asmlinkage void trap(void);
asmlinkage void evt_ivhw(void);
asmlinkage void evt_rst(void);
asmlinkage void evt_timer(void);
asmlinkage void evt_evt7(void);
asmlinkage void evt_evt8(void);
asmlinkage void evt_evt9(void);
asmlinkage void evt_evt10(void);
asmlinkage void evt_evt11(void);
asmlinkage void evt_evt12(void);
asmlinkage void evt_evt13(void);
asmlinkage void evt_soft_int1(void);
asmlinkage void evt_system_call(void);
void frio_irq_panic(int reason, struct pt_regs * regs);
void FRIO_free_irq(unsigned int irq, void *dev_id);
void call_isr(int irq, struct pt_regs * fp);
void FRIO_do_irq(int vec, struct pt_regs *fp);
void FRIO_init_IRQ(void);
void FRIO_enable_irq(unsigned int irq);
void FRIO_disable_irq(unsigned int irq);
extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
/* static irq_handler_t irq_list[SYS_IRQS]; */
/* static irq_node_t nodes[NUM_IRQ_NODES]; */
int FRIO_request_irq(unsigned int irq,
		     void (*handler)(int, void *, struct pt_regs *),
		     unsigned long flags,const char *devname,void *dev_id);

#endif
