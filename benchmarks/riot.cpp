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
#include <array>

#include "../df_interface.h"
#include "type_system/ts_type.h"
#include "type_system/types/ts_array.h"
#include "type_system/types/ts_matrix.h"
#include "type_system/types/ts_primitive.h"
#include "type_system/types/ts_string.h"

void Test2Hosts() 
{
    int ns = 1;
    system("sudo rm -f lmr*");
    laminar_init();
    //laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "128.111.39.3:55372", "/home/pi/src/cspot-apps/build/bin/");
    add_host(2, "128.111.39.222:55583", "/home/centos/src/cspot-apps/build/bin/");


    // put annotate node on host 2
    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_ANNOTATE_PRIM});
    // put annotate node on host 2
    add_node(ns, 2, 2, {DF_CUSTOM, RIOT_ANNOTATE_PRIM});

    // put the operand on host 1
    add_operand(ns, 1, 3);

    subscribe(ns, 1, 0, ns, 3);  // Input --> Annotate
    subscribe(ns, 2, 0, ns, 1);  // Annotate --> Annotate

    laminar_setup();

    const char* str = "[{\"n\": \"source_id\",\"t\": \"int\",\"v\": 1},{\"n\": \"acc_chest_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.8184},{\"n\": \"acc_chest_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.009971},{\"n\": \"acc_chest_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.29563},{\"n\": \"ecg_1\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"ecg_2\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"acc_ankle_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 2.1849},{\"n\": \"acc_ankle_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.6967},{\"n\": \"acc_ankle_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.63077},{\"n\": \"gyro_ankle_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.1039},{\"n\": \"gyro_ankle_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.84053},{\"n\": \"gyro_ankle_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.68762},{\"n\": \"mag_ankle_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.37},{\"n\": \"mag_ankle_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.36327},{\"n\": \"mag_ankle_z\",\"t\": \"double\",\"u\": \"local\",\"v\": 0.29963},{\"n\": \"acc_arm_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -8.6499},{\"n\": \"acc_arm_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -4.5781},{\"n\": \"acc_arm_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.18776},{\"n\": \"gyro_arm_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.44902},{\"n\": \"gyro_arm_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -1.0103},{\"n\": \"gyro_arm_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.034483},{\"n\": \"mag_arm_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -2.35},{\"n\": \"mag_arm_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -1.6102},{\"n\": \"mag_arm_z\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.030899},{\"n\": \"label\",\"t\": \"int\",\"v\": 0}]";

	
    if(get_curr_host_id() == 1) {
        struct ts_value operand_value{};
        set_prim_string(&operand_value,str);
    	operand op1(&operand_value, 1);
    	fire_operand(ns, 3, &op1);
    }

    if(get_curr_host_id() == 2) {
    	    operand result;
	    // get theresult from node 1
	    int err = get_result(ns, 2, &result, 1);
	    if (err < 0) {
		std::cout << "Failed to read the result " << std::endl;
	    }
	    printf("get_result completed: %d\n",err);
	    ts_value* loaded_result = load_value(&result.operand_value);
	    printf("loaded_result %s\n",loaded_result->value.ts_prim_string);
	    char *result_str = value_to_prim_string(loaded_result);

	    //char* result_str =
		//static_cast<char*>(malloc(get_string_length(loaded_result) + 1));
	    //get_string(result_str, loaded_result);
	    //std::cout << "Result: " << std::string(result_str) << std::endl;
	    std::cout << "Result: " << std::string(result_str) << std::endl;
    }
    // std::cout << "Size: " << std::string(result_str).size() << std::endl;
    // std::cout << "Type: " << result.operand_value.type;
}

void ETL2Hosts() {
    struct timeval tm;
    double ts;
    double bench_start_time;
    double bench_end_time;

    system("sudo rm -f lmr*");
    laminar_init();
    laminar_reset(); /* reset setup data structures */
    set_host(2);
    add_host(1, "128.111.39.3:55372", "/home/pi/src/cspot-apps/build/bin/");
    add_host(2, "128.111.39.222:55583", "/home/centos/src/cspot-apps/build/bin/");

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_SERIALIZE_SENML_PRIM});
    add_node(ns, 2, 2, {DF_CUSTOM, RIOT_RANGE_FILTER_PRIM});
    add_node(ns, 2, 3, {DF_CUSTOM, RIOT_BLOOM_FILTER_PRIM});
    add_node(ns, 2, 4, {DF_CUSTOM, RIOT_INTERPOLATE_PRIM});
    add_node(ns, 2, 5, {DF_CUSTOM, RIOT_ANNOTATE_PRIM});

    // driver code on Host 2 will take time stamp and fire to an addtion
    // of zero back on Host 1
    add_node(ns, 1, 6, {DF_ARITHMETIC, DF_ARITH_ADDITION});

    add_operand(ns, 1, 7); // initial input is on Host 1
    add_operand(ns, 2, 8); // to trigger final timestamp on Host 2
    add_operand(ns, 1, 9); // for the zero

    subscribe(ns, 1, 0, ns, 7);  // Input --> Serialize
    subscribe(ns, 2, 0, ns, 1);  // Serialize --> Range filter
    subscribe(ns, 3, 0, ns, 2);  // Range filter --> Bloom filter
    subscribe(ns, 4, 0, ns, 3);  // Bloom filter --> Interpolate
    subscribe(ns, 5, 0, ns, 4);  // Interpolate --> Annotate

    subscribe(ns, 6, 0, ns, 8);  // timestamp --> add on Host 1
    subscribe(ns, 6, 1, ns, 9);  // zero --> add on Host 1

    laminar_setup();

    const char* str = "[{\"n\": \"source_id\",\"t\": \"int\",\"v\": 1},{\"n\": \"acc_chest_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.8184},{\"n\": \"acc_chest_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.009971},{\"n\": \"acc_chest_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.29563},{\"n\": \"ecg_1\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"ecg_2\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"acc_ankle_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 2.1849},{\"n\": \"acc_ankle_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.6967},{\"n\": \"acc_ankle_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.63077},{\"n\": \"gyro_ankle_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.1039},{\"n\": \"gyro_ankle_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.84053},{\"n\": \"gyro_ankle_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.68762},{\"n\": \"mag_ankle_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.37},{\"n\": \"mag_ankle_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.36327},{\"n\": \"mag_ankle_z\",\"t\": \"double\",\"u\": \"local\",\"v\": 0.29963},{\"n\": \"acc_arm_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -8.6499},{\"n\": \"acc_arm_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -4.5781},{\"n\": \"acc_arm_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.18776},{\"n\": \"gyro_arm_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.44902},{\"n\": \"gyro_arm_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -1.0103},{\"n\": \"gyro_arm_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.034483},{\"n\": \"mag_arm_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -2.35},{\"n\": \"mag_arm_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -1.6102},{\"n\": \"mag_arm_z\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.030899},{\"n\": \"label\",\"t\": \"int\",\"v\": 0}]";

	
    if(get_curr_host_id() == 1) {
	// first fire a zero into the final add
	struct ts_value zero_value{};
	set_double(&zero_value,0.0);
	operand op_z(&zero_value,1);
	fire_operand(ns, 9, &op_z);

	// now start timer

	gettimeofday(&tm,NULL);
	bench_start_time = (double)tm.tv_sec + (tm.tv_usec / 1000000.0); // sec
	struct ts_value operand_value{};
	set_prim_string(&operand_value,str);
        operand op1(&operand_value, 1);
    	fire_operand(ns, 7, &op1);

	// wait for return timestamp

	operand return_ts;
	int ret_err = get_result(ns,  6, &return_ts, 1);
	if (ret_err < 0) {
		std::cout << "Failed to read the result ts " << std::endl;
	}
	bench_end_time = return_ts.operand_value.value.ts_double;
	printf("START: %f END: %f DURATION: %f (ms)\n",
		bench_start_time,
		bench_end_time,
		(bench_end_time-bench_start_time)*1000.0);
    }

    if(get_curr_host_id() == 2) {
    	    operand result;
	    int err = get_result(ns, 5, &result, 1);
	    if (err < 0) {
		std::cout << "Failed to read the result " << std::endl;
	    }
	    //printf("get_result: err: %d\b",err);
	    ts_value* loaded_result = load_value(&result.operand_value);
	    //printf("loaded_result %s\n",loaded_result->value.ts_prim_string);

	    gettimeofday(&tm,NULL); // get time stamp
	    ts = (double)tm.tv_sec + (double)(tm.tv_usec / 1000000.0); // sec
	    struct ts_value ts_operand{};
	    set_double(&ts_operand,ts);
	    operand op_ts(&ts_operand,1);
	    fire_operand(ns, 8, &op_ts); // send the timestamp back to Host 1

	    char* result_str = value_to_prim_string(loaded_result);
	    std::cout << "Result: " << std::string(result_str) << std::endl;
    }
    // std::cout << "Size: " << std::string(result_str).size() << std::endl;
    // std::cout << "Type: " << result.operand_value.type;
}

void ETL3Hosts() {
    struct timeval tm;
    double ts;
    double bench_start_time;
    double bench_end_time;

    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "128.111.39.3:55372", "/home/pi/src/cspot-apps/build/bin/");
    add_host(2, "128.111.39.222:55583", "/home/centos/src/cspot-apps/build/bin/");
    add_host(3, "169.231.230.185:55583", "/home/centos/src/cspot-apps/build/bin/");

    laminar_init();
    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_SERIALIZE_SENML_PRIM});
    add_node(ns, 2, 2, {DF_CUSTOM, RIOT_RANGE_FILTER_PRIM});
    add_node(ns, 2, 3, {DF_CUSTOM, RIOT_BLOOM_FILTER_PRIM});
    add_node(ns, 2, 4, {DF_CUSTOM, RIOT_INTERPOLATE_PRIM});
    add_node(ns, 3, 5, {DF_CUSTOM, RIOT_ANNOTATE_PRIM});

    // driver code on Host 2 will take time stamp and fire to an addtion
    // of zero back on Host 1 and on Host 2 to sync
    add_node(ns, 2, 6, {DF_ARITHMETIC, DF_ARITH_ADDITION});
    add_node(ns, 1, 7, {DF_ARITHMETIC, DF_ARITH_ADDITION});

    add_operand(ns, 1, 8); // initial input is on Host 1
    add_operand(ns, 3, 9); // to trigger final timestamp on Host 2
    add_operand(ns, 3, 10); // to trigger final timestamp on Host 1
    add_operand(ns, 1, 11); // for the zero
    add_operand(ns, 2, 12); // for the zero

    subscribe(ns, 1, 0, ns, 8);  // Input --> Serialize
    subscribe(ns, 2, 0, ns, 1);  // Serialize --> Range filter
    subscribe(ns, 3, 0, ns, 2);  // Range filter --> Bloom filter
    subscribe(ns, 4, 0, ns, 3);  // Bloom filter --> Interpolate
    subscribe(ns, 5, 0, ns, 4);  // Interpolate --> Annotate

    subscribe(ns, 6, 0, ns, 9);  // timestamp --> add on Host 2
    subscribe(ns, 6, 1, ns, 12);  // zero --> add on Host 2
    subscribe(ns, 7, 0, ns, 10);  // timestamp --> add on Host 1
    subscribe(ns, 7, 1, ns, 11);  // zero --> add on Host 1

    laminar_setup();





    const char* str = "[{\"n\": \"source_id\",\"t\": \"int\",\"v\": 1},{\"n\": \"acc_chest_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.8184},{\"n\": \"acc_chest_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.009971},{\"n\": \"acc_chest_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.29563},{\"n\": \"ecg_1\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"ecg_2\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"acc_ankle_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 2.1849},{\"n\": \"acc_ankle_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.6967},{\"n\": \"acc_ankle_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.63077},{\"n\": \"gyro_ankle_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.1039},{\"n\": \"gyro_ankle_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.84053},{\"n\": \"gyro_ankle_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.68762},{\"n\": \"mag_ankle_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.37},{\"n\": \"mag_ankle_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.36327},{\"n\": \"mag_ankle_z\",\"t\": \"double\",\"u\": \"local\",\"v\": 0.29963},{\"n\": \"acc_arm_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -8.6499},{\"n\": \"acc_arm_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -4.5781},{\"n\": \"acc_arm_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.18776},{\"n\": \"gyro_arm_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.44902},{\"n\": \"gyro_arm_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -1.0103},{\"n\": \"gyro_arm_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.034483},{\"n\": \"mag_arm_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -2.35},{\"n\": \"mag_arm_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -1.6102},{\"n\": \"mag_arm_z\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.030899},{\"n\": \"label\",\"t\": \"int\",\"v\": 0}]";
    int count = 100;

	
    for(unsigned long long i=1; i <= count; i++) {
	    if(get_curr_host_id() == 1) {
		// first fire a zero into the final add
    		struct ts_value zero_value{};
    		set_double(&zero_value,0.0);
    	        operand op_z(&zero_value,i);
		fire_operand(ns, 11, &op_z);

		// now start timer

    		struct ts_value operand_value{};
    		set_prim_string(&operand_value,str);

		gettimeofday(&tm,NULL);
		bench_start_time = (double)tm.tv_sec + (tm.tv_usec / 1000000.0); // sec


		// start the computation

		operand op1(&operand_value, i);
		fire_operand(ns, 8, &op1);

		// wait for return timestamp

    		operand return_ts;
		int ret_err = get_result(ns,  7, &return_ts, i);
		if (ret_err < 0) {
			std::cout << "Failed to read the result ts " << std::endl;
		}
		bench_end_time = return_ts.operand_value.value.ts_double;
		printf("START: %f END: %f DURATION: %f (ms)\n",
			bench_start_time,
			bench_end_time,
			(bench_end_time-bench_start_time)*1000.0);
	    }

#ifdef TWOSYNC
// use this section to sync Host 2 if needed
	    if(get_curr_host_id() == 2) {
		// first fire a zero into the final add
    		struct ts_value zero_value{};
    		set_double(&zero_value,0.0);
		operand op_z(&zero_value,i);
		fire_operand(ns, 12, &op_z);

		// start time on Host 2

		gettimeofday(&tm,NULL);
		bench_start_time = (double)tm.tv_sec + (tm.tv_usec / 1000000.0); // sec

    		operand return_ts;
		int ret_err = get_result(ns,  6, &return_ts, i);
		if (ret_err < 0) {
			std::cout << "Failed to read the result ts " << std::endl;
		}
		bench_end_time = return_ts.operand_value.value.ts_double;
		printf("Host 2 -> Host 3: START: %f END: %f DURATION: %f (ms)\n",
			bench_start_time,
			bench_end_time,
			(bench_end_time-bench_start_time)*1000.0);

	    }
#endif

	    if(get_curr_host_id() == 3) {
    		    operand result;
		    int err = get_result(ns, 5, &result, i);
		    if (err < 0) {
			std::cout << "Failed to read the result " << std::endl;
		    }
		    ts_value* loaded_result = load_value(&result.operand_value);

		    gettimeofday(&tm,NULL); // get time stamp
		    ts = (double)tm.tv_sec + (double)(tm.tv_usec / 1000000.0); // sec
    		    struct ts_value ts_operand3{};
		    set_double(&ts_operand3,ts);
		    operand op_ts3(&ts_operand3,i);
		    fire_operand(ns, 9, &op_ts3); // send the timestamp back to Host 2
		    fire_operand(ns, 10, &op_ts3); // send the timestamp back to Host 1

		    char* result_str = value_to_prim_string(loaded_result);
		    std::cout << "Result: " << std::string(result_str) << std::endl;
	    }
	}
}

void ETL() {
    double bench_start_time;
    double bench_end_time;
    struct timeval tm;

    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_SERIALIZE_SENML});
    add_node(ns, 1, 2, {DF_CUSTOM, RIOT_RANGE_FILTER});
    add_node(ns, 1, 3, {DF_CUSTOM, RIOT_BLOOM_FILTER});
    add_node(ns, 1, 4, {DF_CUSTOM, RIOT_INTERPOLATE});
    add_node(ns, 1, 5, {DF_CUSTOM, RIOT_ANNOTATE});

    add_operand(ns, 1, 6);

    subscribe(ns, 1, 0, ns, 6);  // Input --> Serialize
    subscribe(ns, 2, 0, ns, 1);  // Serialize --> Range filter
    subscribe(ns, 3, 0, ns, 2);  // Range filter --> Bloom filter
    subscribe(ns, 4, 0, ns, 3);  // Bloom filter --> Interpolate
    subscribe(ns, 5, 0, ns, 4);  // Interpolate --> Annotate

    laminar_setup();

    const char* str = "[{\"n\": \"source_id\",\"t\": \"int\",\"v\": 1},{\"n\": \"acc_chest_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.8184},{\"n\": \"acc_chest_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.009971},{\"n\": \"acc_chest_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.29563},{\"n\": \"ecg_1\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"ecg_2\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"acc_ankle_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 2.1849},{\"n\": \"acc_ankle_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.6967},{\"n\": \"acc_ankle_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.63077},{\"n\": \"gyro_ankle_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.1039},{\"n\": \"gyro_ankle_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.84053},{\"n\": \"gyro_ankle_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.68762},{\"n\": \"mag_ankle_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.37},{\"n\": \"mag_ankle_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.36327},{\"n\": \"mag_ankle_z\",\"t\": \"double\",\"u\": \"local\",\"v\": 0.29963},{\"n\": \"acc_arm_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -8.6499},{\"n\": \"acc_arm_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -4.5781},{\"n\": \"acc_arm_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.18776},{\"n\": \"gyro_arm_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.44902},{\"n\": \"gyro_arm_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -1.0103},{\"n\": \"gyro_arm_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.034483},{\"n\": \"mag_arm_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -2.35},{\"n\": \"mag_arm_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -1.6102},{\"n\": \"mag_arm_z\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.030899},{\"n\": \"label\",\"t\": \"int\",\"v\": 0}]";

    struct ts_value* operand_value = value_from_string(str);
    operand op1(operand_value, 1);
    write_value(&op1.operand_value);

    // start timing
    gettimeofday(&tm,NULL);
    bench_start_time = (double)tm.tv_sec + (double)(tm.tv_usec / 1000000.0);

    fire_operand(ns, 6, &op1);
    value_deep_delete(operand_value);

    operand result;
    int err = get_result(ns, 5, &result, 1);
    if (err < 0) {
        std::cout << "Failed to read the result " << std::endl;
    }

    // stop timing
    gettimeofday(&tm,NULL);
    bench_end_time = (double)tm.tv_sec + (double)(tm.tv_usec / 1000000.0);

    ts_value* loaded_result = load_value(&result.operand_value);

    char* result_str =
        static_cast<char*>(malloc(get_string_length(loaded_result) + 1));
    get_string(result_str, loaded_result);
    std::cout << "Result: " << std::string(result_str) << std::endl;
    // std::cout << "Size: " << std::string(result_str).size() << std::endl;
    // std::cout << "Type: " << result.operand_value.type;
    printf("START: %f END: %f DURATION: %f (ms)\n",
	bench_start_time,
	bench_end_time,
	(bench_end_time-bench_start_time)*1000.0);
}

void ETL_stress_test() {
    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_SERIALIZE_SENML});
    add_node(ns, 1, 2, {DF_CUSTOM, RIOT_RANGE_FILTER});
    add_node(ns, 1, 3, {DF_CUSTOM, RIOT_BLOOM_FILTER});
    add_node(ns, 1, 4, {DF_CUSTOM, RIOT_INTERPOLATE});
    add_node(ns, 1, 5, {DF_CUSTOM, RIOT_ANNOTATE});

    add_operand(ns, 1, 6);

    subscribe(ns, 1, 0, ns, 6);  // Input --> Serialize
    subscribe(ns, 2, 0, ns, 1);  // Serialize --> Range filter
    subscribe(ns, 3, 0, ns, 2);  // Range filter --> Bloom filter
    subscribe(ns, 4, 0, ns, 3);  // Bloom filter --> Interpolate
    subscribe(ns, 5, 0, ns, 4);  // Interpolate --> Annotate

    laminar_setup();

    std::string filename =
        "/home/centos/cspot-apps/dataflow_multiwoof/benchmarks/mhealth_dataset/"
        "senml/mHealth_subject1.log";

    std::ifstream input(filename);
    std::string line;

    if (!input.is_open()) {
        std::cout << "Could not open input file" << std::endl;
        exit(1);
    }

    std::ofstream output_file("/home/centos/laminar_riot_results.csv");
    output_file << "run, serialize_node_ms, range_filter_node_ms, bloom_filter_node_ms, interpolate_node_ms, annotate_node_ms\n";

    // Latency

    double sum = 0;
    int count = 1;
    while (getline(input, line)) {
        struct ts_value* operand_value = value_from_string(line.c_str());
        operand op1(operand_value, count);
        write_value(&op1.operand_value);

        const auto now = std::chrono::system_clock::now();
        auto start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             now.time_since_epoch())
                             .count();

        fire_operand(ns, 6, &op1);
        value_deep_delete(operand_value);

        // START TEST
      
        sleep(1);

        std::cout << std::endl;
        output_file << count << ",";

        {
            operand result;
            for (int i = 1; i < 6; i++) {
                get_result(ns, i, &result, count);
                std::cout << i << ": " << (double)(result.timestamp - start_time) / 1e6 << "ms" << std::endl;
                output_file << (double)(result.timestamp - start_time) / 1e6 << (i < 5 ? "," : "");

                start_time = result.timestamp;
            }
        }

        output_file << "\n";

        // END TEST
    
        /*
        operand result;
        int err = get_result(ns, 5, &result, count);
        if (err < 0) {
            std::cout << "Failed to read the result " << std::endl;
        }
        ts_value* loaded_result = load_value(&result.operand_value);

        char* result_str =
            static_cast<char*>(malloc(get_string_length(loaded_result) + 1));
        get_string(result_str, loaded_result);
        std::string res = std::string(result_str);
        size_t start_pos = res.find('~') + 1;
        res = res.substr(start_pos, res.find(' ', start_pos) - start_pos);
        long end_time = std::stol(res);

        // std::cout << "Result: " << std::string(result_str) << std::endl;
        // std::cout << "Obtained result" << std::endl;
        // std::cout << "Start: " << start_time << std::endl;
        // std::cout << "End: " << end_time << std::endl;
        double latency = (double)(end_time - start_time) / 1e6;
        std::cout << "Latency: " << latency << "ms" << std::endl;
        sum += latency;
        */

        count++;
    }

    
    /*
    std::cout << "Average latency: " << sum / (count - 1) << "ms" << std::endl;

    input.clear();
    input.seekg(0);

    // Throughput

    while (getline(input, line)) {
        struct ts_value* operand_value = value_from_string(line.c_str());
        operand op1(operand_value, count);
        write_value(&op1.operand_value);

        const auto now = std::chrono::system_clock::now();
        auto start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             now.time_since_epoch())
                             .count();

        fire_operand(ns, 6, &op1);
        value_deep_delete(operand_value);
    }

    sleep(2);

    // Get last output
    operand result;
    int err = get_result(ns, 5, &result, count);
    if (err < 0) {
        std::cout << "Failed to read the result " << std::endl;
    }
    ts_value* loaded_result = load_value(&result.operand_value);
    char* result_str =
        static_cast<char*>(malloc(get_string_length(loaded_result) + 1));
    get_string(result_str, loaded_result);
    std::string res = std::string(result_str);
    size_t start_pos = res.find('~') + 1;
    res = res.substr(start_pos, res.find(' ', start_pos) - start_pos);
    long last_output_time = std::stol(res);

    // Get first output
    get_result(ns, 5, &result, 1);
    loaded_result = load_value(&result.operand_value);
    get_string(result_str, loaded_result);
    res = std::string(result_str);
    start_pos = res.find('~') + 1;
    res = res.substr(start_pos, res.find(' ', start_pos) - start_pos);
    long first_output_time = std::stol(res);

    // Calculate throughput (number of ops / time interval)
    double time = (double)(last_output_time - first_output_time) / 1e9;
    std::cout << "Throughput: " << (count - 1) / time << " ops/sec" << std::endl;
    */
}

void STATS() {
    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_SERIALIZE_SENML});
    add_node(ns, 1, 2, {DF_CUSTOM, RIOT_AVERAGE});
    add_node(ns, 1, 3, {DF_CUSTOM, RIOT_KALMAN_FILTER});
    add_node(ns, 1, 4, {DF_CUSTOM, RIOT_DISTINCT_COUNT});
    add_node(ns, 1, 5, {DF_CUSTOM, RIOT_REGRESSION});

    add_operand(ns, 1, 6);

    subscribe(ns, 1, 0, ns, 6);  // Input --> Serialize
    subscribe(ns, 2, 0, ns, 1);  // Serialize --> Average
    subscribe(ns, 3, 0, ns, 1);  // Serialize --> Kalman filter
    subscribe(ns, 4, 0, ns, 1);  // Serialize --> Distinct count
    subscribe(ns, 5, 0, ns, 3);  // Kalman filter --> Regression
    // subscribe(ns, 6, 0, ns, 5);  // Regresstion --> Plot

    laminar_setup();


    std::string filename =
        "/home/centos/cspot-apps/dataflow_multiwoof/benchmarks/mhealth_dataset/"
        "senml/mHealth_subject1.log";

    std::ifstream input(filename);
    std::string line;

    if (!input.is_open()) {
        std::cout << "Could not open input file" << std::endl;
        exit(1);
    }

    std::ofstream output_file("/home/centos/laminar_riot_results_stat.csv");
    output_file << "run, serialize_node_ms, average_node_ms, kalman_filter_node_ms, count_node_ms, regression_node_ms\n";

// Latency

    double sum = 0;
    int count = 1;
    while (getline(input, line)) {
        struct ts_value* operand_value = value_from_string(line.c_str());
        operand op1(operand_value, count);
        write_value(&op1.operand_value);

        const auto now = std::chrono::system_clock::now();
        auto start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                             now.time_since_epoch())
                             .count();

        fire_operand(ns, 6, &op1);
        value_deep_delete(operand_value);

        // START TEST
      
        sleep(1);

        std::cout << std::endl;
        output_file << count << ",";

        
        operand result;
        for (int i = 1; i < 6; i++) {
            get_result(ns, i, &result, count);
            std::cout << i << ": " << (double)(result.timestamp - start_time) / 1e6 << "ms" << std::endl;
            output_file << (double)(result.timestamp - start_time) / 1e6 << (i < 5 ? "," : "");

            start_time = result.timestamp;
        }

        output_file << "\n";

        count++;
    }
    /*
    const char* str = "[{\"n\": \"source_id\",\"t\": \"int\",\"v\": 1},{\"n\": \"acc_chest_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.8184},{\"n\": \"acc_chest_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.009971},{\"n\": \"acc_chest_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.29563},{\"n\": \"ecg_1\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"ecg_2\",\"t\": \"double\",\"u\": \"mV\",\"v\": 0.0041863},{\"n\": \"acc_ankle_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 2.1849},{\"n\": \"acc_ankle_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -9.6967},{\"n\": \"acc_ankle_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.63077},{\"n\": \"gyro_ankle_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.1039},{\"n\": \"gyro_ankle_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.84053},{\"n\": \"gyro_ankle_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.68762},{\"n\": \"mag_ankle_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.37},{\"n\": \"mag_ankle_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.36327},{\"n\": \"mag_ankle_z\",\"t\": \"double\",\"u\": \"local\",\"v\": 0.29963},{\"n\": \"acc_arm_x\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -8.6499},{\"n\": \"acc_arm_y\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": -4.5781},{\"n\": \"acc_arm_z\",\"t\": \"double\",\"u\": \"m/s^2\",\"v\": 0.18776},{\"n\": \"gyro_arm_x\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -0.44902},{\"n\": \"gyro_arm_y\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": -1.0103},{\"n\": \"gyro_arm_z\",\"t\": \"double\",\"u\": \"deg/s\",\"v\": 0.034483},{\"n\": \"mag_arm_x\",\"t\": \"double\",\"u\": \"local\",\"v\": -2.35},{\"n\": \"mag_arm_y\",\"t\": \"double\",\"u\": \"local\",\"v\": -1.6102},{\"n\": \"mag_arm_z\",\"t\": \"double\",\"u\": \"local\",\"v\": -0.030899},{\"n\": \"label\",\"t\": \"int\",\"v\": 0}]";

    struct ts_value* operand_value = value_from_string(str);
    operand op1(operand_value, 1);
    write_value(&op1.operand_value);
    fire_operand(ns, 6, &op1);
    value_deep_delete(operand_value);

    operand result;
    int err = get_result(ns, 5, &result, 1);
    if (err < 0) {
        std::cout << "Failed to read the result " << std::endl;
    }
    ts_value* loaded_result = load_value(&result.operand_value);

    double* m_and_b =
            static_cast<double*>(malloc(get_array_size(loaded_result)));
    get_double_array(m_and_b, loaded_result);
    std::cout << "y = " << m_and_b[0] << "x + " << m_and_b[1] << std::endl;
    */
}


void test_average() {
    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_AVERAGE});

    add_operand(ns, 1, 2);

    subscribe(ns, 1, 0, ns, 2);  // Input --> Average

    laminar_setup();

    std::vector<double> vals = {2, 4, 6, 8, 12, 14, 16, 18, 20};

    for (int i = 0; i < vals.size(); i++) {
        std::string input = "test~" + std::to_string(vals[i]);

        struct ts_value* operand_value = value_from_string(input.c_str());
        operand op1(operand_value, i + 1);
        write_value(&op1.operand_value);
        fire_operand(ns, 2, &op1);
        value_deep_delete(operand_value);

        std::cout << "Fired " << i + 1 << std::endl;
        operand result;
        int err = get_result(ns, 1, &result, i + 1);
        if (err < 0) {
            std::cout << "Failed to read the result " << std::endl;
        }
        ts_value* loaded_result = load_value(&result.operand_value);

        char* result_str =
            static_cast<char*>(malloc(get_string_length(loaded_result) + 1));
        get_string(result_str, loaded_result);
        std::cout << "Result: " << std::string(result_str) << std::endl;

        sleep(1);
    }
}


void test_kalman_filter() {
    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_KALMAN_FILTER});

    add_operand(ns, 1, 2);

    subscribe(ns, 1, 0, ns, 2);  // Input --> Kalman filter

    laminar_setup();

    std::vector<double> vals = {3.0, 3.1, 3.2, 3.1, 3.2, 4.0, 4.2, 4.1, 4.0};

    for (int i = 0; i < vals.size(); i++) {
        std::string input = "test~" + std::to_string(vals[i]) + '\t';

        struct ts_value* operand_value = value_from_string(input.c_str());
        operand op1(operand_value, i + 1);
        write_value(&op1.operand_value);
        fire_operand(ns, 2, &op1);
        value_deep_delete(operand_value);

        std::cout << "Fired " << i + 1 << std::endl;

        operand result;
        int err = get_result(ns, 1, &result, i + 1);
        std::cout << "Got result" << std::endl;
        if (err < 0) {
            std::cout << "Failed to read the result " << std::endl;
        }
        ts_value* loaded_result = load_value(&result.operand_value);

        char* result_str =
            static_cast<char*>(malloc(get_string_length(loaded_result) + 1));
        get_string(result_str, loaded_result);
        std::cout << "Result: " << std::string(result_str) << std::endl;

        sleep(1);
    }
}

void test_regression() {
    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_REGRESSION});

    add_operand(ns, 1, 2);

    subscribe(ns, 1, 0, ns, 2);  // Input --> Regression

    laminar_setup();

    constexpr const std::array vals = {1.0, 1.1, 1.2, 1.1, 1.3, 2.0, 2.1};

    for (int i = 0; i < vals.size(); i++) {
        std::string input = "test~" + std::to_string(vals[i]) + '\t';

        struct ts_value* operand_value = value_from_string(input.c_str());
        operand op1(operand_value, i + 1);
        write_value(&op1.operand_value);
        fire_operand(ns, 2, &op1);
        value_deep_delete(operand_value);

        std::cout << "Fired " << i + 1 << std::endl;

        operand result;
        int err = get_result(ns, 1, &result, i + 1);
        std::cout << "Got result" << std::endl;
        if (err < 0) {
            std::cout << "Failed to read the result " << std::endl;
        }
        ts_value* loaded_result = load_value(&result.operand_value);

        double* m_and_b =
            static_cast<double*>(malloc(get_array_size(loaded_result)));
        get_double_array(m_and_b, loaded_result);
        std::cout << "y = " << m_and_b[0] << "x + " << m_and_b[1] << std::endl;

        sleep(1);
    }
}

void test_distinct_count() {
    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_REGRESSION});

    add_operand(ns, 1, 2);

    subscribe(ns, 1, 0, ns, 2);  // Input --> Regression

    laminar_setup();

    constexpr const std::array vals = {1.0, 1.1, 1.2, 1.1, 1.3, 2.0, 2.1};

    for (int i = 0; i < vals.size(); i++) {
        std::string input = "test~" + std::to_string(vals[i]) + '\t';

        struct ts_value* operand_value = value_from_string(input.c_str());
        operand op1(operand_value, i + 1);
        write_value(&op1.operand_value);
        fire_operand(ns, 2, &op1);
        value_deep_delete(operand_value);

        std::cout << "Fired " << i + 1 << std::endl;

        operand result;
        int err = get_result(ns, 1, &result, i + 1);
        std::cout << "Got result" << std::endl;
        if (err < 0) {
            std::cout << "Failed to read the result " << std::endl;
        }
        ts_value* loaded_result = load_value(&result.operand_value);

        int count;
        get_integer(&count, loaded_result);
        std::cout << "Result = " << count << std::endl;

        sleep(1);
    }
}

void test_plot() {
    system("sudo rm -f lmr*");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    add_node(ns, 1, 1, {DF_CUSTOM, RIOT_PLOT});
    add_operand(ns, 1, 2);

    subscribe(ns, 1, 0, ns, 2);  // Input --> Regression

    laminar_setup();

    double input[] = {2.0, 3.0};

    struct ts_value* operand_value = value_from_double_array(input, 2);
    operand op1(operand_value, 1);
    write_value(&op1.operand_value);
    fire_operand(ns, 2, &op1);
    value_deep_delete(operand_value);
}

int main() {
    //Test2Hosts();
    //ETL2Hosts();
    ETL3Hosts();
    // ETL();
    // ETL_stress_test();
    // STATS();

    // test_average();
    // test_kalman_filter();
    // test_regression();
    // test_distinct_count();
    // test_plot();

    return 0;
}
