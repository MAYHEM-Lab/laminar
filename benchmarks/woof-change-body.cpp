#ifdef ESP8266
#define CSPOTDEVICE
#endif
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#if defined(CSPOTDEVICE) && defined(ESP8266)
#include "df_interface.h"
#include "ts_type.h"
//#include "ts_array.h"
//#include "ts_matrix.h"
#include "ts_primitive.h"
//#include "ts_string.h"
#else
#include "../df_interface.h"
#include "type_system/ts_type.h"
#include "type_system/types/ts_array.h"
#include "type_system/types/ts_matrix.h"
#include "type_system/types/ts_primitive.h"
#include "type_system/types/ts_string.h"
#endif

#define FIRSTOP 5
#define DUTYCYCLECOUNT 6

#ifdef CSPOTDEVICE
extern "C" {
#endif

void WooFChangeBody() 
{
	struct timeval tm;
	double ts;
	double bench_start_time;
	double bench_end_time;
	int duty_cycle_count = DUTYCYCLECOUNT;
	int i;
	int op_node; 
	int iteration;
	int cycle;
	int state[2];
	int err;
	unsigned long seqno;
	int ns = 1;
	double telemetry_value;
	char telemetry_string[1024];

#ifndef CSPOTDEVICE
	laminar_init();
#endif

	err = WooFGet("WOOFCHANGE.state",(void *)state,0); // get latest value
	if(err < 0) {
		iteration = 1;
		cycle = 0;
	} else {
		iteration = state[0];
		cycle = state[1];
printf("body: restarting i: %d c: %d\n",iteration,cycle);
	}



	if(get_curr_host_id() == 1) {
		memset(telemetry_string,0,sizeof(telemetry_string));
	// made to run as a cronjob -- read stdin for latest telemetry value
		err = read(0,telemetry_string,sizeof(telemetry_string)-1);
		if(err <= 0) {
			printf("woof-change-body: read failed cycle: %d, iteration: %d\n",
				iteration,cycle);
			fflush(stdout);
			exit(1);
		}
		telemetry_value = atof(telemetry_string);
		//gettimeofday(&tm,NULL);
		//bench_start_time = (double)tm.tv_sec + (tm.tv_usec / 1000000.0); // sec
		op_node = cycle + FIRSTOP;
		//struct ts_value operands[duty_cycle_count];
		struct ts_value my_operand;
		set_double(&my_operand,telemetry_value); // simulated measurement
		operand opnd(&my_operand,iteration); // iteration count
		printf("firing operand %d\n",op_node);
		fire_operand(ns,op_node,&opnd); // fire the operand
		cycle++;
		if(cycle == duty_cycle_count) {
			cycle = 0;
			iteration++;
		}
		state[0] = iteration;
		state[1] = cycle;
		if(err >= 0) {
			seqno = WooFPut("WOOFCHANGE.state",NULL,(void *)state);
		}
	}
       /*
	* host 2 waits for the Anaomaly signal
	*/
	if(get_curr_host_id() == 2) {
		operand result;
//		sleep(60);
		int err = get_result(ns, 4, &result, iteration);
		if (err < 0) {
			std::cout << "Failed to read first result " << std::endl;
		} else {
			printf("Anomaly signal for iteration %d: %d\n", iteration, result.operand_value.value.ts_int);
		}
		iteration++;
		state[0] = iteration;
		state[1] = cycle;
		if(err >= 0) {
			seqno = WooFPut("WOOFCHANGE.state",NULL,(void *)state);
		}
	}

    // std::cout << "Size: " << std::string(result_str).size() << std::endl;
    // std::cout << "Type: " << result.operand_value.type;

}
#ifdef CSPOTDEVICE
} // extern C
#endif

#ifndef CSPOTDEVICE
int main(int argc, char **argv) {
	WooFChangeBody();
    return 0;
}
#endif
