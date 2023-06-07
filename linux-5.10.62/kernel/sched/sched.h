


struct sched_class {
#ifdef CONFIG_UCLAMP_TASK
	int uclamp_enabled;
#endif

	void (*enqueue_task)(struct rq *rq, struct task_struct *p, int flags);
	void (*dequeue_task)(struct rq *rq, struct task_struct *p, int flags);
	void (*yield_task)(struct rq *rq);
	bool (*yield_to_task)(struct rq *rq, struct task_struct *p);

	void (*check_preempt_curr)(struct rq *rq, struct task_struct *p,
				   int flags);

	struct task_struct *(*pick_next_task)(struct rq *rq);

	void (*put_prev_task)(struct rq *rq, struct task_struct *p);
	void (*set_next_task)(struct rq *rq, struct task_struct *p, bool first);

#ifdef CONFIG_SMP
	int (*balance)(struct rq *rq, struct task_struct *prev,
		       struct rq_flags *rf);
	int (*select_task_rq)(struct task_struct *p, int task_cpu, int sd_flag,
			      int flags);
	void (*migrate_task_rq)(struct task_struct *p, int new_cpu);

	void (*task_woken)(struct rq *this_rq, struct task_struct *task);

	void (*set_cpus_allowed)(struct task_struct *p,
				 const struct cpumask *newmask);

	void (*rq_online)(struct rq *rq);
	void (*rq_offline)(struct rq *rq);
#endif

	void (*task_tick)(struct rq *rq, struct task_struct *p, int queued);
	void (*task_fork)(struct task_struct *p);
	void (*task_dead)(struct task_struct *p);

	/*
	 * The switched_from() call is allowed to drop rq->lock, therefore we
	 * cannot assume the switched_from/switched_to pair is serliazed by
	 * rq->lock. They are however serialized by p->pi_lock.
	 */
	void (*switched_from)(struct rq *this_rq, struct task_struct *task);
	void (*switched_to)(struct rq *this_rq, struct task_struct *task);
	void (*prio_changed)(struct rq *this_rq, struct task_struct *task,
			     int oldprio);

	unsigned int (*get_rr_interval)(struct rq *rq,
					struct task_struct *task);

	void (*update_curr)(struct rq *rq);

#define TASK_SET_GROUP 0
#define TASK_MOVE_GROUP 1

#ifdef CONFIG_FAIR_GROUP_SCHED
	void (*task_change_group)(struct task_struct *p, int type);
#endif
} __aligned(STRUCT_ALIGNMENT); /* STRUCT_ALIGN(), vmlinux.lds.h */


static inline void put_prev_task(struct rq *rq, struct task_struct *prev)
{
	WARN_ON_ONCE(rq->curr != prev);
	prev->sched_class->put_prev_task(rq, prev);
}

static inline void set_next_task(struct rq *rq, struct task_struct *next)
{
	WARN_ON_ONCE(rq->curr != next);
	next->sched_class->set_next_task(rq, next, false);
}

/* Defined in include/asm-generic/vmlinux.lds.h */
extern struct sched_class __begin_sched_classes[];
extern struct sched_class __end_sched_classes[];

#define sched_class_highest (__end_sched_classes - 1)
#define sched_class_lowest (__begin_sched_classes - 1)

#define for_class_range(class, _from, _to)                                     \
	for (class = (_from); class != (_to); class --)

#define for_each_class(class)                                                  \
	for_class_range(class, sched_class_highest, sched_class_lowest)

extern const struct sched_class stop_sched_class;
extern const struct sched_class dl_sched_class;
extern const struct sched_class rt_sched_class;
extern const struct sched_class fair_sched_class;
extern const struct sched_class comp3520_sched_class;
extern const struct sched_class mlfq_sched_class;
extern const struct sched_class idle_sched_class;


static inline bool sched_stop_runnable(struct rq *rq)
{
	return rq->stop && task_on_rq_queued(rq->stop);
}

static inline bool sched_dl_runnable(struct rq *rq)
{
	return rq->dl.dl_nr_running > 0;
}

static inline bool sched_rt_runnable(struct rq *rq)
{
	return rq->rt.rt_queued > 0;
}

static inline bool sched_fair_runnable(struct rq *rq)
{

//    printk("1913 in fair.c %d\n", rq->cfs.nr_running);
	return rq->cfs.nr_running > 0;
}

extern struct task_struct *pick_next_task_fair(struct rq *rq,
					       struct task_struct *prev,
					       struct rq_flags *rf);
extern struct task_struct *pick_next_task_idle(struct rq *rq);

extern struct task_struct *pick_next_task_comp3520(struct rq *rq);

extern void put_prev_task_comp3520(struct rq *rq, struct task_struct *prev);

extern struct task_struct *pick_next_task_mlfq(struct rq *rq);

extern void put_prev_task_mlfq(struct rq *rq, struct task_struct *prev);
