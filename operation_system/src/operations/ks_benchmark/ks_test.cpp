#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include "ks_test.h"

extern "C" {

#include "redblack.h"

#define DEFAULT_BLOCKSIZE (10)



struct data_set_stc
{
	double **data;
	int current;
	int data_c;
	int space_size;
	int block_size;
	int fields;
};

typedef struct data_set_stc DataSet;

int
InitDataSet(void **cookie, int fields)
{
	DataSet *ds;
	int i;
	
	ds = (DataSet *)malloc(sizeof(DataSet));
	if(ds == NULL)
	{
		fprintf(stderr,
		"InitDataSet: couldn't malloc space for data_set on open\n");
		fflush(stderr);
		exit(1);
	}

	ds->block_size = DEFAULT_BLOCKSIZE;
	
	
	ds->fields = fields;
	ds->data =(double **)malloc(fields*sizeof(double *));

	if(ds->data == NULL)
	{
		fprintf(stderr,
	"InitDataSet: can't malloc %d elements on init\n",DEFAULT_BLOCKSIZE);
		fflush(stderr);
		exit(1);
	}

	for(i=0; i < fields; i++)
	{
		ds->data[i] = (double *)malloc(ds->block_size*sizeof(double));
		if(ds->data[i] == NULL)
		{
			exit(1);
		}
	}

	ds->data_c = 0;
	ds->current = -1;
	ds->space_size = ds->block_size;

	*cookie = (void *)ds;
	
	return(1);
}

void SetBlockSize(void *cookie, int size)
{
	DataSet *ds = (DataSet *)cookie;

	ds->block_size = size;
	return;
}


void FreeDataSet(void *cookie)
{
	DataSet *ds = (DataSet *)cookie;
	int i;

	for(i=0; i < ds->fields; i++)
	{
		free(ds->data[i]);
	}
	free(ds->data);
	free(ds);
}


int SizeOf(void *cookie)
{
	DataSet *ds = (DataSet *)cookie;

	return(ds->data_c);
}

void Rewind(void *cookie)
{
	DataSet *ds = (DataSet *)cookie;

	ds->current = 0;

	return;
}

void Seek(void *cookie, int position)
{
	DataSet *ds = (DataSet *)cookie;

	ds->current = position;

	return;
}


int WriteData(void *cookie, int fields, double *values)
{
	DataSet *ds = (DataSet *)cookie;
	int lfields;
	int i;

	/*
	 * EOF is when current == data_c
	 */
	if(ds->current < ds->data_c)
	{
		ds->current++;
	}

	if(fields > ds->fields)
	{
		lfields = ds->fields;
	}
	else
	{
		lfields = fields;
	}

	for(i=0; i < lfields; i++)
	{
		ds->data[i][ds->current] = values[i];
	}

	if(ds->current >= ds->data_c)
	{
		ds->data_c++;
	}
	
	return(1);
}


int ReadData(void *cookie, int fields, double *values)
{
	DataSet *ds = (DataSet *)cookie;
	int lfields;
	int i;

	if(ds->current == -1)
	{
		return(0);
	}

	if(ds->current >= ds->data_c)
	{
		return(0);
	}

	if(fields > ds->fields)
	{
		lfields = ds->fields;
	}
	else
	{
		lfields = fields;
	}

	for(i=0; i < lfields; i++)
	{
		values[i] = ds->data[i][ds->current];
	}

	ds->current++;

	return(1);
}

int MakeCDF_LE(void *data, int fields, double **cdf, int *size)
{
	int count;
	double *lcdf;
	int i;
	int j;
	int k;
	RB *rb;
	RB *rp;
	RB *cp;
	double ts;
	double value;
	double *values;
	double next_value;
	double acc;
	double last_acc;
	Hval hv;

	Rewind(data);

	count = SizeOf(data);

	lcdf = (double *)malloc(count* 2 * sizeof(double));
	if(cdf == NULL)
	{
		exit(1);
	}

	values = (double *)malloc(fields*sizeof(double));
	if(values == NULL) {
		exit(1);
	}

	rb = RBInitD();

	if(rb == NULL)
	{
		exit(1);
	}

	while(ReadData(data,fields,values) != 0)
	{
		if(fields > 1) {
			value = values[1];
		} else {
			value = values[0];
		}
		hv.v = NULL;
		RBInsertD(rb,value,hv);
	}
	

	last_acc = acc = 1.0/(double)count;
	i = 0;
	RB_FORWARD(rb,rp)
	{
		value = K_D(rp->key);
		if(rp->next != NULL) {
			next_value = K_D(rp->next->key);
		} else {
			next_value = -1.0;
		}

		j = 0;
		while(value == next_value) {
			last_acc = acc;
			acc += (1.0 / (double)count);
			rp = rp->next;
			if(rp == NULL) {
				break;
			}
			if(rp->next != NULL) {
				next_value = K_D(rp->next->key);
			}
			j++;
		}
		lcdf[i*2] = value;
		lcdf[i*2+1] = last_acc;
		i++;
		k=0;
		while(k < j) {
			lcdf[i*2] = value;
			lcdf[i*2+1] = last_acc;
			k++;
			i++;
		}

		if(rp == NULL) {
			break;
		}

		acc += (1.0 / (double)count);
		last_acc = acc;
	}

	RBDestroyD(rb);
	free(values);

	*cdf = lcdf;
	*size = count;

	return(1);
}

int CompressCDF(double *big, int bigsize, double **small, int *smallsize)
{
	int i;
	int j;
	int count;
	double *lsmall;

	count = 1;

	for(i=0; i < (bigsize-1); i++)
	{
		if(big[i*2] == big[(i+1)*2])
		{
			continue;
		}
		count++;
	}

	lsmall = (double *)malloc(count*sizeof(double)*2);
	if(lsmall == NULL)
	{
		fprintf(stderr,"CompressCDF no space\n");
		fflush(stderr);
		exit(1);
	}

	memset(lsmall,0,count*sizeof(double)*2);

	j = 0;
	for(i=0; i < count; i++)
	{
		lsmall[i*2] = big[j*2];
		lsmall[i*2+1] = big[j*2+1];
		while(big[j*2] == big[(j+1)*2])
		{
			/*
			 * pick up the larger fraction
			 */
			lsmall[i*2+1] = big[(j+1)*2+1];
			j++;
			if(j == bigsize) {
				break;
			}
		}
		j++;
		if(j == bigsize) {
			break;
		}
	}

	free(big);

	*smallsize = count;
	*small = lsmall;

	return(1);
}


double KS(void *s1, int f1, void *s2, int f2)
{
	double *tcdf;
	int tsize;
	double *cdf1;
	int size1;
	double *cdf2;
	int size2;
	int err;
	int i;
	int j;
	int last_j;
	double max;
	double diff;
	int done;
	double value1;
	double value2;
	

	err = MakeCDF_LE(s1,f1,&tcdf,&tsize);
	if(err <= 0) {
		fprintf(stderr,"KS: couldn't make cdf for s1\n");
		fflush(stderr);
		return(-1.0);
	}

#if 0
	err = CompressCDF(tcdf,tsize,&cdf1,&size1);
	if(err <= 0) {
		fprintf(stderr,"KS: couldn't compress cdf for s1\n");
		fflush(stderr);
		return(-1.0);
	}
#endif
	cdf1 = tcdf;
	size1 = tsize;

	err = MakeCDF_LE(s2,f2,&tcdf,&tsize);
	if(err <= 0) {
		fprintf(stderr,"KS: couldn't make cdf for s2\n");
		fflush(stderr);
		free(cdf1);
		return(-1.0);
	}
#if 0
	err = CompressCDF(tcdf,tsize,&cdf2,&size2);
	if(err <= 0) {
		fprintf(stderr,"KS: couldn't compress cdf for s2\n");
		fflush(stderr);
		return(-1.0);
	}
#endif
	cdf2 = tcdf;
	size2 = tsize;

	value1 = cdf1[i*0+0];
	value2 = cdf2[i*0+0];

	/*
	 * swap them to get the first comparison right
	 */
	if(value2 > value1) {
		tsize = size1;
		tcdf = cdf1;
		size1 = size2;
		cdf1 = cdf2;
		size2 = tsize;
		cdf2 = tcdf;
	}

	max = 0;
	j = 0;
	done = 0;
	for(i=0; i < size1; i++)
	{
		value1 = cdf1[i*2+0];
		value2 = cdf2[j*2+0];

		if(value1 == value2) {
			diff = fabs(cdf1[i*2+1] - cdf2[j*2+1]);
			if(diff > max) {
				max = diff;
			}
			j++;
			if(j == size2) {
				done = 1;
				break;
			}
			continue;
		}
		if(value2 > value1) {
			continue;
		}
		j++;
		if(j == size2) {
			done = 1;
			break;
		}
		value2 = cdf2[j*2+0];
		while(value2 < value1)
		{
			j++;
			if(j == size2) {
				done = 1;
				break;
			}
			value2 = cdf2[j*2+0];
		}
		if(done == 1) {
			break;
		}
		/*
		 * here, we have the place where value2 crosses value1 from
		 * < to >
		 *
		 * compute the diff of the CDF fractions
		 */
//		diff = fabs(cdf1[i*2+1] - cdf2[last_j*2+1]);
		diff = fabs(cdf1[i*2+1] - cdf2[j*2+1]);
		if(diff > max) {
			max = diff;
		}

	}

	if(done == 1) {
		/*
		 * now find the value in cdf1 that is the supremum to
		 * last value in cdf2
		 */
		while((i >= 0) && (cdf1[i*2+0] >= value2)) {
			i--;
		}
		diff = fabs(cdf1[i*2+1] - 1.0);
		if(diff > max) {
			max = diff;
		}
	} else {
		while((j >= 0) && (cdf2[j*2+0] >= value1)) {
			j--;
		}
		diff = fabs(1.0 - cdf2[j*2+1]);
		if(diff > max) {
			max = diff;
		}
	}


	free(cdf1);
	free(cdf2);

	return(max);
		
}

double KSCritical(double alpha, double m, double n)
{
	double critical;

	if((m == 6.0) && (n == 6.0)) {
		return(30.0/36.0);
	}

	critical = sqrt(-0.5 * log(alpha/2.0)) *
                sqrt((n + m) / (n*m));

	return(critical);
}

} // end extern C

int ks_test_node(const struct ts_value* const* operands,
                         const unsigned int operand_count,
			 struct df_operation_metadata *const operation_metadata,
                         const enum ts_types result_type,
                         struct ts_value* result) {
        std::string history_woof = "KS-node." +
		std::to_string(operation_metadata->node_namespace) + "." +
		std::to_string(operation_metadata->node_id) + "." +
		std::to_string(operation_metadata->host_id);

	/*
	 * see if the history woof exists and if it does not, create it
	 * return zero as the KS score
	 */
	double dval;
	int i;
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
	void *old;
	InitDataSet(&old,1);
	for(i=0; i < operand_count; i++) {
		woof_get(history_woof,&dval,seqno);
		WriteData(old,1,&dval);
		seqno--;
	}

	/*
	 * create a dataset with the new values
	 */
	void *newds;
	InitDataSet(&newds,1);
	for(i=0; i < operand_count; i++) {
		dval = operands[i]->value.ts_double;
		WriteData(newds,1,&dval);
	}

	/*
	 * create a KS score
	 */
	double ks = KS(old,1,newds,1);
	FreeDataSet(old);
	FreeDataSet(newds);

	/*
	 * write the new values to the history woof
	 */
	for(i=0; i < operand_count; i++) {
		dval = operands[i]->value.ts_double;
		woof_put(history_woof,"",&dval);
	}

	double crit = KSCritical(0.05,operand_count,operand_count);
printf("KS test: ks: %f, crit: %f\n",ks,crit);

	if(ks < crit) {
		result->value.ts_int = 0;
	} else {
		result->value.ts_int = 1;
	}

	return 1;
}
