#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "corr_test.h"


double CORR(double *ds1, double *ds2, int size, double *tval)
{

	double f_mu;
	double g_mu;
	double f_var;
	double g_var;
	double f;
	double g;
	double rval;
	double accf;
	double accg;
	double acc;
	double t;
	int i;

	if((size == 0) || (size == 1)) {
		*tval = 0;
		return(0.0);
	}

	/*
	 * compute the means
	 */
	accf = 0;
	accg = 0;
	for(i=0; i < size; i++) {
		accf += ds1[i];
		accg += ds2[i];
	}
	f_mu = accf / (double)size;
	g_mu = accg / (double)size;

	/*
	 * compute the varainces
	 */
	accf = 0;
	accg = 0;
	for(i=0; i < size; i++) {
		accf += ((f_mu - ds1[i]) * (f_mu - ds1[i]));
		accg += ((f_mu - ds2[i]) * (f_mu - ds2[i]));
	}
	f_var = accf / (double)(size-1);
	g_var = accg / (double)(size-1);


	acc = 0;
	for(i=0; i < size; i++) {
		acc += (ds1[i]*ds2[i]);
	}

	rval = ((acc / (double)size) - f_mu*g_mu) / 
		(sqrt(((double)size-1)*f_var/(double)size) * sqrt(((double)size-1)*g_var/(double)size));

	if(rval == 1.0) {
		*tval = 0;
		return(0.0);
	}
	
	t = (rval*sqrt((double)size-2.0)) / (sqrt(1.0 - rval*rval));
	*tval = t;
	return(rval);
}


int corr_test_node(const struct ts_value* const* operands,
                         const unsigned int operand_count,
			 struct df_operation_metadata *const operation_metadata,
                         const enum ts_types result_type,
                         struct ts_value* result) {
        std::string history_woof = "CORR-node." +
		std::to_string(operation_metadata->node_namespace) + "." +
		std::to_string(operation_metadata->node_id) + "." +
		std::to_string(operation_metadata->host_id);

	/*
	 * see if the history woof exists and if it does not, create it
	 * return zero as the KS score
	 */
	double dval;
	int i;
	int j;
	double corr;
	double tval;
	double crit;

	unsigned long seqno = woof_last_seq(history_woof);

	// if first to fire
	//
	// race condition if multiple nodes get -1 but probably unlikely
	if(seqno == (unsigned long)-1) {
//		printf("KS: history woof: %s does not exist\n",history_woof.c_str());
		woof_create(history_woof, sizeof(double), operand_count);
		for(i=0; i < operand_count; i++) {
			dval = operands[i]->value.ts_double;
			woof_put(history_woof,"",&dval);
		}
		result->value.ts_int = 0;
		return 1;
	} else if(seqno < operand_count) { // not enough data yet
		result->value.ts_int = 0;
		return 1;
	}
	/*
	 * otherwise, read the previous values from the history woof
	 */
	double *oldds;
	oldds = (double *)malloc(operand_count * sizeof(double));
	if(oldds == NULL) {
		result->value.ts_int = 0;
		return(1);
	}

	double *newds;
	newds = (double *)malloc(operand_count * sizeof(double));
	if(newds == NULL) {
		free(oldds);
		result->value.ts_int = 0;
		return(1);
	}
		
	/*
	 * youngest in oldds[0]
	 */
	j = operand_count - 1;
	for(i=0; i < operand_count; i++) {
		woof_get(history_woof,&dval,seqno);
		oldds[j] = dval;
		seqno--;
		j--;
	}

	/*
	 * create a dataset with the new values
	 */
	for(i=0; i < operand_count; i++) {
		dval = operands[i]->value.ts_double;
		newds[i] = dval;
	}

	/*
	 * create a KS score
	 */
	tval;
	corr = CORR(oldds,newds,operand_count,&tval);
	free(oldds);
	free(newds);

	/*
	 * write the new values to the history woof
	 */
	for(i=0; i < operand_count; i++) {
		dval = operands[i]->value.ts_double;
		woof_put(history_woof,"",&dval);
	}

	/*
	 * t value, 2-sided, alpha=0.05, 3 df 
	 */
	if(operand_count <= 5) {
		crit = 3.182;
	} else {
		crit = 2.776; // value fo 4 df in case oper cnt is 6
	}

	if(tval < 0) {
		tval = tval * -1.0;
	}
printf("CORR test: corr: %f, t: %f crit: %f\n",corr,tval,crit);

	if((tval > (-1.0*crit)) && (tval < crit)) {
		result->value.ts_int = 0; // no corrrelation
	} else {
		result->value.ts_int = 1;
	}

	return 1;
}
