#include "../linux/types.h"
#ifndef _TASK_H
#define _TASK_H
struct mm_struct;
struct task_struct {
	unsigned int			futex_state;
	u64				timer_slack_ns;
	struct mm_struct			*mm;
}*current;
#endif
