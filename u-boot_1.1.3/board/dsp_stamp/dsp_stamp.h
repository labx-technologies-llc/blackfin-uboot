#ifndef __STAMP_H__
#define __STAMP_H__

extern void init_Flags(void);

extern volatile unsigned long *ambctl0;
extern volatile unsigned long *ambctl1;
extern volatile unsigned long *amgctl;

extern unsigned long pll_div_fact;
extern void serial_setbrg(void);
extern void pll_set(int vco, int crystal_frq, int pll_div);


#endif
