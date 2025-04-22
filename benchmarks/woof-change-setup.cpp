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

#ifdef CSPOTDEVICE
extern "C" {
#endif

#define DUTYCYCLECOUNT (6)

void WooFChangeSetUp() 
{
	struct timeval tm;
	double ts;
	double bench_start_time;
	double bench_end_time;
	int duty_cycle_count = DUTYCYCLECOUNT;
	int i;
	int op_node; 
	int iteration;
	int err;
	int state[2];
	unsigned long seqno;



#ifndef ESP8266
	system("sudo rm -f lmr* RE* KS* CO* WOOFCHANE.state");
#endif
	laminar_init();
	laminar_reset();
	seqno = WooFGetLatestSeqno("WOOFCHANGE.state");
	if((seqno == -1) || (seqno == 0)) {
		state[0] = 1; // starting iteration
		state[1] = 0; // starting cycle
		err = WooFCreate("WOOFCHANGE.state",2*sizeof(int),2);
		if(err < 0) {
			printf("woof-change-setup: could not create state woof\n");
			exit(1);
		}
		seqno = WooFPut("WOOFCHANGE.state",NULL,(void *)state);
		if(seqno == -1) {
			printf("woof-change-setup: failed to put state\n");
			exit(1);
		}
	} else {
		printf("woof-change-setup: restarting, setup exiting\n");
		return;
	}

	set_host(2);
//	add_host(1, "169.231.230.148", "/cspot-device-namespace/");
//	add_host(1, "169.231.230.191", "/devices/feather-1/");
//	add_host(1, "169.231.230.191", "/devices/linux@169.231.230.148/");
	add_host(2, "169.231.230.148", "/cspot-device-namespace/");
//	add_host(1, "169.231.230.191", "/devices/linux@pi3-10/");
	add_host(1, "169.231.230.191", "/cspot-device-namespace/");

	int ns = 1;

	// two node computes KS test on node 2
	// two node computes REGRESS test on node 2
	// two node computes CORR test on node 2
	// two node computes ANOMALY test on node 2
	add_node(ns, 1, 1, {DF_CUSTOM, KS_TEST});
	add_node(ns, 1, 2, {DF_CUSTOM, REGRESS_TEST});
	add_node(ns, 1, 3, {DF_CUSTOM, CORR_TEST});
	add_node(ns, 2, 4, {DF_CUSTOM, ANOMALY_TEST});


	// operands come from node 1 -- start with 5
	// use op_node variable to give the virtual node ids
	// note that virtual graph boundary nodes must begin
	// 1 higher than actual nodes
	op_node = duty_cycle_count;
	for(i=0; i < duty_cycle_count; i++) {
		add_operand(ns, 1, op_node); // value from graph boundary on host 1
	/*
	 * subscribe node 1 (KS) node 2 (REGRESS), and node 3 (CORR) on 
	 * host 2 at port i to this operand coming from
	 * host 1 
	 */
		subscribe(ns, 1, i, ns, op_node); 
		subscribe(ns, 2, i, ns, op_node); 
		subscribe(ns, 3, i, ns, op_node); 
		op_node++;
	}

	// subscribe anomaly test node to KS (node 1) on port 0
	subscribe(ns,4,0,ns,1);
	// subscribe anomaly test node to REGRESS (node 2) on port 1
	subscribe(ns,4,1,ns,2);
	// subscribe anomaly test node to CORR (node 3) on port 2
	subscribe(ns,4,2,ns,3);

	laminar_setup();

	return;
}

#ifdef CSPOTDEVICE
}// extern C
#endif

#ifndef CSPOTDEVICE
int main() {
	WooFChangeSetUp();
	return 0;
}
#endif
