#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#ifdef ESP8266
#include "df_interface.h"
#include "ts_primitive.h"
#else
#include "../../../df_interface.h"
#include "type_system/types/ts_primitive.h"
#endif
#include "anomaly_test.h"

int anomaly_test_node(const struct ts_value* const* operands,
                         const unsigned int operand_count,
			 struct df_operation_metadata *const operation_metadata,
                         const enum ts_types result_type,
                         struct ts_value* result) {
	int ks_test = operands[0]->value.ts_int; // KS is on port 0
	int regress_test = operands[1]->value.ts_int; // regress on port 1
	int corr_test = operands[2]->value.ts_int; // CORR on port 2
	int vote;
printf("ANOMALY: ks: %d regress: %d corr: %d\n",ks_test,regress_test,corr_test);

	/*
	 * if it fails a ks_test and a regression test but there is
	 * significant correlation, no anomaly
	 */
	//if((ks_test == 0) || (regress_test == 0) || (corr_test == 1)) {
	//if((ks_test == 0) && (regress_test == 0) && (corr_test == 1)) {
	corr_test = 1 - corr_test;
	vote = ks_test + regress_test + corr_test;
	if(vote <= 1) {
//	if((ks_test == 0) && (regress_test == 0) && (corr_test == 0)) {
		result->value.ts_int = 0;
	} else {
		result->value.ts_int = 1;
	}

	return 1;
}

// pass through node for graph inputs in anomaly test
int anomaly_proxy_node(const struct ts_value* const* operands,
                         const unsigned int operand_count,
			 struct df_operation_metadata *const operation_metadata,
                         const enum ts_types result_type,
                         struct ts_value* result) {

			 result->value.ts_double = operands[0]->value.ts_double;

			 return 1;
}

struct anomaly_opt_stc
{
	int port;
	double value;
	int op2_id;
	unsigned long long itr; // for log scan
};

#ifdef CSPOTDEVICE
extern "C" {
#endif

int anomaly_hist_el_size()
{
	return(sizeof(struct anomaly_opt_stc));
}

#ifdef CSPOTDEVICE
}
#endif



int anomaly_optsrc_node(const struct ts_value* const* operands,
                         const unsigned int operand_count,
			 struct df_operation_metadata *const operation_metadata,
                         const enum ts_types result_type,
                         struct ts_value* result) {
	int err;
	int state[6];
	int i;
	struct anomaly_opt_stc aos;
	unsigned long long seqno;
	unsigned long long itr;
	int snk_host_id;
	int op2_id;

 	std::string state_string = "ANOMALY-OPT.state"; 
 	err = woof_get(state_string,(void *)state,0);

//	err = woof_get("ANOMALY-OPT.state",(void *)state,0);
	if(err < 0) {
		printf("anomaly_optsrc_node: could not get state from ANOMALY-OPT.state\n");
		result->value.ts_int = 0;
		return 1;
	}
	snk_host_id = state[5];
	op2_id = state[3];
	
	itr = (unsigned long long)operands[operand_count-1]->value.ts_int; // itr is last operand val
printf("anomaly_optsrc_node: starting itr: %d\n",(int)itr);
	// put the aos structure into remote hist state
	std::string woof_name = generate_woof_host_url(snk_host_id) + "ANOMALY-OPT-HIST.state";
	for(i=0; i < operand_count-1; i++) {
		aos.itr = itr;
		aos.port = i;
		aos.value = operands[i]->value.ts_double;
		aos.op2_id = op2_id;
printf("anomaly_optsrc_node: putting %s itr: %d, port: %d val: %f\n",
		woof_name.c_str(),(int)itr,aos.port,aos.value);
		seqno = woof_put(woof_name,"",(void *)&aos);
printf("anomaly_optsrc_node: put seqno %llu\n",seqno);
	}
	// send the itr and the output

printf("anomaly_optsrc_node: returning %d\n",(int)itr);
//	result->value.ts_double = (double)itr;
	result->value.ts_int = (int)itr;
	return 1;
}

int anomaly_optsnk_node(const struct ts_value* const* operands,
                         const unsigned int operand_count,
			 struct df_operation_metadata *const operation_metadata,
                         const enum ts_types result_type,
                         struct ts_value* result) {
	unsigned long seqno;
	int err;
	struct anomaly_opt_stc aos;
	int op_node;
	int ns2 = 2;
	unsigned long long itr;
//	double values[5];
	int i;

	if(operands[0]->value.ts_int == 0) {
		printf("anomaly_optsnk_node: received failure from src, ending\n");
		return 1;
	}

	itr = (unsigned long long)operands[0]->value.ts_int; // current iteration
printf("anomaly_optsnk_node: starting itr: %d\n",(int)itr);

	seqno = woof_last_seq("ANOMALY-OPT-HIST.state");
	if(seqno <= 0) {
		printf("anomaly_optsnk_node: ANOMALY-OPT_HIST.state not initialized, ending\n");
		return 1;
	}
	err = woof_get("ANOMALY-OPT-HIST.state",(void *)&aos,seqno);
	while(err >= 0) {
		if(aos.itr == itr) { // only fire operands for this iteration
			struct ts_value value;
			set_double(&value,aos.value); // set the value
			operand opnd(&value,itr);
			op_node = aos.port + aos.op2_id; // set the node based on the port in ns2
printf("anomaly_optsnk_node: firing itr: %lld, port: %d op2_id: %d, op_node: %d val: %f\n",
aos.itr,aos.port,aos.op2_id,op_node,aos.value);
			fire_operand(ns2,op_node,&opnd); // fire the opnd in ns2
//			values[aos.port] = aos.value;
		}
		seqno--;
		if(seqno == 0) {
			break;
		}
		err = woof_get("ANOMALY-OPT-HIST.state",(void *)&aos,seqno);
	}

//	for(i=0; i < 5; i++) {
//		struct ts_value value;
//		operand opnd(&value,itr);
//		set_double(&value,values[i]); // set the value
//		op_node = i + aos.op2_id; // set the node based on the port in ns2
//printf("anomaly_optsnk_node: firing itr: %lld, port: %d op2_id: %d, op_node: %d val: %f\n",
//aos.itr,aos.port,aos.op2_id,op_node,aos.value);
//		fire_operand(ns2,op_node,&opnd); // fire the opnd in ns2
//	}

	return 1;

}
