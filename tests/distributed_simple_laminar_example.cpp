#include "../df_interface.h"
#include "type_system/types/ts_primitive.h"

#include <iostream>
#include <string>
#include <unistd.h>

// SAMPLE APPLICATION
// (a + b) * c
// MUL(ADD(a, b), c)

//  Host 1    ----> HOST 2
//  ADD(a,b) 
//            --->  MUL()
//  c

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

    add_node(ns, 1, 1, {.category = DF_ARITHMETIC, .operation = DF_ARITH_ADDITION});       // a + b
    add_node(ns, 2, 2, {.category = DF_ARITHMETIC, .operation = DF_ARITH_MULTIPLICATION}); // (a + b) * c

    // Inputs
    add_operand(ns, 1, 3); // a
    add_operand(ns, 1, 4); // b
    add_operand(ns, 1, 5); // c

    int id_a = 3, id_b = 4, id_c = 5;

    // Edges
    subscribe("1:1:0", "1:3"); // ADD[0] <-- a
    subscribe("1:1:1", "1:4"); // ADD[1] <-- b
    subscribe("1:2:0", "1:1"); // MUL[0] <-- (a + b)
    subscribe("1:2:1", "1:5"); // MUL[1] <-- c

    // Optional: Autogenerate diagram
    // std::cout << graphviz_representation() << std::endl;
    // exit(0);

    laminar_setup();
    if(curr_host_id == 1) {
        // Example: (1 + 2) * 3
        struct ts_value value_a{};
        set_double(&value_a, 11);
        operand op_a(&value_a, 1);
        fire_operand(ns, id_a, &op_a);

        struct ts_value value_b{};
        set_double(&value_b, 2);
        operand op_b(&value_b, 1);
        fire_operand(ns, id_b, &op_b);

        struct ts_value value_c{};
        set_double(&value_c, 3);
        operand op_c(&value_c, 1);
        fire_operand(ns, id_c, &op_c);
        std::cout << "Completed input " << std::endl;
    }
    else {
        operand result;
        int err = get_result(ns, 2, &result, 1);
        if (err < 0) {
            std::cout << "Failed to read the result " << std::endl;
        }
        
        std::cout << "Result: " << result.operand_value.value.ts_double << std::endl;
    }
}
