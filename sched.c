#include "sched.h"
#include<stdio.h>


void FCFS (
    int number_of_jobs,
    const int job_submitted_time [],
    const int job_required_time [],
    int job_sched_start []
) {
	int time = 0;
	int tot = number_of_jobs;
	int v[2020];
	int j;
	for (j = 0; j < number_of_jobs; j++) {
		v[j] = 0;
	}
	while (tot) {
		int i;
		int timemin = 0x3f3f3f3f;
		int ansi = -1;
		for (i = 0; i < number_of_jobs; i++) {
			if (v[i] == 0 && job_submitted_time[i] < timemin) {
				timemin = job_submitted_time[i];
				ansi = i;
			}
		}
		if (ansi != -1) {
			v[ansi] = 1;
			tot--;
			if (time < job_submitted_time[ansi]) time = job_submitted_time[ansi];
			job_sched_start[ansi] = time;
			time+=job_required_time[ansi];
		}
	} 
}

void SJF (
    int number_of_jobs,
    const int job_submitted_time [],
    const int job_required_time [],
    int job_sched_start []
) {
	int time = 0;
	int tot = number_of_jobs;
	int v[2020];
	int i;
	for (i = 0; i < number_of_jobs; i++) {
		v[i] = 0;
	}
	while (tot) {
		int reqtime = 0x3f3f3f3f;
		int ansi = -1;
		int mintime = 0x3f3f3f3f;
		for (i = 0; i < number_of_jobs; i++) {
			if (v[i] == 0 && job_submitted_time[i] <= time){
				if (job_required_time[i] < reqtime) {
					reqtime = job_required_time[i];
					ansi = i;
				} else if (job_required_time[i] == reqtime) {
					if (job_submitted_time[i] < job_submitted_time[ansi]) {
						ansi = i;
					}
				}
			} else if (v[i] == 0 && job_submitted_time[i] > time) {
				if (job_submitted_time[i] < mintime) {
					mintime = job_submitted_time[i];
				}
			}
		}
		if (ansi != -1) {
			tot--;
			v[ansi] = 1;
			job_sched_start[ansi] = time;
			time += job_required_time[ansi];
		} else {
			time = mintime;
		}
	}
}
