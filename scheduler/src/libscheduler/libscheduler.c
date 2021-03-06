/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

typedef struct _job_t
{
	int id;
	int priority;
	int initial_time;
	int running_time;
	int remaining_time;
	int interupted_time; //time that a process gets interupted by another process
	int running;
	int first;
	int new;
} job_t;

typedef struct _scheduler_t
{
	int cores;
	int waitTime;
	int responseTime;
	int turnAroundTime;
	int jobsCompleted;
	job_t** currentCore;
	scheme_t scheme;
	priqueue_t queue;
} scheduler_t;
scheduler_t* scheduler;

/**
  Compare functions for each scheme.
 */

// First Come First Serve
int fcfs(const void * a, const void * b) //DONE
{
	/*cast to job type*/
	job_t* joba = *(job_t**)a;
	job_t* jobb = *(job_t**)b;
	
	printf("%d\n", joba->initial_time);
	printf("%d\n", jobb->initial_time);

	if (joba->initial_time < jobb->initial_time)
	{
		printf("%d\n", -1);
		return -1;
	}
	else if (joba->initial_time > jobb->initial_time)
	{
		printf("%d\n", 1);
		return 1;
	}
	else
	{
		printf("%d\n", 0);
		return 0;
	}
}
// First Come First Serve
int rr(const void * a, const void * b) //DONE
{
	/*cast to job type*/
	job_t* joba = *(job_t**)a;
	job_t* jobb = *(job_t**)b;

	if (joba-> new > jobb->new)
	{
		return -1;
	}
	else if (joba->new < jobb->new)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
// Shortest Job First
int sjf(const void * a, const void * b) //TODO
{
	/*cast to job type*/
	job_t* joba = *(job_t**)a;
	job_t* jobb = *(job_t**)b;

	if (joba->running_time < jobb->running_time)
	{
		return -1;
	}
	else if (joba->running_time > jobb->running_time)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

// Preemptive Shortest Job First
int psjf(const void * a, const void * b) //TODO
{
	/*cast to job type*/
	job_t* joba = *(job_t**)a;
	job_t* jobb = *(job_t**)b;

	if (joba->remaining_time < jobb->remaining_time)
	{
		return -1;
	}
	else if (joba->remaining_time > jobb->remaining_time)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//Fixed Priority
int pri(const void * a, const void * b) //TODO
{
	/*cast to job type*/
	job_t* joba = *(job_t**)a;
	job_t* jobb = *(job_t**)b;
	
	if (joba->priority > jobb->priority)
	{
		return 1;
	}
	else if (joba->priority < jobb->priority)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}


/**
  Initalizes the scheduler.
Assumptions:
- You may assume this will be the first scheduler function called.
- You may assume this function will be called once once.
- You may assume that cores is a positive, non-zero number.
- You may assume that scheme is a valid scheduling scheme.
@param cores the number of cores that is available by the scheduler. These cores will be known as currentCore(id=0), currentCore(id=1), ..., currentCore(id=cores-1).
@param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
 */
void scheduler_start_up(int cores, scheme_t scheme)
{
	scheduler = (scheduler_t *)malloc(sizeof(scheduler_t));
	scheduler->cores = cores;
	scheduler->currentCore = (job_t **)malloc(cores * sizeof(job_t *));

	// reset values on startup
	scheduler->waitTime = 0;
	scheduler->responseTime = 0;
	scheduler->turnAroundTime = 0;
	scheduler->jobsCompleted = 0;
	scheduler->scheme = scheme;

	//initializes cores to NULL
	for(int i=0; i<cores; i++) {
		scheduler->currentCore[i] = NULL;
	}

	//Comparators
	if(scheduler -> scheme == FCFS)
	{
		priqueue_init(&scheduler->queue, fcfs);
	}

	else if(scheduler -> scheme == SJF)
	{
		priqueue_init(&scheduler->queue, sjf);
	}

	else if(scheduler -> scheme == PSJF)
	{
		priqueue_init(&scheduler->queue, psjf);
	}

	else if(scheduler -> scheme == PRI)
	{
		priqueue_init(&scheduler->queue, pri);
	}

	else if(scheduler -> scheme == PPRI)
	{
		priqueue_init(&scheduler->queue, pri);
	}

	else if(scheduler -> scheme == RR)
	{
		priqueue_init(&scheduler->queue, rr);
	}
}

/**
  Called when a new job arrives.
  If multiple cores are idle, the job should be assigned to the currentCore with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the currentCore the job should be
  scheduled on. If another job is already running on the currentCore specified,
  this will preempt the currently running job.
Assumptions:
- You may assume that every job wil have a unique arrival time.
@param job_number a globally unique identification number of the job arriving.
@param time the current time of the simulator.
@param running_time the total number of time units this job will run before it will be finished.
@param priority the priority of the job. (The lower the value, the higher the priority.)
@return index of currentCore job should be scheduled on
@return -1 if no scheduling changes should be made.
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	//new job
	job_t* job = malloc(sizeof(job_t));
	job->id = job_number;
	job->priority = priority;
	job->initial_time = time;
	job->running_time = running_time;
	job->remaining_time = running_time;
	job->interupted_time = -1;
	job->running = -1;
	job->new = time;
	job->first = -1;

	for( int i=0; i<scheduler->cores; i++ )
	{
		// assigns the job to an idle core
		if(scheduler->currentCore[i] == NULL)
		{
			scheduler->currentCore[i] = job; //assigns job to idle core
			job->interupted_time = time; //sets interupted time to current time

			job->running = i;
			job->first = time;
			priqueue_offer(&scheduler->queue, job);
			return i;
		}
	}
	
	//check for a preemptive scheduler
	if (scheduler->scheme == PPRI)
	{
		int min = 0;
		
		//loop through cores and find the core with min priority
		for( int i=1; i<scheduler->cores; i++ )
		{
			if (pri(&scheduler->currentCore[min], &scheduler->currentCore[i]) < 0)
			{
				min = i;
			}
		}
		
		//if the current job could replace the other job
		if (pri(&job, &scheduler->currentCore[min]) < 0)
		{
			scheduler->currentCore[min]->interupted_time = time;
			scheduler->currentCore[min]->running = -1;
			
			scheduler->currentCore[min] = job; //assigns job to idle core
			job->interupted_time = time; //sets interupted time to current time

			job->running = 0;
			job->first = time;
			priqueue_offer(&scheduler->queue, job);
			return min;
		}
	}
	else if (scheduler->scheme == PSJF)
	{
		int min = 0;
		
		//loop through cores and find the core with min priority
		for( int i=1; i<scheduler->cores; i++ )
		{
			if (psjf(&scheduler->currentCore[min], &scheduler->currentCore[i]) < 0)
			{
				min = i;
			}
		}
		
		//if the current job could replace the other job
		if (psjf(&job, &scheduler->currentCore[min]) < 0)
		{
			scheduler->currentCore[min]->interupted_time = time;
			scheduler->currentCore[min]->running = -1;
			
			scheduler->currentCore[min] = job; //assigns job to idle core
			job->interupted_time = time; //sets interupted time to current time

			job->running = 0;
			job->first = time;
			priqueue_offer(&scheduler->queue, job);
			return min;
		}
	}
	
	priqueue_offer(&scheduler->queue, job);

	return -1;
}


/**
  Called when a job has completed execution.
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the currentCore free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on currentCore core_id.
  @param core_id the zero-based index of the currentCore where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on currentCore core_id
  @return -1 if currentCore should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	scheduler->jobsCompleted++;

	job_t* temp;

	//pop the job out of the queue
	for (int i = 0; i < priqueue_size(&scheduler->queue); i++)
	{
		if (((job_t*)priqueue_at(&scheduler->queue, i))->id == job_number)
		{
			temp = priqueue_remove_at(&scheduler->queue, i);
		}
	}

	scheduler->currentCore[core_id] = NULL;

	//update the time stuff
	int turn = time - temp->initial_time;
	int wait = time - temp->initial_time - temp->running_time;
	printf("wait first %d initial %d ", temp->first, temp->initial_time);
	int response = temp->first - temp->initial_time;


	scheduler->turnAroundTime += turn;
	scheduler->responseTime += response;
	scheduler->waitTime += wait;

	//loop through queue checking for running = -1
	for (int i = 0; i < priqueue_size(&scheduler->queue); i++)
	{
		if (((job_t*)priqueue_at(&scheduler->queue, i))->running == -1)
		{
			((job_t*)priqueue_at(&scheduler->queue, i))->running = core_id;
			if (((job_t*)priqueue_at(&scheduler->queue, i))->first == -1)
			{
				((job_t*)priqueue_at(&scheduler->queue, i))->first = time;
			}
			scheduler->currentCore[core_id] = (job_t*)priqueue_at(&scheduler->queue, i);
			return ((job_t*)priqueue_at(&scheduler->queue, i))->id;
		}
	}
	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a currentCore.
  If any job should be scheduled to run on the currentCore free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on currentCore core_id.
  @param core_id the zero-based index of the currentCore where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on currentCore cord_id
  @return -1 if currentCore should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	 job_t* temp =	scheduler->currentCore[core_id];
	 temp-> running = -1;
   temp-> new = time;

	 //pop the job out of the queue
 	for (int i = 0; i < priqueue_size(&scheduler->queue); i++)
 	{
 		if (((job_t*)priqueue_at(&scheduler->queue, i)) == temp)
 		{
 			temp = priqueue_remove_at(&scheduler->queue, i);
 		}
 	}

	priqueue_offer(&scheduler->queue, temp);

	for (int i = 0; i < priqueue_size(&scheduler->queue); i++)
	{
		if(	scheduler->currentCore[core_id] == NULL ||
			((job_t*)priqueue_at(&scheduler->queue, i))->running == -1)
		{
			((job_t*)priqueue_at(&scheduler->queue, i))->running = core_id;
			((job_t*)priqueue_at(&scheduler->queue, i))->first = time;
			scheduler->currentCore[core_id] = (job_t*)priqueue_at(&scheduler->queue, i);
			return ((job_t*)priqueue_at(&scheduler->queue, i))->id;
		}
	}
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.
Assumptions:
- This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
@return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return (1.0 * scheduler->waitTime) / scheduler->jobsCompleted;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.
Assumptions:
- This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
@return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return (1.0 * scheduler->turnAroundTime) / scheduler->jobsCompleted;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.
Assumptions:
- This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
@return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return (1.0 * scheduler->responseTime) / scheduler->jobsCompleted;
}


/**
  Free any memory associated with your scheduler.
Assumptions:
- This function will be the last function called in your library.
 */
void scheduler_clean_up()
{
	//clean up mallocs
	free(scheduler->currentCore);
	priqueue_destroy(&scheduler->queue);
	free(scheduler);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:
  2(-1) 4(0) 1(-1)
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
	for (int i = 0; i < priqueue_size(&scheduler->queue); i++)
	{
		printf("%d(%d) ", ((job_t*)priqueue_at(&scheduler->queue, i))->id,  ((job_t*)priqueue_at(&scheduler->queue, i))->priority);
	}
}