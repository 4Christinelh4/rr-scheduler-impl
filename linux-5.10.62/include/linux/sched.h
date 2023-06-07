
/*
 * sched_entity for CFS scheduler (the RB tree)
 */
struct sched_entity {
	/* For load-balancing: */
	struct load_weight load;
	struct rb_node run_node;
	struct list_head group_node;
	unsigned int on_rq;

	u64 exec_start;
	u64 sum_exec_runtime;
	u64 vruntime;
	u64 prev_sum_exec_runtime;

	u64 nr_migrations;

	struct sched_statistics statistics;
};

// For tracking priority/(time run) stats
struct sched_comp3520_entity {
    // se is lined by the linux datastructure: struct list_head
    struct list_head list_node_for_this_entity; 
    struct sched_comp3520_entity *my_parent;
    
    struct comp3520_rq *my_q; // for group schedule
    
    short on_rq;

    // on cpu, off cpu info here
    struct comp3520_rq *queue_that_will_be_queued;

    u64 start_exec; // the time at which entity's start executing
    unsigned int sum_exec_time;
    u64 prev_sum_exec_runtime; // to be removed

    int time_slice;

    // Don't worry about this
	struct sched_statistics statistics;
};
