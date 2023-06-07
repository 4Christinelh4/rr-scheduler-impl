#include "linux/list.h"
#include "linux/types.h"
#include "sched.h"
#include "pelt.h"

#define TASK_ON_QUEUE 1
#define TASK_OFF_QUEUE 0

/* * * * * * * * * * * * * * * * * * *
 *                                   *
 * This is from round robin branch   *
 *                                   *
 * * * * * * * * * * * * * * * * * * *
 */

// current is a pointer points to the process that is running now
 const struct sched_class comp3520_sched_class;

static void set_next_entity(struct comp3520_rq *q, struct sched_comp3520_entity *se);

static void set_next_task_comp3520(struct rq *rq, struct task_struct *p,
                                   bool first);

static void update_curr_comp3520(struct rq *rq);

static inline struct task_struct *task_of(struct sched_comp3520_entity *comp3520_se) {
    return container_of(comp3520_se, struct task_struct, comp3520_se);
}

static inline struct rq *my_rq_of(struct comp3520_rq *comp3520_rq) {
    return container_of(comp3520_rq, struct rq, comp3520);
}

static void helper(struct comp3520_rq *rq){
    struct list_head *my_head = & rq->entity_list;

    struct sched_comp3520_entity *se = NULL;
    struct task_struct *t;

    int count= 0;

    printk("=================\n");

    list_for_each_entry(se, my_head, list_node_for_this_entity) {

        if (se != NULL){
            t = task_of(se);

            if (t != NULL) {
                printk("[%d] %d\n", count, t->pid);
            }
        }

        count++;
    }
    printk("=================\n");
}

static void update_comp3520_rq_time(struct comp3520_rq *comp3520_rq){
}

static inline struct rq *get_runqueue_from_se(struct sched_comp3520_entity *entity_3520){
    struct task_struct *p = task_of(entity_3520);
    return task_rq(p);
}

static inline struct comp3520_rq *get_comp3520rq_from_se(struct sched_comp3520_entity *entity_3520){
    struct rq *run_queue = get_runqueue_from_se(entity_3520);
    return &(run_queue->comp3520);
}

static void enqueue_3520_entity(struct sched_comp3520_entity *se, struct rq *main_rq){
    // 1. if se.parent is null, should put in rq->comp3520 (se is the parent for its group)
    // 2. if se.parent is not null, should put in parent's my_q
    struct sched_comp3520_entity *parent_se = se->my_parent;
    struct sched_comp3520_entity *cur = se;
    struct comp3520_rq *rq_3520_queue = NULL;

    int finish = 0;

    while (parent_se){

        if (1 == cur->on_rq){
            finish = 1; // no need to enqueue again
            break;
        }

        rq_3520_queue = parent_se->my_q;

        // enqueue cur to parent->my_queue
        list_add_tail(&(cur->list_node_for_this_entity), &(rq_3520_queue->entity_list));
        cur->on_rq = 1;
        cur->queue_that_will_be_queued = rq_3520_queue;

        if (cur->my_q != NULL){

            rq_3520_queue->h_nr_running += cur->my_q->h_nr_running;
        }

        rq_3520_queue->h_nr_running += 1;
        rq_3520_queue->nr_running++; // just enqueue cur

        cur = parent_se;
        parent_se = parent_se->my_parent;

        // parent_se should also be enqueued to 3520_rq
        // if parent_se = NULL, out the loop, and cur is now at the highest level...
    }

    // parent se = NULL
    // now, cur should be enqueued into the TOP 3520 rq!
    if (1 == finish){
        return;
    }

    rq_3520_queue = &(main_rq->comp3520);
    list_add_tail(&(cur->list_node_for_this_entity), &(rq_3520_queue->entity_list));
    rq_3520_queue->h_nr_running += cur->my_q->h_nr_running;

    rq_3520_queue->nr_running++;

    cur->on_rq = 1;
    cur->queue_that_will_be_queued = rq_3520_queue;

    main_rq->nr_running++;
    return;
}

static void enqueue_task_comp3520(struct rq *rq, struct task_struct *p,
				  int flags)
{

    struct comp3520_rq *comp3520_rq = &rq->comp3520;

    // get the schedule entity from the task
    // normally, a se needs to be added to the corresponding runqueue data structure
	struct sched_comp3520_entity *se = &p->comp3520_se;

    // if the entity is already on runqueue, don't need to add to the linkedlist
    if (se->on_rq == 1){
        return;
    }

    // Here I consider group scheduling, which is not needed...
    if (se->my_parent == NULL){

        list_add_tail(&(se->list_node_for_this_entity), &(comp3520_rq->entity_list));

        se->on_rq = 1;
        se->queue_that_will_be_queued = comp3520_rq;

        comp3520_rq->nr_running++;

        if (se->my_q) {
            comp3520_rq->h_nr_running += se->my_q->h_nr_running;
        } else {
            comp3520_rq->h_nr_running++;
        }

        rq->nr_running++;
        flags = ENQUEUE_WAKEUP;
        return;
    }
}

static void dequeue_entity(struct sched_comp3520_entity *se, struct comp3520_rq *comp3520_rq,
        int flags){

    // if se is running now: the entity is not in the list. no need to delete
    if (se != comp3520_rq->curr){
    // remove se from se->queue_it_is_on
        list_del(&(se->list_node_for_this_entity));
    }

    se->on_rq = 0;
    return;
}

static void dequeue_task_comp3520(struct rq *rq, struct task_struct *p,
				  int flags)
{

//    printk("before deque: \n");
    struct comp3520_rq *comp3520_rq = &rq->comp3520;
    struct sched_comp3520_entity *se = &p->comp3520_se;
    struct sched_comp3520_entity *cur = se;

    while (cur){
        // dequeue entity...
        dequeue_entity(cur, comp3520_rq, flags);
        comp3520_rq->nr_running--;

        cur = cur->my_parent;
    }

    rq->nr_running--;
    return;
}

static void yield_task_comp3520(struct rq *rq){

    struct comp3520_rq *my_comp3520_rq = &rq->comp3520;

    my_comp3520_rq->curr = NULL;

//    printk("63: finish yield task comp3520\n");
}

static bool yield_to_task_comp3520(struct rq *rq, struct task_struct *p)
{
//    printk("71: start yield to task comp3520\n");

    struct comp3520_rq *comp3520_rq = &rq->comp3520;
    struct sched_comp3520_entity *se = &p->comp3520_se;

    if (!se->on_rq) {
        return false;
    }

    yield_task_comp3520(rq); // gives up
    return true;
}

// if p ran long enough, preemption will happen
static void check_preempt_curr_comp3520(struct rq *rq, struct task_struct *p,
					int wake_flags) {

}

bool is_single_task_se(struct sched_comp3520_entity *se){
    return (se->my_q == NULL);
}

// return the entity at the first of the queue
static struct sched_comp3520_entity *pick_next_entity(struct rq *rq){

    struct comp3520_rq *my_comp3520_rq = &(rq->comp3520);
    struct list_head *p_head = (my_comp3520_rq->entity_list).next;
    struct sched_comp3520_entity *get_entity;

    if (list_empty(&(my_comp3520_rq->entity_list))){
        return NULL;
    }

    get_entity = list_first_entry(&(my_comp3520_rq->entity_list)
            , struct sched_comp3520_entity, list_node_for_this_entity);

    if (is_single_task_se(get_entity)){

        return get_entity;

    } else {

        my_comp3520_rq = get_entity->my_q;

        struct sched_comp3520_entity *cur_se;

        while (my_comp3520_rq){
            p_head = (my_comp3520_rq->entity_list).next;
            cur_se = list_entry(p_head, struct sched_comp3520_entity, list_node_for_this_entity);
            my_comp3520_rq = cur_se->my_q;
        }

        // cur->se doesnt have my_q. It contains a task.
        return cur_se;
    }
}

// return the task
struct task_struct *pick_next_task_comp3520(struct rq *rq)
{

    struct sched_comp3520_entity *get_task_from_entity = pick_next_entity(rq);
    if (!get_task_from_entity) {
        return NULL;
    }

    struct task_struct *p = task_of(get_task_from_entity);

    set_next_task_comp3520(rq, p, false);
    return p;
}

void put_prev_entity(struct comp3520_rq *comp3520_rq, struct sched_comp3520_entity *prev_se){

    u64 now;
    u64 delta_exec_long;

    now = rq_clock_task(my_rq_of(comp3520_rq));
    delta_exec_long = now - prev_se->start_exec;
    prev_se->sum_exec_time += delta_exec_long;

    if (prev_se->on_rq){ // prev_se is not dequeue_entity
        // enqueue prev_se on prev_se->queue_it_is_on
        list_add_tail(&(prev_se->list_node_for_this_entity), &(comp3520_rq->entity_list));
    }

    comp3520_rq->curr = NULL;
}

// add prev_se to the end of queue
void put_prev_task_comp3520(struct rq *rq, struct task_struct *prev)
{
    // if prev is on rq, it's possible that prev is preempted...
    struct sched_comp3520_entity *se = &(prev->comp3520_se);
    struct sched_comp3520_entity *cur = se;

    // traverse 3520 rq

    while (cur){
        put_prev_entity(&rq->comp3520, cur);
        cur = cur->my_parent;
    }

    return;
}

// remove the entity (returned in pick next entity) from the queue
// set curr to se
static void set_next_entity(struct comp3520_rq *q, struct sched_comp3520_entity *se){
//
//    printk("set next entity: %d\n", se->on_rq);

    if (se->on_rq){

        struct task_struct *my_task = task_of(se);
//        printk("dequeue here, pid = %d\n", my_task->pid);
        list_del(&(se->list_node_for_this_entity));
    }

    se->start_exec = rq_clock_task(my_rq_of(q));
    q->curr = se;
}

// set next task...
static void set_next_task_comp3520(struct rq *rq, struct task_struct *p,
				   bool first)
{
    struct sched_comp3520_entity *se = &p->comp3520_se;

    struct sched_comp3520_entity *cur = se;
    struct comp3520_rq *cur_q;
    while (cur){
        cur_q = cur->queue_that_will_be_queued;

        set_next_entity(cur_q, cur); // set curr entity for the queue, cur -> on rq = 0;

        cur = cur->my_parent;
    }
    return;
}

/*
 * mostly called from time tick functions;
 * it might lead to process switch. This drives the running preemption;
 */
static void task_tick_comp3520(struct rq *rq, struct task_struct *curr,
			       int queued)
{
    struct sched_comp3520_entity *se_curr = &curr->comp3520_se;
    u64 curr_runtime;

    u64 now;

    now = rq_clock_task(rq);
    curr_runtime = now - se_curr->start_exec;
    se_curr->sum_exec_time += curr_runtime; // update the time

    if (se_curr->time_slice -- < 0){
        se_curr->time_slice =RR_INTERVAL;
        resched_curr(rq);
        return;
    }
}

/*
 * notify the scheduler that a new task was spawned
 * when there is a new task forked
 */
static void task_fork_comp3520(struct task_struct *p)
{
    struct comp3520_rq *comp3520_rq;
    struct sched_comp3520_entity *se = &p->comp3520_se;
    struct sched_comp3520_entity *curr = NULL;

    struct rq *rq = this_rq();
    struct rq_flags rf;


    rq_lock(rq, &rf);
    update_rq_clock(rq);

    comp3520_rq = &(task_rq(current)->comp3520);

    curr = comp3520_rq->curr;
    u64 now = rq_clock_task(my_rq_of(comp3520_rq));

    if (curr) {
//        printk("296: curr exists\n");
        // update_comp3520_rq_time(comp3520_rq);
    }

    rq_unlock(rq, &rf);
}

static void prio_changed_comp3520(struct rq *rq, struct task_struct *p,
				  int oldprio)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;

    printk("prio changed\n");
    return;
}


static void switched_from_comp3520(struct rq *rq, struct task_struct *p)
{
	struct comp3520_rq *comp3520_rq;
	struct sched_comp3520_entity *se = &p->comp3520_se;

    printk("switched from\n");
    return;
}


static void switched_to_comp3520(struct rq *rq, struct task_struct *p)
{

    printk("switch to\n");
    return;
}


static unsigned int get_rr_interval_comp3520(struct rq *rq,
					     struct task_struct *task)
{
    return RR_INTERVAL;
}

static void update_curr_comp3520(struct rq *rq)
{
//    printk("207: update curr 3520 starts\n");
//    struct task_struct *current_task = rq->curr;
//    struct sched_comp3520_entity *comp3520_se = &current_task->comp3520_se;

    struct sched_comp3520_entity *the_se = &(rq->curr->comp3520_se);
    struct comp3520_rq *comp3520_rq;
//    u64 curr_exec_time, now, delta_exec_time;

    comp3520_rq = get_comp3520rq_from_se(the_se);
    update_comp3520_rq_time(comp3520_rq);
}

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

#ifdef CONFIG_SMP
		.balance = balance_comp3520,
		.select_task_rq = select_task_rq_comp3520,
		.migrate_task_rq = migrate_task_rq_comp3520,

		.rq_online = rq_online_comp3520,
		.rq_offline = rq_offline_comp3520,

		.task_dead = task_dead_comp3520,
		.set_cpus_allowed = set_cpus_allowed_common,
#endif

		.task_tick = task_tick_comp3520,
		.task_fork = task_fork_comp3520,

		.prio_changed = prio_changed_comp3520,
		.switched_from = switched_from_comp3520,
		.switched_to = switched_to_comp3520,

		.get_rr_interval = get_rr_interval_comp3520,

		.update_curr = update_curr_comp3520,

#ifdef CONFIG_UCLAMP_TASK
		.uclamp_enabled = 1,
#endif

};


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

#ifdef CONFIG_SCHED_DEBUG
extern void print_comp3520_stats(struct seq_file *m, int cpu);
extern void print_comp3520_rq(struct seq_file *m, int cpu, struct rt_rq *rt_rq);
#endif

