#include "df.h"
#include "df_interface.h"
#ifndef ESP8266
#include "logger_system/df_logger.h"
#include "operation_system/df_operation.h"
#include "type_system/ts_type.h"
#else
#include "df_logger.h"
#include "df_operation.h"
#include "ts_type.h"
#endif

#include <iostream>
#include <string>
#include <vector>

#ifdef DF_TIMING
#include <chrono>
#endif // DF_TIMING

operand perform_operation(const std::vector<operand>& operands,
                          const struct df_operation operation,
                          struct df_operation_metadata* const operation_metadata,
                          const std::vector<subscription>& input_hosts) {
    const unsigned long operand_count = operands.size();
    struct ts_value* operands_array[operand_count];
    const struct ts_value* const_operands_array[operand_count];
    for (size_t i = 0; i < operand_count; ++i) {
        operands_array[i] = load_value(&operands[i].operand_value, input_hosts[i].ns, input_hosts[i].id);
        const_operands_array[i] = operands_array[i];
        if (const_operands_array[i] == nullptr) {
            log_error("Could not load value [input:%lu]", i);
#ifndef ESP8266
            throw std::runtime_error("loading of input failed");
#endif
        }
        log_debug_with_value("[input:%lu] Loaded value:", const_operands_array[i], i);
    }
    log_debug("Performing operation:[category:%d][operation:%u]", operation.category, operation.operation);
    struct ts_value* result_value;
    bool operation_result = df_operation_with_type(
        operation, const_operands_array, operand_count, operation_metadata, operands_array[0]->type, &result_value);
    if (!operation_result) {
        log_error("Unknown operation handler occurred!");
    }
    log_info_with_value("Operation result:", result_value);

    for (size_t i = 0; i < operand_count; ++i) {
        value_deep_delete(operands_array[i]);
    }

    operand result(result_value, operation_metadata->flow_control.execution_iteration);
    value_deep_delete(result_value);
    
#ifdef DF_TIMING
    const auto now = std::chrono::high_resolution_clock::now();
    result.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          now.time_since_epoch()).count();
#endif // DF_TIMING

    return result;
}

#ifdef CSPOTDEVICE
extern "C" int subscription_event_handler(char* wf, unsigned long seqno, void* ptr) { // NOLINT(misc-no-recursion)
#else
extern "C" int subscription_event_handler(WOOF* wf, unsigned long seqno, void* ptr) { // NOLINT(misc-no-recursion)
#endif
    log_init("DF-S", true);
    log_debug("START: Subscription handler");
//printf("subscription handler started\n");

    int err;
    int max_retries = 10;

    // Get name of this woof
#ifdef CSPOTDEVICE
    std::string woof_name(wf);
#else
    std::string woof_name(WoofGetFileName(wf));
#endif
    // Extract namespace
    const int node_namespace = get_ns_from_woof_path(woof_name);
    // Extract id
    const unsigned long node_id = get_node_id_from_woof_path(woof_name);
    // Get subscription_event
    auto* subevent = static_cast<subscription_event*>(ptr);
    // Get host retry type
    enum RetryType retry_type = get_curr_retry_type();

    // NOTE: Description of loop at the end
    while (true) {  // BEGIN EVENT LOOP

    log_debug("[namespace:%d][node_id:%d][seqno:%lu] has event: [namespace:%d][node_id:%d][port:%d][iteration:%lu]",
            node_namespace,
            node_id,
            seqno,
            subevent->ns,
            subevent->id,
            subevent->port,
            subevent->itr);


    // Get current execution iteration
    execution_state curr_state;
    const std::string curr_state_woof = generate_woof_path(NODE_ST_WF_TYPE, node_namespace, (int)node_id);
#if 0
    const unsigned long long current_state_sequence_number = (unsigned long long)woof_last_seq(curr_state_woof);
    err = woof_get(curr_state_woof, &curr_state, (unsigned long)current_state_sequence_number);
#endif
    err = woof_get(curr_state_woof, &curr_state, 0);
    if (err < 0) {
        log_error("Error reading curr_state woof (s1): %s", curr_state_woof.c_str());
        log_error("EXIT: Subscription handler");
        return err;
    }
    log_debug(
        "[namespace:%d][node_id:%d] has current state: [iteration: %lu]", node_namespace, node_id, curr_state.itr);

    // Only proceed if this event is relevant to the current execution iteration
    const unsigned long long curr_itr = curr_state.itr;
    if (subevent->itr != curr_itr) {
        log_info("Event is not for current itr (event itr: %lu, current iteration: %lu)", subevent->itr, curr_itr);
        log_info("EXIT: Subscription handler");
        return 0;
    }
    if (curr_state.exec) {
        log_info("Event already picked up by a thread (ns: %ld, id: %ld, event itr: %lu)\n", node_namespace, node_id, subevent->itr);
        log_info("EXIT: Subscription handler");
        return 0;
    }

    // Look up subscriptions to determine required number of operands
    std::string submap = generate_woof_path(SUBN_MAP_WF_TYPE, node_namespace);
    std::string subdata = generate_woof_path(SUBN_DATA_WF_TYPE, node_namespace);
    unsigned long start_idx, end_idx;
    const unsigned long long last_seq = (unsigned long long)woof_last_seq(submap);

    err = woof_get(submap, &start_idx, node_id);
    if (err < 0) {
        log_error("Error reading submap woof (s1): %s", submap.c_str());
        log_error("EXIT: Subscription handler");
        return err;
    }

    if (node_id == (unsigned long)last_seq) {
        end_idx = woof_last_seq(subdata) + 1;
    } else {
        err = woof_get(submap, &end_idx, node_id + 1);
        if (err < 0) {
            log_error("Error reading submap woof (s2): %s", submap.c_str());
            log_error("EXIT: Subscription handler");
            return err;
        }
    }

    const unsigned long input_count = (unsigned long)end_idx - (unsigned long)start_idx;
    log_debug("[namespace:%d][node_id:%d] has %lu inputs", node_namespace, node_id, input_count);
    // Scan through subscription outputs and collect operands
    std::vector<operand> op_values(input_count);
    std::vector<subscription> input_hosts(input_count);
    for (unsigned long input_index = 0; input_index < input_count; input_index++) {
        log_debug("[input:%lu] START input processing", input_index);
        // Get last used output and seqno for this port
        cached_output cached_last_output;
        std::string last_used_sub_pos_woof =
            generate_woof_path(SUBN_POS_WF_TYPE, node_namespace, (int)node_id, -1, (int)input_index);
        const unsigned long long last_used_sub_pos_sequence_number = (unsigned long long)woof_last_seq(last_used_sub_pos_woof);
        if (last_used_sub_pos_sequence_number == 0) {
            // On first read, check if output woof is empty
            // Ignore and take the default cached_output
            log_info("[input:%lu] Fresh subscription node. No inputs yet", input_index);
        } else {
            log_debug("[input:%lu] Loading last_used_subscription_positions", input_index);
            woof_get(last_used_sub_pos_woof, &cached_last_output, (unsigned long)last_used_sub_pos_sequence_number);
        }

        log_debug("[input:%lu] [iteration:%lu] cached_itr: %lu", input_index, curr_itr, cached_last_output.op.itr);
        if (cached_last_output.op.itr == curr_itr) {
            // Operand for this seq has already been found and cached. Retrieve from cache and proceed
            op_values[input_index] = cached_last_output.op;
            log_debug("[input:%lu] Already retrieved, continue", input_index);
            log_debug("[input:%lu] END input processing", input_index);
            continue;
        } else if (cached_last_output.op.itr > curr_itr) {
            // Another handler has already cached an operand from a later execution iteration,
            // thus this execution iteration has already been completed, exit
            log_info("[input:%lu] Cached operand exceeds current execution iteration", input_index);
            log_info("EXIT: Subscription handler");
            return 0;
        } else {
            log_debug("[input:%lu] Not cached. Now grabbing.", input_index);
        }

        // Get subscription id
        subscription input_subscription;
        err = woof_get(subdata, &input_subscription, input_index + start_idx);
        if (err < 0) {
            log_error("Error reading subdata woof: %s", subdata.c_str());
            log_error("EXIT: Subscription handler");
            return err;
        }
        log_debug("[input:%lu] Subscription: [namespace:%d][node_id:%d][port:%d]",
                input_index,
                input_subscription.ns,
                input_subscription.id,
                input_subscription.port);
        input_hosts[input_index] = input_subscription;

        // Get relevant operand from subscription output (if it exists)
        std::string subscription_output_woof =
            generate_woof_path(OUT_WF_TYPE, input_subscription.ns, input_subscription.id);

        // Scan from last used output until finding current seq
        unsigned long long cached_sequence_number = (unsigned long long)cached_last_output.seq;
        unsigned long long subscription_sequence_number = (unsigned long long)woof_last_seq(subscription_output_woof);

    // if the subscription_sequence_number is invalid, retry to get valid sequence number
        int retry_itr = 1;
        while(WooFInvalid((unsigned long)subscription_sequence_number) &&
            (retry_itr < max_retries)) {
            subscription_sequence_number = (unsigned long long)woof_last_seq(subscription_output_woof);
            log_debug("[input:%lu] Retrying to get the subscription seq number. Unable to reach output woof : %s", 
                        input_index,
                        subscription_output_woof.c_str());
            retry_sleep(retry_type, retry_itr);
            retry_itr++;
        }
    if(retry_itr == max_retries) {
        log_error("[input:%lu] Giving up on last seqno for output woof %s after %d retries\n",
                input_index,
                subscription_output_woof.c_str(),
                max_retries);
        return(-1);
    }



        if (cached_sequence_number >= subscription_sequence_number) {
            log_info("[input:%lu] No new outputs to check (cached_seq=%lu, subscription_seq=%lu)",
                    input_index,
                    cached_sequence_number,
                    subscription_sequence_number);
            log_info("EXIT: Subscription handler");
            return 0;
        } else {
            log_debug("[input:%lu] New inputs to check (cached_seq=%lu, subscription_seq=%lu)",
                    input_index,
                    cached_sequence_number,
                    subscription_sequence_number);
        }

        // Increment sequence number (idx) until finding current execution iteration
        operand subscription_operand;
        unsigned long long new_sequence_number = (unsigned long long)cached_sequence_number;
        bool retrieved_data;
        do {
            new_sequence_number++;
            retrieved_data = false;
            retry_itr = 1;
            while(!retrieved_data) {
                err = woof_get(subscription_output_woof, &subscription_operand, (unsigned long)new_sequence_number);
                if (err < 0) {
                    log_error("Error reading output woof: %s at seqno %lu", subscription_output_woof.c_str(), new_sequence_number);
                    log_debug("Retrying reading from output woof: %s itr : %d", 
                                subscription_output_woof.c_str(), 
                                retry_itr);
                    retry_sleep(retry_type, retry_itr);
                    retry_itr++;
            if(retry_itr == max_retries) {
                log_error("Subscription handler: Max retries reading output woof: %s itr: %d, END\n",
                                subscription_output_woof.c_str(), 
                                retry_itr);
                return -1;
            }
                }
                else {
                    retrieved_data = true;
                }
            }

            log_debug("[input:%lu] Checking subscription output woof at sequence %lu: [subscription_itr:%d]",
                    input_index,
                    new_sequence_number,
                    subscription_operand.itr);
        } while (subscription_operand.itr < curr_itr && new_sequence_number < subscription_sequence_number);

        if (subscription_operand.itr != curr_itr) {
            log_error("[input:%lu] UNEXPECTED BEHAVIOR (SKIPPED EXECUTION ITER)", input_index);
            log_error(
                "[input:%lu] curr_itr: %lu, subscription itr: %lu", input_index, curr_itr, subscription_operand.itr);
            log_error("[input:%lu] List of previous subscription operands:", input_index);
            for (unsigned long long j = 1; j <= (unsigned long long)new_sequence_number; j++) {
                operand tmp;
                retry_itr = 1;
                retrieved_data = false;
                while(!retrieved_data) {
                    err = woof_get(subscription_output_woof, &tmp, (unsigned long)j);
                    if (err < 0) {
                        log_error("Error reading output woof: %s at seqno %lu", subscription_output_woof.c_str(),j);
                        log_debug("Retrying reading from output woof: %s itr : %d", 
                                    subscription_output_woof.c_str(), 
                                    retry_itr);
                        retry_sleep(retry_type, retry_itr);
                        retry_itr++;
            if(retry_itr == max_retries) {
                            log_error("Giving up on reading output woof: %s at seqno %lu after %d retries", 
                        subscription_output_woof.c_str(),j, max_retries);
                return(-1);
            }
                    }
                    else {
                        retrieved_data = true;
                    }
                }
                
                log_error_with_value("[input:%lu] [woof:%s][iteration:%lu]:",
                                    &tmp.operand_value,
                                    input_index,
                                    subscription_output_woof.c_str(),
                                    tmp.itr);
            }
            return -1;
        } else {
            log_debug_with_value("[input:%lu] Found input (iteration %lu at sequence_number %lu):",
                                &subscription_operand.operand_value,
                                input_index,
                                new_sequence_number,
                                subscription_operand.itr,
                                subscription_operand.operand_value);
        }

        // Cache latest idx back to `last used subscription position` woof
        if (subscription_operand.itr <= curr_itr) {
            log_debug_with_value("[input:%lu] Caching operand: (itr: %lu, seq: %lu)",
                                &subscription_operand.operand_value,
                                input_index,
                                subscription_operand.itr,
                                new_sequence_number);
            const cached_output new_cached_output = cached_output(subscription_operand, new_sequence_number);
            woof_put(last_used_sub_pos_woof, "", &new_cached_output);
        }

        if (subscription_operand.itr == curr_itr) {
            // Relevant operand found, save and continue
            log_debug_with_value("[input:%lu] Operand found: (woof_name: %s, curr_itr: %lu)",
                                &subscription_operand.operand_value,
                                input_index,
                                woof_name.c_str(),
                                curr_itr);
            op_values[input_index] = subscription_operand;
        } else {
            // At least one input is not ready --> exit handler
            log_info("[input:%lu] Operand not yet found (idx: %lu, curr_itr: %lu, op.itr: %lu)",
                    input_index,
                    new_sequence_number,
                    curr_itr,
                    subscription_operand.itr);
            log_info("EXIT: Subscription handler");
            return 0;
        }
        log_debug("[input:%lu] END input processing", input_index);
    }

    // Second check
    // Get current execution iteration
#if 0
    const unsigned long long current_state_sequence_number2 = (unsigned long long)woof_last_seq(curr_state_woof);
    err = woof_get(curr_state_woof, &curr_state, (unsigned long)current_state_sequence_number2);
#endif
    err = woof_get(curr_state_woof, &curr_state, 0);
    if (err < 0) {
        log_error("Error reading curr_state woof (s2): %s", curr_state_woof.c_str());
        log_error("EXIT: Subscription handler");
        return err;
    }

    // Only proceed if this event is relevant to the current execution iteration and not already executing
    if (subevent->itr != curr_state.itr) {
        log_info("Event is not for current itr (event itr: %lu, current iteration: %lu)", subevent->itr, curr_state.itr);
        log_info("EXIT: Subscription handler");
        return 0;
    }

    // For medium-long operations, this check will help keep the utilization of the snc_woof low
    if (curr_state.exec) {
        log_info("Event already picked up by a thread (ns: %ld, id: %ld, event itr: %lu)\n", node_namespace, node_id, subevent->itr);
        log_info("EXIT: Subscription handler");
        return 0;
    }

    // still could race -- double check with a log scan
#ifndef ESP8266
    int host_id = get_curr_host_id();
    const std::string node_snc_woof = generate_woof_path(NODE_SNC_WF_TYPE, node_namespace, node_id, host_id);
    unsigned long long snc_seqno;
    node_snc_event nse(node_namespace,node_id,host_id,curr_state.itr);
    snc_seqno = woof_put(node_snc_woof,"",&nse); // mark the end of the woof
    log_debug("Marked node %d itr: %d\n",node_id,(int)curr_state.itr);
    snc_seqno--;
    node_snc_event other_event;
    if(snc_seqno > 0) {
        err = woof_get(node_snc_woof,&other_event,snc_seqno);
    } else {
    err = -1; // handle first case
    }
    while(err >= 0) {
    if(memcmp(&nse,&other_event,sizeof(other_event)) == 0) {
            log_info("Event already picked up by a thread on scan (ns: %ld, id: %ld, event itr: %lu)\n", node_namespace, node_id, curr_state.itr);
            log_info("EXIT: Subscription handler");
        return(0);
    }
    snc_seqno--;
    if(snc_seqno == 0) { // don't read last entry as 0
        break;
    }
        err = woof_get(node_snc_woof,&other_event,snc_seqno);
    }

#endif
    /* Fire node */

    log_debug("Firing node");

    // Node has go ahead to execute, update the curr_state exec
    curr_state.exec = true;
    woof_put(curr_state_woof, "", &curr_state);

    // Get opcode
    node n;
    const std::string nodes_woof = generate_woof_path(NODES_WF_TYPE, node_namespace);
    err = woof_get(nodes_woof, &n, node_id);
    if (err < 0) {
    log_error("Could not find node  in %s at seqno %d\n",nodes_woof.c_str(),(int)node_id);
        return err;
    }

    const std::string output_woof = generate_woof_path(OUT_WF_TYPE, node_namespace, (int)node_id);

    struct df_operation_metadata operation_metadata = {.host_id = n.host_id,
                                                    .node_namespace = node_namespace,
                                                    .node_id = n.id,
                                                    .flow_control = {.execution_iteration = subevent->itr,
                                                                        .write_value = true,
                                                                        .output_woof_path = output_woof.c_str()}};
    log_debug("Firing operation %d\n",n.id);
    operand result = perform_operation(op_values, n.operation, &operation_metadata, input_hosts);
    log_debug("Fired operation %d\n",n.id);

    const unsigned long long last_output_sequence_number = (unsigned long long)woof_last_seq(output_woof);
    if (last_output_sequence_number == 0) {
        log_debug("No previous output, writing first one");
    } else {
        operand last_result;
        woof_get(output_woof, &last_result, (unsigned long)last_output_sequence_number);

        // Do not write result if it already exists
        if (!(n.operation.category == DF_INTERNAL && n.operation.operation == DF_INTERNAL_OFFSET) &&
            !(n.operation.category == DF_INTERNAL && n.operation.operation == DF_INTERNAL_FLATTEN)) {
            // Fix for offset: the following code prevents duplicates with most
            // nodes, but renders the offset node useless. For now, this feature
            // will be disabled for offset nodes. TODO: account for offset node's
            // `offset` to check for duplicates.
            // I think this can be fixed with operation_metadata.consumer_sequence instead of subevent->itr
            if (last_result.itr >= subevent->itr) {
                log_info("Operation already performed");
                log_info("EXIT: Subscription handler");
                return 0;
            }
        }
    }

    if (operation_metadata.flow_control.write_value) {
        write_value(&result.operand_value);
        woof_put(output_woof, OUTPUT_HANDLER, &result);
        log_debug("Wrote result of iteration: %lu", subevent->itr);
    }

    log_debug("END: Subscription handler");
    curr_state.itr++;
    curr_state.exec = false;
    woof_put(curr_state_woof, "", &curr_state);

    // Setup next iteration
    subevent->itr++;
    ++seqno;

    }   // END EVENT LOOP

    // NOTE: Removed the recursive call because of stack overlow in stream inputs. Changed to iterative.
    // Call handler for next iter in case all operands were received before this function finished
    // subscription_event_handler(wf, seqno + 1, subevent);

    return 0;
}

#ifdef ESP8266
extern "C" int Subscription_event_handler(char* wf, unsigned long seqno, void* ptr) {
        return(subscription_event_handler(wf,seqno,ptr));
}
#endif
