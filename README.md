# rr-scheduler-impl
This is an explanation on how a linux scheduler works. It just highlights some crucial codes for the scheduler.

## kernel/sched
Normally, there exist group scheduling in Linux. However, this may not need to be considered. <br />
### comp3520.c
`static void enqueue_task_comp3520(struct rq *rq, struct task_struct *p, int flags)`<br />
This function retrieves the schedule entity from the struct task_struct *p, and check if the entity is already waiting on the runqueue. If not, use `list_add_tail` to add the entity to the tail of the queue. <br />

`static void dequeue_task_comp3520(struct rq *rq, struct task_struct *p, int flags)`<br />
In contrast with enqueue_task, this function uses `list_del` to delete the entity associated with task_struct *p from the queue.<br />

`struct task_struct *pick_next_task_comp3520(struct rq *rq)`<br />
By using `pick_next_entity` function, the entity at the front of runqueue is picked. The task associated with the entity is the next struct task_struct that will run.<br />
The `task_of` called Linux function `container_of`, it returns the address of task_struct based on the address of the entity (`comp3520_se`), based on the name of the member in the struct.
It's defined as: 
```
static inline struct task_struct *task_of(struct sched_comp3520_entity *comp3520_se) {
    return container_of(comp3520_se, struct task_struct, comp3520_se);
}
```
`static void set_next_task_comp3520(struct rq *rq, struct task_struct *p, bool first)`<br />


```
const struct sched_class
	comp3520_sched_class __section("__comp3520_sched_class") = {
		.enqueue_task = enqueue_task_comp3520,
		.dequeue_task = dequeue_task_comp3520,
		.yield_task = yield_task_comp3520,
		.yield_to_task = yield_to_task_comp3520,

		.check_preempt_curr = check_preempt_curr_comp3520,

		.pick_next_task = pick_next_task_comp3520,
		.put_prev_task = put_prev_task_comp3520,
		.set_next_task = set_next_task_comp3520, 

        /*
         * ... other implementations of function points ...
         */
}
```
Here, the sched_class is constructed. 

### core.c
In `sched_init`, `init_comp3520_rq(&rq->comp3520)` is called, which is defined in comp3520.c. It sets the task at the front of this rq as NULL, and initialised the entity_list, which is basically a linkedlist. 
```
void init_comp3520_rq(struct comp3520_rq *comp3520_rq)
{
	comp3520_rq->nr_running = 0;
    comp3520_rq->h_nr_running = 0;
    comp3520_rq->curr = NULL;

    comp3520_rq->total_run_time = 0;

    // Don't forget to initialize the list comp3520 list
    INIT_LIST_HEAD(&(comp3520_rq->entity_list)); // this is empty!!!!

    /*
     * comp3520_rq->entity_list should be initialised
     * ___________     ______________    ______________
     *|empty head| -> |list_head 1  | ->|list_head 2  | -> ...so on
     *|__________|    |3520 entity 1|   |3520 entity 2|
     *                |__(group)____|   |__(single)___|
     *
     *
     */
}
```
The priority of different sched_class is shown here:
```
	BUG_ON(&idle_sched_class + 1 != &comp3520_sched_class ||
	       &comp3520_sched_class + 1 != &fair_sched_class ||
	       &fair_sched_class + 1 != &rt_sched_class ||
	       &rt_sched_class + 1 != &dl_sched_class);
```

### sched.h


## include/linux
### sched.h
