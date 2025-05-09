#include "../df_interface.h"
#include "type_system/types/ts_primitive.h"

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

void online_linreg_multinode() {
    // Update variables (num, x, y, xx, xy)
    const struct df_operation multiplication = {DF_ARITHMETIC, DF_ARITH_MULTIPLICATION};
    const struct df_operation addition = {DF_ARITHMETIC, DF_ARITH_ADDITION};
    const struct df_operation offset = {DF_INTERNAL, DF_INTERNAL_OFFSET};
    const struct df_operation select = {DF_INTERNAL, DF_INTERNAL_SELECT};
    const struct df_operation subtraction = {DF_ARITHMETIC, DF_ARITH_SUBTRACTION};
    const struct df_operation division = {DF_ARITHMETIC, DF_ARITH_DIVISION};
    const struct df_operation greater_than = {DF_LOGIC, DF_LOGIC_GREATER_THAN};

    add_node(1, 1, 1, multiplication);  // num *= decay_rate
    add_node(1, 1, 2, multiplication);  // x *= decay_rate
    add_node(1, 1, 3, multiplication);  // y *= decay_rate
    add_node(1, 1, 4, multiplication);  // xx *= decay_rate
    add_node(1, 1, 5, multiplication);  // xy *= decay_rate
    add_node(1, 1, 6, addition);        // num += 1
    add_node(1, 1, 7, addition);        // x += new_x
    add_node(1, 1, 8, addition);        // y += new_y
    add_node(1, 1, 9, multiplication);  // new_x ^ 2
    add_node(1, 1, 10, addition);       // xx += new_x ^ 2
    add_node(1, 1, 11, multiplication); // new_x * new_y
    add_node(1, 1, 12, addition);       // xy += new_x * new_y
    add_node(1, 1, 13, offset);         // num seq + 1
    add_node(1, 1, 14, offset);         // x seq + 1
    add_node(1, 1, 15, offset);         // y seq + 1
    add_node(1, 1, 16, offset);         // xx seq + 1
    add_node(1, 1, 17, offset);         // xy seq + 1
    add_node(1, 1, 18, select);         // num or 0?
    add_node(1, 1, 19, select);         // x or 0?
    add_node(1, 1, 20, select);         // y or 0?
    add_node(1, 1, 21, select);         // xx or 0?
    add_node(1, 1, 22, select);         // xy or 0?

    // Calculate slope and intercept

    add_node(2, 1, 1, multiplication); // num * xx
    add_node(2, 1, 2, multiplication); // x * x
    add_node(2, 1, 3, subtraction);    // det = num * xx - x * x
    add_node(2, 1, 4, multiplication); // xx * y
    add_node(2, 1, 5, multiplication); // xy * x
    add_node(2, 1, 6, subtraction);    // xx * y - xy * x
    add_node(2, 1, 7, division);       // intercept = (xx * y - xy * x) / det;
    add_node(2, 1, 8, multiplication); // xy * num
    add_node(2, 1, 9, multiplication); // x * y
    add_node(2, 1, 10, subtraction);   // xy * num - x * y
    add_node(2, 1, 11, division);      // slope = (xy * num - x * y) / det;
    add_node(2, 1, 12, greater_than);  // det > 1e-10?
    // add_node(2, 13, SEL);   // intercept or 0?
    // add_node(2, 14, SEL);   // slope or 0?

    // Constants

    add_operand(3, 1, 1); // const 1
    add_operand(3, 1, 2); // const 0
    add_operand(3, 1, 3); // decay_factor = exp(-dt / T)
    add_operand(3, 1, 4); // handle init (0, 1, 1, ..., 1)
    add_operand(3, 1, 5); // const 1e-10

    // Inputs

    add_operand(4, 1, 1); // input x
    add_operand(4, 1, 2); // input y

    // Outputs

    add_node(5, 2, 1, select); // intercept = intercept or 0
    add_node(5, 2, 2, select); // slope = slope or 0

    // Edges

    // Update num, x, y, xx, xy
    subscribe("1:1:0", "3:3");   // decay_rate * _
    subscribe("1:1:1", "1:18");  // decay_rate * num
    subscribe("1:2:0", "3:3");   // decay_rate * _
    subscribe("1:2:1", "1:19");  // decay_rate * x
    subscribe("1:3:0", "3:3");   // decay_rate * _
    subscribe("1:3:1", "1:20");  // decay_rate * y
    subscribe("1:4:0", "3:3");   // decay_rate * _
    subscribe("1:4:1", "1:21");  // decay_rate * xx
    subscribe("1:5:0", "3:3");   // decay_rate * _
    subscribe("1:5:1", "1:22");  // decay_rate * xy
    subscribe("1:6:0", "3:1");   // _ + 1
    subscribe("1:6:1", "1:1");   // num += 1
    subscribe("1:7:0", "4:1");   // _ + new_x
    subscribe("1:7:1", "1:2");   // x += new_x
    subscribe("1:8:0", "4:2");   // _ + new_y
    subscribe("1:8:1", "1:3");   // y += new_y
    subscribe("1:9:0", "4:1");   // new_x * _
    subscribe("1:9:1", "4:1");   // new_x * new_x
    subscribe("1:10:0", "1:9");  // _ + new_x * new_x
    subscribe("1:10:1", "1:4");  // xx += new_x * new_x
    subscribe("1:11:0", "4:1");  // x * _
    subscribe("1:11:1", "4:2");  // x * y
    subscribe("1:12:0", "1:11"); // _ + x * y
    subscribe("1:12:1", "1:5");  // xy += x * y

    // Feedback offset
    subscribe("1:13:0", "3:1");  // offset = 1
    subscribe("1:13:1", "1:6");  // num seq + 1
    subscribe("1:14:0", "3:1");  // offset = 1
    subscribe("1:14:1", "1:7");  // x seq + 1
    subscribe("1:15:0", "3:1");  // offset = 1
    subscribe("1:15:1", "1:8");  // y seq + 1
    subscribe("1:16:0", "3:1");  // offset = 1
    subscribe("1:16:1", "1:10"); // xx seq + 1
    subscribe("1:17:0", "3:1");  // offset = 1
    subscribe("1:17:1", "1:12"); // xy seq + 1
    subscribe("1:18:0", "3:4");  // SEL: 0, 1, ..., 1
    subscribe("1:18:1", "3:2");  // 0 or _?
    subscribe("1:18:2", "1:13"); // 0 or num?
    subscribe("1:19:0", "3:4");  // SEL: 0, 1, ..., 1
    subscribe("1:19:1", "3:2");  // 0 or _?
    subscribe("1:19:2", "1:14"); // 0 or x?
    subscribe("1:20:0", "3:4");  // SEL: 0, 1, ..., 1
    subscribe("1:20:1", "3:2");  // 0 or _?
    subscribe("1:20:2", "1:15"); // 0 or y?
    subscribe("1:21:0", "3:4");  // SEL: 0, 1, ..., 1
    subscribe("1:21:1", "3:2");  // 0 or _?
    subscribe("1:21:2", "1:16"); // 0 or xx?
    subscribe("1:22:0", "3:4");  // SEL: 0, 1, ..., 1
    subscribe("1:22:1", "3:2");  // 0 or _?
    subscribe("1:22:2", "1:17"); // 0 or xy?

    // Determinant
    subscribe("2:1:0", "1:6");  // num * _
    subscribe("2:1:1", "1:10"); // num * xx
    subscribe("2:2:0", "1:7");  // x * _
    subscribe("2:2:1", "1:7");  // x * x
    subscribe("2:3:0", "2:1");  // num * xx - ____
    subscribe("2:3:1", "2:2");  // det = num * xx - x * x
    subscribe("2:12:0", "2:3"); // det > ____?
    subscribe("2:12:1", "3:5"); // det > 1e-10?

    // Intercept
    subscribe("2:4:0", "1:10"); // xx * _
    subscribe("2:4:1", "1:8");  // xx * y
    subscribe("2:5:0", "1:12"); // xy * _
    subscribe("2:5:1", "1:7");  // xy * x
    subscribe("2:6:0", "2:4");  // xx * y - ____
    subscribe("2:6:1", "2:5");  // xx * y - xy * x
    subscribe("2:7:0", "2:6");  // (xx * y - xy * x) / ____
    subscribe("2:7:1", "2:3");  // intercept = (xx * y - xy * x) / det
    subscribe("5:1:0", "2:12"); // result SEL: det > 1e-10?
    subscribe("5:1:1", "3:2");  // result = 0 or ____?
    subscribe("5:1:2", "2:7");  // result = 0 or intercept?

    // Slope
    subscribe("2:8:0", "1:12");  // xy * _
    subscribe("2:8:1", "1:6");   // xy * num
    subscribe("2:9:0", "1:7");   // x * _
    subscribe("2:9:1", "1:8");   // x * y
    subscribe("2:10:0", "2:8");  // xy * num - ____
    subscribe("2:10:1", "2:9");  // xy * num - x * y
    subscribe("2:11:0", "2:10"); // (xy * num - x * y) / ____
    subscribe("2:11:1", "2:3");  // slope = (xy * num - x * y) / det
    subscribe("5:2:0", "2:12");  // result SEL: det > 1e-10?
    subscribe("5:2:1", "3:2");   // result = 0 or ____?
    subscribe("5:2:2", "2:11");  // result = 0 or slope?

    laminar_setup();
    // std::cout << graphviz_representation() << std::endl;
    // return;

    // Initialization
    int curr_host_id = get_curr_host_id();
    int iters = 100;

    if (curr_host_id == 1) {
        std::cout << "Initializing constants" << std::endl;

        // Const (3:1) = 1
        for (int i = 1; i <= iters; i++) {
            struct ts_value* double_value = value_from_double(1.0);
            operand op(double_value, i);
            fire_operand(3, 1, &op);
            value_deep_delete(double_value);
        }

        // Const (3:2) = 0
        for (int i = 1; i <= iters; i++) {
            struct ts_value* double_value = value_from_double(0.0);
            operand op(double_value, i);
            fire_operand(3, 2, &op);
            value_deep_delete(double_value);
        }

        // Const (3:3) = exp(-dt/T)  [decay_rate]
        double dt = 1e-2;
        double T = 5e-2;
        double decay_rate = exp(-dt / T);
        for (int i = 1; i <= iters; i++) {
            struct ts_value* double_value = value_from_double(decay_rate);
            operand op(double_value, i);
            fire_operand(3, 3, &op);
            value_deep_delete(double_value);
        }

        // Const (3:4) = 0, 1, 1, ..., 1
        for (int i = 1; i <= iters; i++) {
            int val = (i == 1 ? 0 : 1);
            struct ts_value* double_value = value_from_double(val);
            operand op(double_value, i);
            fire_operand(3, 4, &op);
            value_deep_delete(double_value);
        }

        // Const (3:5) = 1e-10
        for (int i = 1; i <= iters; i++) {
            struct ts_value* double_value = value_from_double(1e-10);
            operand op(double_value, i);
            fire_operand(3, 5, &op);
            value_deep_delete(double_value);
        }

        // Seed offset nodes with initial value
        for (int i = 13; i <= 17; i++) {
            struct ts_value* double_value = value_from_double(0.0);
            operand op(double_value, 1);
            fire_operand(1, i, &op);
            value_deep_delete(double_value);
        }

        // Run program

        std::random_device rd;
        std::default_random_engine eng(rd());
        std::uniform_real_distribution<double> distr(-0.1, 0.1);

        std::vector<operand> x_values;
        std::vector<operand> y_values;

        std::cout << "Writing x and y values" << std::endl;

        std::vector<ts_value*> pointers_to_free;
        for (int i = 0; i < iters; i++) {
            double x = i + distr(eng);
            struct ts_value* x_value = value_from_double(x);
            double y = 3 + 2 * i + distr(eng);
            struct ts_value* y_value = value_from_double(y);
            x_values.push_back(operand(x_value, i + 1));
            y_values.push_back(operand(y_value, i + 1));
            pointers_to_free.push_back(x_value);
            pointers_to_free.push_back(y_value);
        }
        /*
        std::chrono::time_point<std::chrono::system_clock> start, end;
        std::vector<host> hosts;
        hosts.push_back(host(2, "woof://169.231.235.188/home/centos/cspot-apps/build/bin/"));
        hosts.push_back(host(2, "woof://169.231.235.168/home/centos/cspot-apps/build/bin/"));
        */
        for (int i = 1; i <= iters; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "Adding inputs " << std::to_string(i) << std::endl;
            fire_operand(4, 1, &x_values[i - 1]);
            fire_operand(4, 2, &y_values[i - 1]);
            /*
            if (i == 5 || i == 20 || i == 50 || i == 90) {
                woof_put(generate_woof_path(HOSTS_WF_TYPE), "", &hosts[0]);
                start = std::chrono::system_clock::now();
            }

            if (i == 7 || i == 23 || i == 54 || i == 95) {
                woof_put(generate_woof_path(HOSTS_WF_TYPE), "", &hosts[1]);
                end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end - start;

                std::cout << "Network partition time : " << elapsed_seconds.count() << std::endl;
            }
            */
        }
        for (const auto& item : pointers_to_free) {
            value_deep_delete(item);
        }
    }

    if (curr_host_id == 2) {
        std::vector<double> intercepts;
        std::vector<double> slopes;

        for (int i = 1; i <= iters; i++) {

            operand op1;
            get_result(5, 1, &op1, i);
            intercepts.push_back(op1.operand_value.value.ts_double);
            std::cout << std::to_string(i) << " Intercept " << std::to_string(op1.operand_value.value.ts_double) << std::endl;

            operand op2;
            get_result(5, 2, &op2, i);
            slopes.push_back(op2.operand_value.value.ts_double);
            std::cout << std::to_string(i) << " Slope " << std::to_string(op2.operand_value.value.ts_double) << std::endl;
        }
    }

    std::cout << "Execution Completed" << std::endl;
}

int main() {
    laminar_init();
    set_host(1);
    add_host(1, "169.231.235.212", "/home/centos/cspot-apps/build/bin/");
    add_host(2, "169.231.235.168", "/home/centos/cspot-apps/build/bin/");

    online_linreg_multinode();
}
