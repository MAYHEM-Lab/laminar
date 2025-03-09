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
    add_host(2, "169.231.230.3", "/disk2/cspot-namespace-platform/");

    add_node(ns, 1, 1, {DF_CUSTOM, MATRIX_MULTIPLY});
    add_node(ns, 2, 2, {DF_CUSTOM, MATRIX_MULTIPLY});

    // Inputs
    add_operand(ns, 1, 3); // a
    add_operand(ns, 1, 4); // b
    add_operand(ns, 1, 5); // c

    subscribe(ns, 1, 0, ns, 3);
    subscribe(ns, 1, 1, ns, 4);
    subscribe(ns, 2, 0, ns, 1);
    subscribe(ns, 2, 1, ns, 5);

    /* Run program */
    laminar_setup();
    if(curr_host_id == 1){
        fire_matrix(a, 3, 1);
        fire_matrix(a, 4, 1);
        fire_matrix(a, 5, 1);
    }
    // get result
    operand result;
    int err = get_result(ns, 2, &result, 1);
    if (err < 0) {
        std::cout << "Failed to read the result " << std::endl;
    }
    ts_value* const result_value = load_value(&result.operand_value, ns, 2);
    // print it out
    int result_matrix[MAT_SIZE][MAT_SIZE];
    get_integer_matrix(result_matrix, result_value);
    for (size_t i = 0; i < MAT_SIZE; i++) {
        for (size_t j = 0; j < MAT_SIZE; j++) {
            std::cout << result_matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}

void fire_matrix(int matrix[MAT_SIZE][MAT_SIZE], int id, unsigned long long itr){
    struct ts_value* operand_value = value_from_integer_matrix(matrix, MAT_SIZE, MAT_SIZE);
    operand op(operand_value, itr);
    write_value(&op.operand_value);
    fire_operand(ns, id, &op);
    value_deep_delete(operand_value);
}