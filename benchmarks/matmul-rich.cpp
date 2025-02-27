#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../df_interface.h"
#include "type_system/ts_type.h"
#include "type_system/types/ts_array.h"
#include "type_system/types/ts_matrix.h"
#include "type_system/types/ts_primitive.h"
#include "type_system/types/ts_string.h"

void transpose(std::vector<std::vector<int>>& m) {
    int n = m.size();
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int tmp = m[i][j];
            m[i][j] = m[j][i];
            m[j][i] = tmp;
        }
    }
}

void matmul() {
    system("sudo find . -name \"lmr*\" -delete");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int ns = 1;

    // std::vector<std::vector<int>> a = {
    //     {1, 2},
    //     {3, 4}
    // };

    //  std::vector<std::vector<int>> b = {
    //     {5, 6},
    //     {7, 8}
    // };

    int mat_size = 4; // matrices will be mat_size x mat_size

    std::vector<std::vector<int>> a(mat_size, std::vector<int>(mat_size));
    std::vector<std::vector<int>> b(mat_size, std::vector<int>(mat_size));
    int rows_a = a.size();
    int cols_a = a[0].size();
    int rows_b = b.size();
    int cols_b = b[0].size();

    // Result matrix dimensions
    int rows_r = a.size();
    int cols_r = b[0].size();

    // Create operands
    for (int i = 1; i <= rows_a; i++) {
        add_operand(1, 1, i);
    }

    for (int i = 1; i <= cols_b; i++) {
        add_operand(2, 1, i);
    }

    int id;
    std::string id_str;
    for (int r = 0; r < rows_r; r++) {
        for (int c = 0; c < cols_r; c++) {
            // Create node for each output cell
            id = r * (cols_r) + c + 1;
            add_node(3, 1, id, {DF_CUSTOM, VECTOR_DOT_PRODUCT});
            subscribe(3, id, 0, 1, r + 1); // Row of matrix A
            subscribe(3, id, 1, 2, c + 1); // Col of matrix B
        }
    }

    transpose(b);

    /* Run program */
    laminar_setup();

    auto start = std::chrono::high_resolution_clock::now();

    // Write matrices to operands
    for (int i = 0; i < rows_a; i++) {
        struct ts_value* operand_value = value_from_integer_array(&a[i][0], a[i].size());
        operand op(operand_value, 1);
        write_value(&op.operand_value);
        fire_operand(1, i + 1, &op);
        value_deep_delete(operand_value);
    }

    for (int i = 0; i < cols_b; i++) {
        struct ts_value* operand_value = value_from_integer_array(&b[i][0], b[i].size());
        operand op(operand_value, 1);
        write_value(&op.operand_value);
        fire_operand(2, i + 1, &op);
        value_deep_delete(operand_value);
    }

    usleep(5e5);

    // // Show matrix multiplication result
    // operand result;
    // for (int r = 0; r < rows_r; r++) {
    //     for (int c = 0; c < cols_r; c++) {
    //         id = r * cols_r + c + 1;

    //         int err = get_result(3, id, &result, 1);
    //         if (err < 0) {
    //             std::cout << "Failed to read the result " << std::endl;
    //         }
    //         std::cout << result.operand_value.value.ts_int << " ";
    //     }
    //     std::cout << std::endl;
    // }

    operand op;
    std::vector<long> timestamps_ns;
    for (int r = 0; r < rows_r; r++) {
        for (int c = 0; c < cols_r; c++) {
            id = r * cols_r + c + 1;

            int err = get_result(3, id, &op, 1);
           
            long ts = op.operand_value.value.ts_long;
            timestamps_ns.push_back(ts);
        }
    }

    long start_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        start.time_since_epoch())
                        .count();

    long end_ns = *std::max_element(timestamps_ns.begin(), timestamps_ns.end());

    std::cout << "latency: " << (double)(end_ns - start_ns) / 1e6 << "ms"
              << std::endl;
}

void transpose(void* matrix, int n) {
    int* m = (int*)(matrix);
    
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int tmp = m[i * n + j];
            m[i * n + j] = m[j * n + i];
            m[j * n + i] = tmp;
        }
    }
}

int a[8192][8192];
int b[8192][8192];

double matmul_partition(const int mat_size, int n_partitions, bool timing) {
    system("sudo find . -name \"lmr*\" -delete");
    laminar_reset(); /* reset setup data structures */
    set_host(1);
    add_host(1, "127.0.0.1", "/home/centos/cspot/build/bin/");
    laminar_init();

    int t_partitions;

    int ns = 1;

    if(mat_size < n_partitions) {
	t_partitions = mat_size;
    } else {
	t_partitions = n_partitions;
    }

    // int a[4][4] = {
    //     {1, 2, 3, 4},
    //     {4, 3, 2, 1},
    //     {2, 4, 1, 3},
    //     {3, 1, 4, 2}
    // };

    // int b[4][4] = {
    //     {1, 2, 3, 4},
    //     {4, 3, 2, 1},
    //     {2, 4, 1, 3},
    //     {3, 1, 4, 2}
    // };

    // const int mat_size = 4;  // matrices will be mat_size x mat_size
    // int t_partitions = 2;    // number of partitions

//    int a[mat_size][mat_size];
//    int b[mat_size][mat_size];


    // Create operands
    for (int i = 1; i <= t_partitions; i++) {
        add_operand(1, 1, i);  // A / t_partitions
    }

    add_operand(2, 1, 1);  // B
    
    // Create node for each partition output
    for (int i = 1; i <= t_partitions; i++) {
        add_node(
            3, 1, i,
            {DF_CUSTOM, (timing ? MATRIX_MULTIPLY_TIMING : MATRIX_MULTIPLY)});
        subscribe(3, i, 0, 1, i);  // Rows of matrix A (A / t_partitions)
        subscribe(3, i, 1, 2, 1);  // Matrix B
    }

    /* Run program */
    laminar_setup();

    auto start_time = std::chrono::high_resolution_clock::now();

    // Write matrix B to operand
    {
        struct ts_value* operand_value = value_from_integer_matrix(b, mat_size, mat_size);
        
        operand op(operand_value, 1);
        write_value(&op.operand_value);
        fire_operand(2, 1, &op, false);
        value_deep_delete(operand_value);
    }

    // Write matrix A to operands in `t_partitions` chunks
    int min_chunk_size = mat_size / t_partitions;
    int overflow = mat_size % t_partitions;

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
        
        operand op(operand_value, 1);
        write_value(&op.operand_value);
        fire_operand(1, i + 1, &op);
        value_deep_delete(operand_value);

        start = end + 1;
    }

    
    // Write final submatrix to operand
    // std::cout << '[' << start << ", " << mat_size - 1 << ']' << std::endl;
    struct ts_value* operand_value = value_from_integer_matrix(a[start], mat_size - start, mat_size);
    operand op(operand_value, 1);
    write_value(&op.operand_value);
    fire_operand(1, t_partitions, &op);
    value_deep_delete(operand_value);

    // usleep(5e5);
    sleep(1);

    if (!timing) {
        // Show matrix multiplication result
        overflow = mat_size % t_partitions;
        operand result;
        for (unsigned int id = 1; id <= t_partitions; id++) {
            int err = get_result(3, id, &result, 1);
            if (err < 0) {
                std::cout << "Failed to read the result " << std::endl;
            }
            ts_value* const result_value = load_value(&result.operand_value);
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
            int err = get_result(3, id, &op, 1);
        
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
char *Usage = "matmul-rich -p partitions -d dimention\n";

int main(int argc, char **argv) {
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
				
    // matmul();
    
    std::vector<double> latencies_ms;
    for (int i = 0; i < 30; i++) {
        latencies_ms.push_back(matmul_partition(dim, partitions, true));
    }

    for (double t : latencies_ms)
        std::cout << t << ", ";

    std::cout << "\nAverage: " << avg(latencies_ms) << std::endl;

    return 0;
}
