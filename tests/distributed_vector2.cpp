// passing a vector of unsigned bytes between two hosts
// with 2 nodes so we can achieve some synchronization
// Written by Krish & Gen 

#include "../df_interface.h"
#include "type_system/ts_type.h"
#include "type_system/types/ts_array.h"
#include "type_system/types/ts_matrix.h"
#include "type_system/types/ts_primitive.h"
#include "type_system/types/ts_string.h"

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

int main() {
    int ns = 1; // Namespace
    system("sudo find . -name \"lmr*\" -delete");
    laminar_reset(); /* reset setup data structures */

    int curr_host_id = 2;
    set_host(curr_host_id);
    add_host(1, "169.231.230.190", "/disk2/cspot-namespace-platform/");
    add_host(2, "169.231.230.247", "/disk2/cspot-namespace-platform/");
    laminar_init();

    const struct df_operation parse = {DF_INTERNAL, DF_INTERNAL_NOOP};
    add_operand(ns, 1, 1);
    add_node(ns, 1, 2, parse);
    add_node(ns, 2, 3, parse);

    subscribe(ns, 2, 0, ns, 1);
    subscribe(ns, 3, 0, ns, 2);
    laminar_setup();

    for (unsigned long long itr = 1; itr < 30; itr++){
        std::cout << "itr: " << itr << std::endl;
        if (curr_host_id == 1){
            uint8_t array[] = {1, 2, 3};
            struct ts_value* operand_value = value_from_unsigned_byte_array(array, 3);
            operand op(operand_value, itr);
            write_value(&op.operand_value);
            fire_operand(ns, 1, &op);
            value_deep_delete(operand_value);

            operand result;
            get_result(ns, 3, &result, itr);
            ts_value* loaded_result = load_value(&result.operand_value, ns, 3);

            uint8_t result_array[3];
            get_unsigned_byte_array(result_array, loaded_result);

            for(int i = 0; i < sizeof(result_array); i++){
                std::cout << (int)result_array[i] << std::endl;
            }
        }
        else{
            operand result;
            get_result(ns, 2, &result, itr);
            ts_value* loaded_result = load_value(&result.operand_value, ns, 2);

            uint8_t result_array[3];
            get_unsigned_byte_array(result_array, loaded_result);

            for(int i = 0; i < sizeof(result_array); i++){
                std::cout << (int)result_array[i] << std::endl;
            }
        }
    }

    return 0;
}