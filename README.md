# rr-scheduler-impl
This is an explanation on how a linux scheduler works. It just highlights some crucial codes for the scheduler.

## kernel/sched
Normally, there exist group scheduling in Linux. However, this may not need to be considered.
### comp3520.c
`static void enqueue_task_comp3520(struct rq *rq, struct task_struct *p, int flags)`<br />
This function retrieves the schedule entity from the struct task_struct *p, and check if the entity is already waiting on the runqueue. If not, use `list_add_tail` to add the entity to the tail of the queue. <br />

`static void dequeue_task_comp3520(struct rq *rq, struct task_struct *p, int flags)`<br />
In contrast with enqueue_task, this function uses `list_del` to delete the entity associated with task_struct *p from the queue. <br />

`struct task_struct *pick_next_task_comp3520(struct rq *rq)`<br />
By using `pick_next_entity` function, the entity at the front of runqueue is picked. The task associated with the entity is the next struct task_struct that will run.<br />
`task_of(get_task_from_entity)`
The `task_of` called Linux function `container_of`, it returns the address of task_struct based on the address of the entity (`comp3520_se`), based on the name of the member in the struct.
```

static inline struct task_struct *task_of(struct sched_comp3520_entity *comp3520_se) {
    return container_of(comp3520_se, struct task_struct, comp3520_se);
}

```

`static void set_next_task_comp3520(struct rq *rq, struct task_struct *p, bool first)`<br />



### core.c

### sched.h


## include/linux
### sched.h
