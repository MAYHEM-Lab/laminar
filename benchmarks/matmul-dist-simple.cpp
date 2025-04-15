// Test file testing distributed array capabilities - multiplies a matrix by itself and the result of previous iteration - distributed across two hosts. 
// Written by Krish & Gen 
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


const size_t MAT_SIZE = 2;

int a[MAT_SIZE][MAT_SIZE] = {
    {1, 1},
    {1, 1}
};
const int curr_host_id = 2;

const int ns = 1;

void fire_matrix(int matrix[MAT_SIZE][MAT_SIZE], int id, unsigned long long itr);

int main(int argc, char **argv) {
    // Program laminar set up
    system("sudo find . -name \"lmr*\" -delete");
    laminar_reset(); /* reset setup data structures */
    laminar_init();
    
    set_host(curr_host_id);
    add_host(1, "169.231.230.190", "/disk2/cspot-namespace-platform/");
    add_host(2, "169.231.230.247", "/disk2/cspot-namespace-platform/");

    add_node(ns, 1, 1, {DF_CUSTOM, MATRIX_MULTIPLY});

    // Inputs
    add_operand(ns, 1, 2); // a
    add_operand(ns, 2, 3); // b

    subscribe(ns, 1, 0, ns, 2);
    subscribe(ns, 1, 1, ns, 3);

    /* Run program */
    laminar_setup();
    if (curr_host_id == 1) {
        fire_matrix(a, 2, 1);
    }
    else{
        fire_matrix(a, 3, 1);
    }
    for (unsigned long long itr = 1; itr < 31; itr++) {
        operand result;
        int err = get_result(ns, 1, &result, itr);
        if (err < 0) {
            std::cout << "Failed to read the result " << std::endl;
        }
        ts_value* const result_value = load_value(&result.operand_value, ns, 1);
        int result_matrix[MAT_SIZE][MAT_SIZE];
                
        get_integer_matrix(result_matrix, result_value);
        for (int i = 0; i < MAT_SIZE; i++) {
            for (int j = 0; j < MAT_SIZE; j++) {
                std::cout << result_matrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
        if (curr_host_id == 1) {
            fire_matrix(a, 2, itr+1);
        }
        else{
            fire_matrix(result_matrix, 3, itr+1);
        }
    }
}

void fire_matrix(int matrix[MAT_SIZE][MAT_SIZE], int id, unsigned long long itr){
    struct ts_value* operand_value = value_from_integer_matrix(matrix, MAT_SIZE, MAT_SIZE);
    operand op(operand_value, itr);
    write_value(&op.operand_value);
    fire_operand(ns, id, &op);
    value_deep_delete(operand_value);
}