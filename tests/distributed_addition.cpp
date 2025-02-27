#include "../df_interface.h"
#include "type_system/types/ts_primitive.h"

#include <iostream>
#include <string>
#include <unistd.h>

int main() {
    int ns = 1; // Namespace
    system("sudo find . -name \"lmr*\" -delete");
    laminar_reset(); /* reset setup data structures */
    laminar_init();

    // Set up single device on localhost
    
    int curr_host_id = 2;
    set_host(curr_host_id);
    add_host(1, "169.231.230.190", "/disk2/cspot-namespace-platform/");
    add_host(2, "169.231.230.3", "/disk2/cspot-namespace-platform/");
    
    // Nodes is on device 1: (namespace, host, node)

    add_node(ns, 1, 1, {.category = DF_ARITHMETIC, .operation = DF_ARITH_ADDITION});       // a + b

    // Inputs is shared on both devices

    add_operand(ns, 1, 2); // a
    add_operand(ns, 2, 3); // b

    // Edges: "namespace:dest_node:operand_num", "namespace:source_node"
    subscribe("1:1:0", "1:2"); // ADD[0] <-- a
    subscribe("1:1:1", "1:3"); // ADD[1] <-- b

    // Optional: Autogenerate diagram
    std::cout << graphviz_representation() << std::endl;
    // exit(0);

    laminar_setup();
    if(curr_host_id == 1) {
        struct ts_value value_a{};
        set_double(&value_a, 1);
        operand op_a(&value_a);
        fire_operand(ns, 2, &op_a);
        std::cout << "Completed input " << std::endl;
    }
    else {
        struct ts_value value_b{};
        set_double(&value_b, 2);
        operand op_b(&value_b);
        fire_operand(ns, 3, &op_b);
        std::cout << "Completed input " << std::endl;
    }

    operand result;
    int err = get_result(ns, 1, &result, 1);
    if (err < 0) {
        std::cout << "Failed to read the result " << std::endl;
    }
    std::cout << "Result: " << result.operand_value.value.ts_double << std::endl;

    return 0;
}