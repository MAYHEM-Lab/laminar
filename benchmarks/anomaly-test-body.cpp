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

#define INTERVAL (12) // 1 minute

#ifdef CSPOTDEVICE
extern "C" {
#endif

void Anomaly2HostsBody() 
{
	struct timeval tm;
	double ts;
	double bench_start_time;
	double bench_end_time;
	int duty_cycle_count = 5;
	int i;
	int op_node; 
	int iteration;
	int cycle;
	int state[2];
	int err;
	unsigned long seqno;
	int ns = 1;
	double *sim_values = NULL;

#ifndef CSPOTDEVICE
	laminar_init();
#endif

	err = WooFGet("ANOMALY.state",(void *)state,0); // get latest value
	if(err < 0) {
		iteration = 1;
		cycle = 0;
	} else {
		iteration = state[0];
		cycle = state[1];
printf("body: restarting i: %d c: %d\n",iteration,cycle);
	}

	sim_values = (double *)malloc(duty_cycle_count * sizeof(double));
	if(sim_values == NULL) {
		exit(1);
	}
	for(i=0; i < duty_cycle_count; i++) {
		sim_values[i] = rand();
	}
	while(iteration <= 40)
	{
		if(get_curr_host_id() == 1) {
			//gettimeofday(&tm,NULL);
			//bench_start_time = (double)tm.tv_sec + (tm.tv_usec / 1000000.0); // sec
			op_node = cycle + 5;
			//struct ts_value operands[duty_cycle_count];
			struct ts_value my_operand[duty_cycle_count];
			set_double(&my_operand[cycle],sim_values[cycle] * (double)iteration); // simulated measurement
			operand opnd(&my_operand[cycle],iteration); // iteration count
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
				seqno = WooFPut("ANOMALY.state",NULL,(void *)state);
			}
#ifdef CSPOTDEVICE
			break;
#else
			sleep(INTERVAL);
#endif

//			for(i=0; i < duty_cycle_count; i++) {
//				set_double(&operands[i],rand()); // simulated measurement
//				operand opnd(&operands[i],iteration); // iteration count
//				printf("firing operand %d\n",op_node);
//				fire_operand(ns,op_node,&opnd); // fire the operand
//				op_node++;
//				}

		}
	       /*
		* host 2 waits for the KS score
		*/
		if(get_curr_host_id() == 2) {
			operand result;
			sleep(60);
			int err = get_result(ns, 4, &result, iteration);
			if (err < 0) {
				std::cout << "Failed to read first result " << std::endl;
			} else {
				printf("Anomaly Test iteration %d: %d\n", iteration, result.operand_value.value.ts_int);
			}
			iteration++;
			state[0] = iteration;
			state[1] = cycle;
			if(err >= 0) {
				seqno = WooFPut("ANOMALY.state",NULL,(void *)state);
			}
		}
	}

	if(sim_values != NULL) {
		free(sim_values);
	}
    // std::cout << "Size: " << std::string(result_str).size() << std::endl;
    // std::cout << "Type: " << result.operand_value.type;
}

#ifdef CSPOTDEVICE
} // extern C
#endif

#ifndef CSPOTDEVICE
int main() {
	Anomaly2HostsBody();
    return 0;
}
#endif
