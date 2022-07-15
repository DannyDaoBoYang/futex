/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_X86_FUTEX_H
#define _ASM_X86_FUTEX_H


//#include <linux/futex.h>
#include "../linux/futex.h"
//#include <linux/uaccess.h>
//did not use this one
//#include <asm/asm.h>
#include "asm.h"
//#include <asm/errno.h>
#include "errno.h"
//#include <asm/processor.h>
//#include <asm/smap.h>
#include "smap.h"
#define Efault 14

#define unsafe_atomic_op1(insn, oval, uaddr, oparg, label) \
do {								\
	int oldval = 0, ret;					\
	asm volatile("1:\t" insn "\n"				\
		     "2:\n"					\
		     " .pushsection \"__ex_table\",\"a\"\n"					\
		     " .balign 4\n"								\
		     " .long (" #1b ") - .\n"						\
		     " .long (" #2b ") - .\n"						\
		     DEFINE_EXTABLE_TYPE_REG							\
		     "extable_type_reg reg=" __stringify(%1) ", type=" __stringify(EX_TYPE_EFAULT_REG) " \n"\
		     UNDEFINE_EXTABLE_TYPE_REG						\
		     " .popsection\n" \
		     : "=r" (oldval), "=r" (ret), "+m" (*uaddr)	\
		     : "0" (oparg), "1" (0));	\
	if (ret)						\
		goto label;					\
	*oval = oldval;						\
} while(0) }


#define unsafe_atomic_op2(insn, oval, uaddr, oparg, label)	do {								\
	int oldval = 0, ret, tem;				\
	asm volatile("1:\tmovl	%2, %0\n"			\
		     "2:\tmovl\t%0, %3\n"			\
		     "\t" insn "\n"				\
		     "3:\t" LOCK_PREFIX "cmpxchgl %3, %2\n"	\
		     "\tjnz\t2b\n"				\
		     "4:\n"					\
		     " .pushsection \"__ex_table\",\"a\"\n"					\
		     " .balign 4\n"								\
		     " .long (" #1b ") - .\n"						\
		     " .long (" #4b ") - .\n"						\
		     DEFINE_EXTABLE_TYPE_REG							\
		     "extable_type_reg reg=" __stringify(%1) ", type=" __stringify(EX_TYPE_EFAULT_REG) " \n"\
		     UNDEFINE_EXTABLE_TYPE_REG						\
		     " .popsection\n" \
		     " .pushsection \"__ex_table\",\"a\"\n"					\
		     " .balign 4\n"								\
		     " .long (" #3b ") - .\n"						\
		     " .long (" #4b ") - .\n"						\
		     DEFINE_EXTABLE_TYPE_REG							\
		     "extable_type_reg reg=" __stringify(%1) ", type=" __stringify(EX_TYPE_EFAULT_REG) " \n"\
		     UNDEFINE_EXTABLE_TYPE_REG						\
		     " .popsection\n"\
		     : "=&a" (oldval), "=&r" (ret),		\
		       "+m" (*uaddr), "=&r" (tem)		\
		     : "r" (oparg), "1" (0));			\
	if (ret)						\
		goto label;					\
	*oval = oldval;						\
} while(0)

static __always_inline int arch_futex_atomic_op_inuser(int op, int oparg, int *oval,
		u32 __user *uaddr)
{
	/*
	if (!user_access_begin(uaddr, sizeof(u32)))
		return -EFAULT;
	*/
	switch (op) {
	case FUTEX_OP_SET:
		unsafe_atomic_op1("xchgl %0, %2", oval, uaddr, oparg, Efault);
		break;
	case FUTEX_OP_ADD:
		unsafe_atomic_op1(LOCK_PREFIX "xaddl %0, %2", oval,
				   uaddr, oparg, Efault);
		break;
	case FUTEX_OP_OR:
		unsafe_atomic_op2("orl %4, %3", oval, uaddr, oparg, Efault);
		break;
	case FUTEX_OP_ANDN:
		unsafe_atomic_op2("andl %4, %3", oval, uaddr, ~oparg, Efault);
		break;
	case FUTEX_OP_XOR:
		unsafe_atomic_op2("xorl %4, %3", oval, uaddr, oparg, Efault);
		break;
	default:
		//user_access_end();
		return -ENOSYS;
	}
	//user_access_end();
	return 0;
Efault:
	//user_access_end();
	return -EFAULT;
}

static inline int futex_atomic_cmpxchg_inatomic(u32 *uval, u32 __user *uaddr,
						u32 oldval, u32 newval)
{
	int ret = 0;
	/*
	if (!user_access_begin(uaddr, sizeof(u32)))
		return -EFAULT;
	*/
	asm volatile("\n"
		"1:\t" LOCK_PREFIX "cmpxchgl %3, %2\n"
		"2:\n"
		" .pushsection \"__ex_table\",\"a\"\n"					
		" .balign 4\n"								
		//TODO
		//" .long (" #1b ") - .\n"						
		//" .long (" #2b ") - .\n"						
		DEFINE_EXTABLE_TYPE_REG							
		"extable_type_reg reg=" __stringify(%0) ", type=" __stringify(EX_TYPE_EFAULT_REG) " \n"
		UNDEFINE_EXTABLE_TYPE_REG						
		" .popsection\n"	
		: "+r" (ret), "=a" (oldval), "+m" (*uaddr)
		: "r" (newval), "1" (oldval)
		: "memory"
	);
	//user_access_end();
	*uval = oldval;
	return ret;
}

#endif /* _ASM_X86_FUTEX_H */
