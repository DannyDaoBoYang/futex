#include "../linux/spinlock_types_raw.h"
#include "../linux/rbtree_types.h"
#include "task.h"
struct rt_mutex_base {
	raw_spinlock_t		wait_lock;
	struct rb_root_cached   waiters;
	struct task_struct	*owner;
};
