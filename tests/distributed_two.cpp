// passing a double from one host to another
// testing simple distributed capabilities
// Written by Krish & Gen 

#include "../df_interface.h"
#include "type_system/types/ts_primitive.h"

#include <iostream>
#include <string>
#include <unistd.h>

int main() {
    int ns = 1; // Laminar Namespace (not CSPOT's)
    system("sudo find . -name \"lmr*\" -delete");
    laminar_reset(); /* reset setup data structures */
    laminar_init();

    // Set up two devices (change IPs and/or cspot namespaces)
    int curr_host_id = 2;
    set_host(curr_host_id);
    add_host(1, "169.231.230.190", "/disk2/cspot-namespace-platform/");
    add_host(2, "169.231.230.247", "/disk2/cspot-namespace-platform/");

    // Nodes
    add_node(ns, 1, 3, {.category = DF_ARITHMETIC, .operation = DF_ARITH_MULTIPLICATION});       // a * b

    // Inputs
    add_operand(ns, 1, 1); // a
    add_operand(ns, 2, 2); // b
    
    // Edges
    subscribe("1:3:0", "1:1"); // ADD[0] <-- a
    subscribe("1:3:1", "1:2"); // ADD[1] <-- b

    laminar_setup();
    if(curr_host_id == 1) {
        // Example: (1 + 2) * 3
        struct ts_value value_a{};
        set_double(&value_a, 1);
        operand op_a(&value_a, 1);
        fire_operand(ns, 1, &op_a);
    }
    else {
        struct ts_value value_b{};
        set_double(&value_b, 2);
        operand op_b(&value_b, 1);
        fire_operand(ns, 2, &op_b);
    }
    operand result;
    int err = get_result(ns, 3, &result, 1);
    if (err < 0) {
        std::cout << "Failed to read the result " << std::endl;
    }
    
    std::cout << "Result: " << result.operand_value.value.ts_double << std::endl;
}
