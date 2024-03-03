/*
 * implementing the scheduler
 */

#include "sched.h"

#include <spike_interface/atomic.h>

#include "spike_interface/spike_utils.h"

process *ready_queue_head = NULL;

//
// insert a process, proc, into the END of ready queue.
//
void insert_to_ready_queue(process *proc)
{
    // if the queue is empty in the beginning
    if (ready_queue_head == NULL)
    {
        proc->status = READY;
        proc->queue_next = NULL;
        ready_queue_head = proc;
        return;
    }

    // ready queue is not empty
    process *p;
    // browse the ready queue to see if proc is already in-queue
    for (p = ready_queue_head; p->queue_next != NULL; p = p->queue_next)
        if (p == proc)
            return; // already in queue

    // p points to the last element of the ready queue
    if (p == proc)
        return;
    p->queue_next = proc;
    proc->status = READY;
    proc->queue_next = NULL;

    return;
}

//
// choose a proc from the ready queue, and put it to run.
// note: schedule() does not take care of previous current process. If the current
// process is still runnable, you should place it into the ready queue (by calling
// ready_queue_insert), and then call schedule().
//
extern process procs[NPROC];
void schedule()
{
    if (!ready_queue_head)
    {
        // by default, if there are no ready process, and all processes are in the status of
        // FREE and ZOMBIE, we should shutdown the emulated RISC-V machine.
        int should_shutdown = 1;

        for (int i = 0; i < NPROC; i++)
            if ((procs[i].status != FREE) && (procs[i].status != ZOMBIE))
            {
                should_shutdown = 0;
                sprint("ready queue empty, but process %d is not in free/zombie state:%d\n", i, procs[i].status);
            }

        if (should_shutdown)
        {
            sprint("no more ready processes, system shutdown now.\n");
            shutdown(0);
        }
        else
        {
            panic("Not handled: we should let system wait for unfinished processes.\n");
        }
    }

    current = ready_queue_head;
    assert(current->status == READY);
    ready_queue_head = ready_queue_head->queue_next;

    current->status = RUNNING;
    switch_to(current);
}

int sems[SEM_MAX];
// 设置原子变量cnt
int cnt = 0;
static process *waiting_queue[SEM_MAX];
spinlock_t sem_lock[SEM_MAX];
spinlock_t cnt_lock, queue_lock;

static void waiting_queue_push(process *proc, int sem)
{
    spinlock_lock(&queue_lock);
    if (waiting_queue[sem] == NULL)
    {
        proc->status = BLOCKED;
        proc->queue_next = NULL;
        waiting_queue[sem] = proc;
        spinlock_unlock(&queue_lock);
        return;
    }

    process *p;
    for (p = waiting_queue[sem]; p->queue_next != NULL; p = p->queue_next)
        if (p == proc)
        {
            spinlock_unlock(&queue_lock);
            return;
        }

    if (p == proc)
    {
        spinlock_unlock(&queue_lock);
        return;
    }
    p->queue_next = proc;
    proc->status = BLOCKED;
    proc->queue_next = NULL;
    spinlock_unlock(&queue_lock);
    return;
}
static process *waiting_queue_pop(int sem)
{
    spinlock_lock(&queue_lock);
    process *ret = waiting_queue[sem];
    waiting_queue[sem] = ret->queue_next;
    spinlock_unlock(&queue_lock);
    return ret;
}

int sem_init(int val)
{
    spinlock_lock(&cnt_lock); // 新建一个信号量
    sems[cnt] = val;
    int ret = cnt++;
    spinlock_unlock(&cnt_lock);
    return ret;
}

void __acquire(uint64 sem)
{
    spinlock_lock(&sem_lock[sem]);
    sems[sem]--;
    if (sems[sem] < 0)
    {
        waiting_queue_push(current, sem);
        spinlock_unlock(&sem_lock[sem]);
        schedule();
    }
    spinlock_unlock(&sem_lock[sem]);
}

void __release(uint64 sem)
{
    spinlock_lock(&sem_lock[sem]);
    sems[sem]++;
    if (sems[sem] <= 0)
    {
        spinlock_unlock(&sem_lock[sem]);
        process *p = waiting_queue_pop(sem);
        p->status = READY;
        insert_to_ready_queue(p);
    }
    spinlock_unlock(&sem_lock[sem]);
}