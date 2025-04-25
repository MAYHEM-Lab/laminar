#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "regress_test.h"

extern "C" {
#include "array.h"
#include "regress.h"

struct regress_coef_stc
{
	double y_int;
	double slope;
};

} // end extern C

int regress_test_node(const struct ts_value* const* operands,
                         const unsigned int operand_count,
			 struct df_operation_metadata *const operation_metadata,
                         const enum ts_types result_type,
                         struct ts_value* result) {
	// use the same history as the other tests
        std::string history_woof = "REGRESS-node." +
		std::to_string(operation_metadata->node_namespace) + "." +
		std::to_string(operation_metadata->node_id) + "." +
		std::to_string(operation_metadata->host_id);

        std::string regress_woof = "REGRESS-hist." +
		std::to_string(operation_metadata->node_namespace) + "." +
		std::to_string(operation_metadata->node_id) + "." +
		std::to_string(operation_metadata->host_id);


	/*
	 * see if the history woof exists and if it does not, create it
	 * return zero as the KS score
	 */
	double dval;
	double ival;
	double rval;
	int test_val;
	int i;
	int j;
	unsigned long seqno = woof_last_seq(history_woof);
	unsigned long rseqno = woof_last_seq(regress_woof);
	Array2D *x;
	Array2D *y;
	Array1D *coef;
	Array2D *ci;
	unsigned long s;
	int err;
	struct regress_coef_stc rc;

	// if first to fire
	//
	// could be a race condition if multiple nodes get -1 back but probably
	// unlikely
	if(seqno == (unsigned long)-1) {
//		printf("REGRESS: history woof: %s does not exist\n",history_woof.c_str());
		woof_create(history_woof, sizeof(double), operand_count);
		for(i=0; i < operand_count; i++) {
			dval = operands[i]->value.ts_double;
			s = woof_put(history_woof,"",&dval);
//printf("init x: %f at seqno %lu\n",dval,s);
		}
		result->value.ts_int = 0;
		return 1;
	} else if(seqno < operand_count) { // if woof is there but not enough data yet
		result->value.ts_int = 0;
		return 1;
	}
	/*
	 * otherwise, read the previous values from the history woof
	 */
	x = MakeArray2D(operand_count,2);
	if(x == NULL) {
		result->value.ts_double = 0.0;
		return 1;
	}
	y = MakeArray1D(operand_count);
	if(y == NULL) {
		FreeArray2D(x);
		result->value.ts_double = 0.0;
		return 1;
	}
		
	/*
	 * put old data in x
	 *
	 * note that oldest goes in last
	 */
	for(i=0; i < operand_count; i++) {
		err = woof_get(history_woof,&dval,seqno);
		if(err < 0) {
			printf("REGRESS: could get value from %s at seqno %lu\n",
					history_woof.c_str(),
					seqno);
			FreeArray2D(x);
			FreeArray2D(y);
			result->value.ts_double = 0.0;
			return 1;
		}
		x->data[i*2 + 0] = 1.0;
		x->data[i*2 + 1] = dval;
		seqno--;
	}

	/*
	 * line up ys -> operands[0] is oldest and operands[operand_count-1]
	 * is youngest which needs to go in y[0]
	 */
	j = operand_count - 1;
	for(i=0; i < operand_count; i++) {
		dval = operands[j]->value.ts_double;
		y->data[i] = dval;
		j--;
	}

	for(i=0; i < operand_count; i++) {
//printf("x[%d]: %f y[%d]: %f\n",i,x->data[i*2+1],i,y->data[i]);
	}
	/*
	 * generate coefficients
	 */
	coef = RegressMatrix2D(x,y);
	if(coef == NULL) {
		printf("regress_test_node: regression failed\n");
		result->value.ts_int = 0;
	}
	ival = coef->data[0];
	rval = coef->data[1];

	ci = CIBeta(x,coef,y,0.05);
	FreeArray2D(x);
	FreeArray1D(y);
	FreeArray1D(coef);

	/*
	 * if we don't have a previous slope and intercept, no anomaly
	 */
	if(rseqno == (unsigned long)-1) {
		woof_create(regress_woof,sizeof(struct regress_coef_stc),2);
		rc.y_int = ival;
		rc.slope = rval;
		woof_put(regress_woof,"",&rc);
		FreeArray2D(ci);
		result->value.ts_int = 0;
		return 1;
	} else {
		err = woof_get(regress_woof,&rc,rseqno);
		if(err < 0) {
			printf("REGRESS ERROR: could not fetch previous coef from %lu in %s\n",
					rseqno,regress_woof.c_str());
			FreeArray2D(ci);
			result->value.ts_int = 0;
			return 1;
		}
	}

	/*
	 * if previous ival or previous rval in this confidence bound, no anomaly
	 */
#if 0
	if((rc.y_int > ci->data[0 * ci->xdim + 0]) &&
	   (rc.y_int < ci->data[0 * ci->xdim + 1])) {
		test_val = 0;
printf("REGRESS: return 0, y_int: %f, low: %f, high: %f\n",
		rc.y_int,ci->data[0 * ci->xdim + 0],ci->data[0 * ci->xdim + 1]);
	} else if((rc.slope > ci->data[1 * ci->xdim + 0]) &&
	   (rc.slope < ci->data[1 * ci->xdim + 1])) {
		test_val = 0;
printf("REGRESS: return 0, slope: %f, low: %f, high: %f\n",
		rc.slope,ci->data[1 * ci->xdim + 0],ci->data[1 * ci->xdim + 1]);
	} else {
printf("REGRESS: return 1, slope: %f, low: %f, high: %f y_int: %f low: %f high: %f\n",
		rc.slope,ci->data[1 * ci->xdim + 0],ci->data[1 * ci->xdim + 1],
		rc.y_int,ci->data[0 * ci->xdim + 0],ci->data[0 * ci->xdim + 1]);
		test_val = 1;
	}
#endif
	if((rc.y_int > ci->data[0 * ci->xdim + 0]) &&
	   (rc.y_int < ci->data[0 * ci->xdim + 1]) &&
	   (rc.slope > ci->data[1 * ci->xdim + 0]) &&
           (rc.slope < ci->data[1 * ci->xdim + 1])) {
printf("REGRESS: return 0, y_int: %f, low: %f, high: %f\n",
		rc.y_int,ci->data[0 * ci->xdim + 0],ci->data[0 * ci->xdim + 1]);
		test_val = 0;
	} else {
printf("REGRESS: return 1, slope: %f, low: %f, high: %f y_int: %f low: %f high: %f\n",
		rc.slope,ci->data[1 * ci->xdim + 0],ci->data[1 * ci->xdim + 1],
		rc.y_int,ci->data[0 * ci->xdim + 0],ci->data[0 * ci->xdim + 1]);
		test_val = 1;
	}

	FreeArray2D(ci);


	/*
	 * write the new values to the history woof
	 */
	for(i=0; i < operand_count; i++) {
		dval = operands[i]->value.ts_double;
		woof_put(history_woof,"",&dval);
	}

//	printf("REGRESS: found %f %f %d (old: %f %f)\n",
//			ival,rval,test_val,rc.y_int,rc.slope);
	/*
	 * store off this intercept and slope for next time
	 */
	rc.y_int = ival;
	rc.slope = rval;
	woof_put(regress_woof,"",&rc);
	result->value.ts_int = test_val;

	return 1;
}
