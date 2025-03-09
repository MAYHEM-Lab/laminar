#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "../df_interface.h"
#include "type_system/ts_type.h"
#include "type_system/types/ts_array.h"
#include "type_system/types/ts_matrix.h"
#include "type_system/types/ts_primitive.h"
#include "type_system/types/ts_string.h"

int a[8192][8192];
int b[8192][8192];

// int a[4][4] = {
//     {2, 2, 3, 4},
//     {4, 3, 2, 1},
//     {2, 4, 1, 3},
//     {3, 1, 4, 2}
// };

// int b[4][4] = {
//     {2, 2, 3, 4},
//     {4, 3, 2, 1},
//     {2, 4, 1, 3},
//     {3, 1, 4, 2}
// };

bool TIMING = false;

int curr_host_id = 2;

double matmul_partition(const int mat_size, int n_partitions, int t_partitions, unsigned long long itr) {
    auto start_time = std::chrono::high_resolution_clock::now();

    // Write matrix A to operands in `t_partitions` chunks
    int min_chunk_size = mat_size / t_partitions;
    int overflow = mat_size % t_partitions;

    if (curr_host_id == 1) {
        // Write matrix B to operand
        struct ts_value* operand_value = value_from_integer_matrix(b, mat_size, mat_size);
        
        operand op(operand_value, itr);
        write_value(&op.operand_value);
        fire_operand(2, 1, &op);
        value_deep_delete(operand_value);
    }
    else{
        //std::cout << "t_partitions: " << t_partitions << "min_chunk_size: " << min_chunk_size << "mat_size: " << mat_size << std::endl;
        int start = 0, end = 0;
        for (int i = 0; i < t_partitions - 1; i++) {
            end = start + min_chunk_size - 1;
            if (overflow) {
                end++;
                overflow--;
            }

            // Write submatrix to operand
            // std::cout << '[' << start << ", " << end << ']' << std::endl;
            struct ts_value* operand_value = value_from_integer_matrix(a[start], end - start + 1, mat_size);
            
            operand op(operand_value, itr);
            write_value(&op.operand_value);
            fire_operand(1, i + 1, &op);
            value_deep_delete(operand_value);

            start = end + 1;
        }
        
        // Write final submatrix to operand
        // std::cout << '[' << start << ", " << mat_size - 1 << ']' << std::endl;
        struct ts_value* operand_value = value_from_integer_matrix(a[start], mat_size - start, mat_size);
        operand op(operand_value, itr);
        write_value(&op.operand_value);
        fire_operand(1, t_partitions, &op);
        value_deep_delete(operand_value);
    }

    // usleep(5e5);
    sleep(1);

    if (!TIMING) {
        // Show matrix multiplication result
        overflow = mat_size % t_partitions;
        operand result;
        for (unsigned int id = 1; id <= t_partitions; id++) {
            int err = get_result(3, id, &result, itr);
            if (err < 0) {
                std::cout << "Failed to read the result " << std::endl;
            }
            ts_value* const result_value = load_value(&result.operand_value, 3, id);
            int result_matrix[min_chunk_size + !!overflow][mat_size];
                    
            get_integer_matrix(result_matrix, result_value);
            for (int i = 0; i < min_chunk_size + !!overflow; i++) {
                for (int j = 0; j < mat_size; j++) {
                    std::cout << result_matrix[i][j] << " ";
                }
                std::cout << std::endl;
            }

            if (overflow) {
                overflow--;
            }
        }

        return 0.0;
    }

    else {
        operand op;
        std::vector<long> timestamps_ns;
        for (unsigned int id = 1; id <= t_partitions; id++) {
            int err = get_result(3, id, &op, itr);
        
            long ts = op.operand_value.value.ts_long;
            timestamps_ns.push_back(ts);
        }

        long start_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            start_time.time_since_epoch())
                            .count();

        long end_ns = *std::max_element(timestamps_ns.begin(), timestamps_ns.end());

        double latency_ms = (double)(end_ns - start_ns) / 1e6;
        std::cout << "latency: " << latency_ms << "ms" << std::endl;

        return latency_ms;
    }
}

double avg(std::vector<double>& v) {
    double sum = 0.0;
    
    for (double d : v)
        sum += d;
    
    return sum / v.size();
}

#define ARGS "p:d:"
char *Usage = "matmul-distributed -p partitions -d dimention\n";

int main(int argc, char **argv) {
    // Program laminar set up
    system("sudo find . -name \"lmr*\" -delete");
    laminar_reset(); /* reset setup data structures */

    set_host(curr_host_id);
    add_host(1, "169.231.230.190", "/disk2/cspot-namespace-platform/");
    add_host(2, "169.231.230.3", "/disk2/cspot-namespace-platform/");
    laminar_init();
    
    // Matrix set up
	int c;
	int partitions;
	int dim;

	partitions = 0;
	dim = 0;

	while((c = getopt(argc,argv,ARGS)) != EOF) {
		switch(c) {
			case 'p':
				partitions = atoi(optarg);
				break;
			case 'd':
				dim = atoi(optarg);
				break;
			default:
				fprintf(stderr,
					"unrecognized command %c\n",
						(char)c);
				fprintf(stderr,"usage: %s",Usage);
				break;
		}
	}

	if(partitions == 0) {
		fprintf(stderr,"must specify partitions\n");
		fprintf(stderr,"usage: %s",Usage);
		exit(1);
	}
	
	if(dim == 0) {
		fprintf(stderr,"must specify partitions\n");
		fprintf(stderr,"usage: %s",Usage);
		exit(1);
	}
				 
    int t_partitions;

    if(dim < partitions) {
        t_partitions = dim;
    } else {
        t_partitions = partitions;
    }

    // Create operands
    for (int i = 1; i <= t_partitions; i++) {
        add_operand(1, 2, i);  // A / t_partitions
    }

    add_operand(2, 1, 1);  // B
    
    // Create node for each partition output
    for (int i = 1; i <= t_partitions; i++) {
        add_node(
            3, (i % 2 + 1), i,
            {DF_CUSTOM, (TIMING ? MATRIX_MULTIPLY_TIMING : MATRIX_MULTIPLY)});
        subscribe(3, i, 0, 1, i);  // Rows of matrix A (A / t_partitions)
        subscribe(3, i, 1, 2, 1);  // Matrix B
    }

    /* Run program */
    laminar_setup();
    
    std::vector<double> latencies_ms;
    for (unsigned long long itr = 1; itr < 31; itr++) {
        latencies_ms.push_back(matmul_partition(dim, partitions, t_partitions, itr));
    }

    for (double t : latencies_ms)
        std::cout << t << ", ";

    std::cout << "\nAverage: " << avg(latencies_ms) << std::endl;

    return 0;
}
