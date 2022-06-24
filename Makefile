# SPDX-License-Identifier: GPL-2.0
everything:
	gcc core.c -o core.o
	gcc requeue.c -o requeue.o
	gcc waitwake.c -o waitwake.o
