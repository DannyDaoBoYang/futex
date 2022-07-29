#define __read_mostly __attribute__((__section__(".data..read_mostly")))
struct mm_struct{};
#define PAGE_SHIFT		12
#define _AC(X,Y)	X
#define PAGE_SIZE		(_AC(1, UL) << PAGE_SHIFT)
