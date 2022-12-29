//
// Author: Aidan Rutherford
//
// Purpose: Compute flight mehanics metrics using Posix
// real-time scheduler. One thread performs the computations
// and the other gets the time the measurement is taken.
// The threads use a mutex lock to prevent race conditions.
//
// Date Created: September 26, 2022
//
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <math.h>

//aircraft attiude metrics
typedef struct _ATTITUDE
{
	double x_acceleration;
	double y_acceleration;
	double z_acceleration;
	double yaw;
	double pitch;
	double roll;
	struct timespec sample_time;
} attitude;

#define MAX_TESTS 100

void* update_attitude_thread(void* void_pointer);
void* set_timestamp_thread(void* void_pointer);
void set_att_measurements();

static attitude att_measurement = { 0 };
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;  

// Name: main
// Purpose: sets up the scheduler and manages threads
// Inputs: void
// Outputs: void
int main()
{
	//thread handles and thread attribute declarations
	pthread_t main_thread;
	pthread_t timer_thread;
	pthread_attr_t main_sched_attr;
	pthread_attr_t timer_sched_attr;
	struct sched_param main_param;
	struct sched_param timer_param;
	pid_t pid = getpid();
	
	//seed random number generator
	srand(time(NULL));

    pthread_attr_init(&main_sched_attr);
	pthread_attr_init(&timer_sched_attr);
	pthread_attr_setinheritsched(&main_sched_attr, 
	                             PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setinheritsched(&timer_sched_attr,
	                             PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&main_sched_attr, SCHED_FIFO);
	pthread_attr_setschedpolicy(&timer_sched_attr, SCHED_FIFO);
	main_param.sched_priority = sched_get_priority_min(SCHED_FIFO);
	timer_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
	
	if (sched_setscheduler(pid, SCHED_FIFO, &main_param))
	{
		perror("Failed to set schedule for thread");
		return EXIT_FAILURE;
	}
	if (sched_setscheduler(pid, SCHED_FIFO, &timer_param))
	{
		perror("Failed to set schedule for thread");
		return EXIT_FAILURE;
	}
	
	//create threads

    if (pthread_create(&main_thread, NULL,
					   &update_attitude_thread, NULL))
    {
        perror("Error creating threads");
        return EXIT_FAILURE;
    }
    if (pthread_create(&timer_thread, NULL,
					   &set_timestamp_thread, NULL))
    {
        perror("Error creating thread");
        return EXIT_FAILURE;
    }

	//join threads with main thread when done
    pthread_join(main_thread, NULL);
	pthread_join(timer_thread, NULL);
	fputs("Test Complete\n", stdout);
	return EXIT_SUCCESS;
}

// Name: update_attitude_thread
// Purpose: computes aircraft attitude metrics
// Inputs: void
// Outputs: void
void* update_attitude_thread(void* void_pointer)
{
	int i = 0;
	for (i = 0; i < MAX_TESTS; ++i)
		if (pthread_mutex_lock(&mutex_lock) && att_measurement.sample_time.tv_sec != 0)
		{
			perror("Mutex lock error");
			return NULL;
		}
		else
		{
            set_att_measurements();
            printf("Test: %d X_Accel: %.3lf Y_Accel: %.3lf Z_Accel: %.3lf Yaw: %.3lf Pitch: %.3lf Roll: %.3lf at time: %ld\n",
                   i,
                   att_measurement.x_acceleration, 
                   att_measurement.y_acceleration,
                   att_measurement.z_acceleration,
                   att_measurement.yaw,
                   att_measurement.pitch,
                   att_measurement.roll,
                   att_measurement.sample_time.tv_sec);
    
			if (pthread_mutex_unlock(&mutex_lock))
			{
				perror("Mutex unlock error");
                return NULL;
			}
		}
    return NULL;
}

// Name: set_timestamp_thread
// Purpose: set the timestamp in the attitude struct
// Inputs: void
// Outputs: void
void* set_timestamp_thread(void* void_pointer)
{
	int i = 0;
	for (i = 0; i < MAX_TESTS; ++i)
		if (pthread_mutex_lock(&mutex_lock))
		{
			perror("Mutex lock error");
			return NULL;
		}
		else
		{
			//modify the timestamp of the attitude
			clock_gettime(CLOCK_MONOTONIC_RAW, 
				&att_measurement.sample_time);
			if (pthread_mutex_unlock(&mutex_lock))
			{
				perror("Mutex unlock error");
                return NULL;
			}
		}
    return NULL;
}

// Name: set_att_measurement
// Purpose: sets the values in the att_measurement struct
// Inputs: void
// Outputs: void
void set_att_measurements()
{
    //use the formula a=v/t
    //velocity (v) is random number between -250 and 250
	//time (t) is given by clock_gettime from the timer thread
	att_measurement.x_acceleration = (double)((rand() % 501) - 250)
		/ (double)att_measurement.sample_time.tv_sec;
	att_measurement.y_acceleration = (double)((rand() % 501) - 250)
		/ (double)att_measurement.sample_time.tv_sec;
	att_measurement.z_acceleration = (double)((rand() % 501) - 250) 
		/ (double)att_measurement.sample_time.tv_sec;
	//calculate pitch
	//pitch = 180 * arctan(xAccel / sqrt(yAccel**2 + zAccel**2)) / pi
	att_measurement.pitch = 180 * atan(att_measurement.x_acceleration / sqrt(att_measurement.y_acceleration * att_measurement.y_acceleration + att_measurement.z_acceleration * att_measurement.z_acceleration)) / M_PI;
	//calculate roll
	//roll = 180 * arctan(yAccel / sqrt(xAccel**2 + zAccel**2)) / pi
	att_measurement.roll = 180 * atan(att_measurement.y_acceleration 
		/ sqrt(att_measurement.x_acceleration 
		* att_measurement.x_acceleration 
		+ att_measurement.z_acceleration 
		* att_measurement.z_acceleration)) / M_PI;
	//calculate yaw
	//yaw = 180 * arctan(zAccel / sqrt(xAccel**2 + yAccel**2)) / pi
	att_measurement.yaw = 180 * atan(att_measurement.z_acceleration 
		/ sqrt(att_measurement.x_acceleration 
		* att_measurement.x_acceleration 
		+ att_measurement.y_acceleration 
		* att_measurement.y_acceleration)) / M_PI;
}
