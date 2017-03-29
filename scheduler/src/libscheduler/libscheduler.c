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
int fcfs() //DONE
{
	return 1;
}

// Shortest Job First
int sjf(const void * a, const void * b) //TODO
{

	return 0;
}

// Preemptive Shortest Job First
int psjf(const void * a, const void * b) //TODO
{
	return 0;
}

// Preemptive Fixed Priority
int pri(const void * a, const void * b) //TODO
{
	return 0;
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
		priqueue_init(&scheduler->queue, fcfs);
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
	job->interupted_time = 0;


	// used for determining which function will get preempted next
	int max_priority = -1;
	int max_pid = -1;

	
	for( int i=0; i<scheduler->cores; i++ ) 
	{

		// assigns the job to an idle core
		if(scheduler->currentCore[i] == NULL) 
		{
			scheduler->currentCore[i] = job; //assigns job to idle core
			job->interupted_time = time; //sets interupted time to current time
			return i;
		}

		if ( ( scheduler->currentCore[i]->priority > max_priority ) || (scheduler->currentCore[i]->priority == max_priority && scheduler->currentCore[i]->id > max_pid ) ) 
		{
			max_priority = scheduler->currentCore[i]->priority;
			max_pid = scheduler->currentCore[i]->id;
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
return 0;
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
	return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

Assumptions:
- This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
@return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

Assumptions:
- This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
@return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return 0.0;
}


/**
  Free any memory associated with your scheduler.

Assumptions:
- This function will be the last function called in your library.
 */
void scheduler_clean_up()
{

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

}


