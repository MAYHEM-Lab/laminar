/**
 * Mapreduce on Wikipedia English dataset (~20 GB)
 * 
 * Dataset sources:
 * https://huggingface.co/datasets/wikipedia
 * https://www.tensorflow.org/datasets/catalog/wikipedia (used)
 * 
 * Dataset preparation:
 * Done separately using Python.
 * The tensorflow wikipedia dataset has ~6.3M 'features'. Use <dataset>.take(7000000)
 * Iterate over each feature and write the 'title', and 'text' to a single text file. (ignore lines which are only '\n')
 * This converts the tensorflow dataset into entirely human-readable text.
 * The process results in 171693782 lines of english text with ~3.27B words.
 * The data is divided equally (as much as possible) into 25200 files, with newlines replaced with spaces.
 * Mappers in the range from 1 - 100 in steps of 10 can divide data equally, reading only required number of files.
 * Instructions to access the prepared dataset are in laminar/docs/benchmarks.md
*/

#include <unistd.h>

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "../df_interface.h"
#include "type_system/types/ts_primitive.h"
#include "type_system/ts_types.h"

// defaults
#define NUM_REGEX_MAPPERS 8
#define NUM_DATA_FILES 400
#define NUM_INV_MAPPERS 1

#define ARGS "r:i:"
char *Usage = "grep_mapreduce <pattern> -r num_regex_mappers -i num_inverse_mappers\n";

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Match pattern not provided." << std::endl;
        fprintf(stderr,"usage: %s",Usage);
        return 1;
    }

    int opt, num_regex_mappers = 0, num_inv_mappers = 0;
    char* pattern;

    // Loop through the arguments
    while ((opt = getopt(argc, argv, ARGS)) != -1) {
        switch (opt) {
        case 'r':
            num_regex_mappers = std::atoi(optarg); // Convert option argument to int
            break;
        case 'i':
            num_inv_mappers = std::atoi(optarg); // Convert option argument to int
            break;
        case '?': // In case of unknown option
            fprintf(stderr,
                "unrecognized command %c\n",
                    (char)opt);
			fprintf(stderr,"usage: %s",Usage);
            return 1;
        }
    }

    if (optind < argc) {
        pattern = argv[optind];
    }

    if (num_regex_mappers == 0)
        num_regex_mappers = NUM_REGEX_MAPPERS;
    if (num_inv_mappers == 0)
        num_inv_mappers = NUM_INV_MAPPERS;

    // Make sure the data folder is on a subpath of the woof path of the host
    std::string folder = "/home/centos/laminar/data_local/wikipedia";

    laminar_init();

    set_host(1);
    int curr_host = 1;

    add_host(1, "localhost", "/home/centos/laminar/");
    // add_host(2, "172.31.20.145", "/home/centos/laminar/");
    int job_1_host = 1;
    int job_2_host = 1;

    /**
     * Job 2 host starts AND ends the execution. It fires the operand for the match pattern
     * and also runs the inverse mapper and reducer.
     * 
     * Job 1 host does the heavy lifting of mapping the text files.
    */

    /////////////////////
    // MAPREDUCE JOB 1 //
    /////////////////////

    // namespace 1 : operands for regex mappers
    add_operand(1, job_2_host, 1); // match pattern : string_prim
    add_operand(1, job_1_host, 2); // data path : string_prim
    add_operand(1, job_1_host, 3); // # of mappers : int
    add_operand(1, job_1_host, 4); // # of data files : int

    for (int i = 1; i <= num_regex_mappers; ++i) {
        // namespace 2 : regex mappers
        add_node(2, job_1_host, i, {.category = DF_CUSTOM, .operation = GREP_REGEX_MAPPER_PRIM});

        // regex mapper subscriptions
        subscribe(2, i, 0, 1, 1);   // MAPPER[0] <--- MATCH_PATTERN
        subscribe(2, i, 1, 1, 2);   // MAPPER[1] <--- DATA_PATH
        subscribe(2, i, 2, 1, 3);   // MAPPER[2] <--- NUM_MAPPERS
        subscribe(2, i, 3, 1, 4);   // MAPPER[3] <--- NUM_DATA_FILES
    }

    // namespace 3 : count reducer
    add_node(3, job_1_host, 1, {.category = DF_CUSTOM, .operation = GREP_WORD_COUNT_REDUCER_PRIM});

    // count reducer subscriptions
    for (int i = 1; i <= num_regex_mappers; ++i) {
        subscribe(3, 1, i-1, 2, i); // REDUCER[i-1] <--- MAPPER
    }

    /////////////////////
    // MAPREDUCE JOB 2 //
    /////////////////////

    for (int i = 1; i <= num_inv_mappers; ++i) {
        // namespace 4 : operands for inverse mappers
        add_operand(4, job_2_host, i);

        // namespace 5 : inverse mappers
        add_node(5, job_2_host, i, {.category = DF_CUSTOM, .operation = GREP_INVERSE_MAPPER_PRIM});
        // inverse mapper subscriptions
        subscribe(5, i, 0, 4, i);   // MAPPER[0] <--- <word, count> list
    }

    // namespace 6 : frequency reducer
    add_node(6, job_2_host, 1, {.category = DF_CUSTOM, .operation = GREP_FREQUENCY_REDUCER_PRIM});

    // frequency reducer subscriptions
    for (int i = 1; i <= num_inv_mappers; ++i) {
        subscribe(6, 1, i-1, 5, i); // REDUCER[i-1] <--- MAPPER
    }

    ////////////////
    // CONTROLLER //
    ////////////////

    laminar_setup();

    // The if-statements are divided such that a single host also works.
    // For single host execution, make sure job_1_host and job_2_host are both 1 and curr_host is also 1.

    if (curr_host == job_2_host) {
        struct ts_value value_match_pattern{};
        set_prim_string(&value_match_pattern, pattern);
        operand op_match_pattern(&value_match_pattern, 1);

        // start execution
        fire_operand(1, 1, &op_match_pattern);
    }
    const auto now = std::chrono::high_resolution_clock::now();
    auto start_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                now.time_since_epoch()).count();

    if (curr_host == job_1_host) {
        struct ts_value value_data_path{};
        set_prim_string(&value_data_path, folder.c_str());
        operand op_data_path(&value_data_path, 1);

        struct ts_value* value_regex_mappers = value_from_integer(num_regex_mappers);
        operand op_num_regex_mappers(value_regex_mappers, 1);

        struct ts_value* value_num_files = value_from_integer(NUM_DATA_FILES);
        operand op_num_files(value_num_files, 1);

        fire_operand(1, 2, &op_data_path);
        fire_operand(1, 3, &op_num_regex_mappers);
        fire_operand(1, 4, &op_num_files);

        //sleep(30);

        // JOB 1 RESULT
        operand job_1_result;
        int err = get_result(3, 1, &job_1_result, 1);
        if (err < 0) {
            std::cout << "Failed to read the result" << std::endl;
            return 0;
        }

        // Divide the results from job 1 among the job 2 mappers
        std::vector<std::string> inv_mapper_inputs(num_inv_mappers);
        char* token = std::strtok(job_1_result.operand_value.value.ts_prim_large_string, " ");
        int kv_pair_num = 0;
        while (token) {
            inv_mapper_inputs[kv_pair_num % num_inv_mappers].append(token).append(" ");
            token = std::strtok(nullptr, " ");
            inv_mapper_inputs[kv_pair_num % num_inv_mappers].append(token).append(" ");
            token = std::strtok(nullptr, " ");
            ++kv_pair_num;
        }

        struct ts_value value_input{};
        for (int i = 1; i <= num_inv_mappers; ++i) {
            set_prim_large_string(&value_input, inv_mapper_inputs[i-1].c_str());
            operand op_mapper_input(&value_input, 1);
            fire_operand(4, i, &op_mapper_input);
        }
    }

    if (curr_host == job_2_host) {
        operand job_2_result;
        int err = get_result(6, 1, &job_2_result, 1);
        auto end_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        double latency_s = (double)(end_time - start_time) / 1e9;
        if (err < 0) {
            std::cout << "Failed to read the result" << std::endl;
            return 0;
        }

        // std::cout << job_2_result.operand_value.value.ts_prim_large_string << std::endl;
        std::cout << "Latency with " << num_regex_mappers << " regex mappers and " << num_inv_mappers << " inverse mappers: " << latency_s << " s" << std::endl;
    }

    return 0;
}
