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
    add_host(2, "169.231.230.3", "/disk2/cspot-namespace-platform/");
    laminar_init();

    const struct df_operation parse = {DF_INTERNAL, DF_INTERNAL_NOOP};
    add_node(ns, 1, 1, parse);
    add_operand(ns, 1, 2);

    subscribe(ns, 1, 0, ns, 2);
    laminar_setup();

    for (unsigned long long itr = 1; itr < 30; itr++){
        std::cout << "itr: " << itr << std::endl;
        if (curr_host_id == 1){
            uint8_t array[] = {1, 2, 3};
            struct ts_value* operand_value = value_from_unsigned_byte_array(array, 3);
            operand op(operand_value, itr);
            write_value(&op.operand_value);
            fire_operand(ns, 2, &op);
            value_deep_delete(operand_value);

            sleep(1);

            std::cout << "gets: " << (int)woof_last_seq(generate_woof_path(OUT_WF_TYPE, ns, 1)) << std::endl;
            operand result;
            get_result(ns, 1, &result, itr);
            ts_value* loaded_result = load_value(&result.operand_value);

            uint8_t result_array[3];
            get_unsigned_byte_array(result_array, loaded_result);

            for(int i = 0; i < sizeof(result_array); i++){
                std::cout << (int)result_array[i] << std::endl;
            }
        }
        else{
            std::cout << "gets: " << (int)woof_last_seq(generate_woof_path(OUT_WF_TYPE, ns, 1)) << std::endl;
            operand result;
            get_result(ns, 1, &result, itr);
            ts_value* loaded_result = load_value(&result.operand_value, ns, 1);

            uint8_t result_array[3];
            get_unsigned_byte_array(result_array, loaded_result);

            for(int i = 0; i < sizeof(result_array); i++){
                std::cout << (int)result_array[i] << std::endl;
            }
        }
    }

    return 0;
}