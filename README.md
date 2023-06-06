# rr-scheduler-impl
This is an explanation on how a linux scheduler works. It just highlights some crucial codes for the scheduler.

## kernel/sched
### comp3520.c
`static void enqueue_task_comp3520(struct rq *rq, struct task_struct *p, int flags)`
This function retrieves the schedule entity from the struct task_struct p, and check if the entity is already waiting on the runqueue. If not, use `list_add_tail` to add the entity to the tail of the queue.

### core.c

### sched.h


## include/linux
### sched.h
