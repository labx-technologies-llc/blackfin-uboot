/*
 *
 *	PROJECT				:	BFIN
 *	VERSION				:	2.0
 *	FILE				:	current.h
 *	MODIFIED DATE			:	29 jun 2004
 *	AUTHOR				:	BFin Project-ADI
 *	LOCATION			:	LG Soft India,Bangalore
 */

#ifndef _FRIONOMMU_CURRENT_H
#define _FRIONOMMU_CURRENT_H
/*
 *	current.h
 *	(C) Copyright 2000, Lineo, David McCullough <davidm@lineo.com>
 *
 *	rather than dedicate a register (as the m68k source does), we
 *	just keep a global,  we should probably just change it all to be
 *	current and lose _current_task.
 */

extern struct task_struct *_current_task;
#define get_current()	_current_task
#define current 	_current_task

#endif				/* _FRIONOMMU_CURRENT_H */
