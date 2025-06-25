#include "df.h"
#include "df_interface.h"
#ifndef ESP8266
#include "logger_system/df_logger.h"
#else
#include "df_logger.h"
#endif
#include "woofc.h"

#include <deque>
#include <iostream>

#ifdef CSPOTDEVICE
extern "C" int output_handler(char* wf, unsigned long seqno, void* ptr) {
#else
extern "C" int output_handler(WOOF* wf, unsigned long seqno, void* ptr) {
#endif
    const auto* result = static_cast<operand*>(ptr);
    log_init("DF-O", true);
    log_debug("START: Output handler");

    // Get name of this woof
#ifdef CSPOTDEVICE
    const std::string woof_name(wf);
#else
    const std::string woof_name(WoofGetFileName(wf));
#endif
    // Extract namespace
    const int node_namespace = get_ns_from_woof_path(woof_name);
    // Extract id
    const unsigned long node_id = get_node_id_from_woof_path(woof_name);
    int err = set_app_id_from_woof_path(woof_name);
    log_debug("output_handler: app_id info %s %s", (std::string(ns_prefix)).c_str(), woof_name.c_str());


    log_debug_with_value("woof: %s, [namespace:%d][node_id:%d][seqno:%lu] has result: [iteration:%lu]",
                         &result->operand_value,
			 woof_name.c_str(),
                         node_namespace,
                         node_id,
                         seqno,
                         result->itr);

    // Exit if double-fired
    if (seqno > 2) {
        operand prev;
        const int prev_err = woof_get(woof_name, &prev, seqno - 1);
        if (prev_err < 0) {
#ifdef ESP8266
            log_debug("Error reading output woof: %s", woof_name.c_str());
            log_debug("EXIT: Output handler");
#else
            log_error("Error reading output woof: %s", woof_name.c_str());
            log_error("EXIT: Output handler");
#endif
            return prev_err;
        }

        if (prev.itr == result->itr) {
            log_debug("Output already present");
            log_debug("EXIT: Output handler");

#ifdef LDGB
printf("Output already present\n");
printf("EXIT: Output handler\n");
#endif
            return 0;
        }
        if (prev.itr > result->itr) {
            log_debug("Previous output entry has a bigger iteration than the current %lu/%lu", prev.itr, result->itr);
            log_debug("EXIT: Output handler");
            return 0;
        }
    }

    // Get subscribers
    const std::string submap_woof = generate_woof_path(SUBR_MAP_WF_TYPE, node_namespace);
    const std::string subdata_woof = generate_woof_path(SUBR_DATA_WF_TYPE, node_namespace);

    unsigned long start_idx;
    const int start_idx_err = woof_get(submap_woof, &start_idx, node_id);
    if (start_idx_err < 0) {
#ifdef ESP8266
        log_debug("Error reading submap woof (start): %s", submap_woof.c_str());
        log_debug("EXIT: Output handler");
#else
        log_error("Error reading submap woof (start): %s", submap_woof.c_str());
        log_error("EXIT: Output handler");
#endif
        return start_idx_err;
    }

    unsigned long end_idx;
    if (node_id == woof_last_seq(submap_woof)) {
        end_idx = woof_last_seq(subdata_woof) + 1;
    } else {
        const int end_idx_err = woof_get(submap_woof, &end_idx, node_id + 1);
        if (end_idx_err < 0) {
#ifdef ESP8266
            log_debug("Error reading submap woof (end): %s", submap_woof.c_str());
            log_debug("EXIT: Output handler");
#else
            log_error("Error reading submap woof (end): %s", submap_woof.c_str());
            log_error("EXIT: Output handler");
#endif
            return end_idx_err;
        }
    }

    // Iterate over subscribers and push to respective woofs
    std::deque<subscription_event> event_buffer;
    for (unsigned long i = start_idx; i < end_idx; i++) {
        // Get subscriber data
        subscriber sub;
        const int sub_error = woof_get(subdata_woof, &sub, i);
        if (sub_error < 0) {
#ifdef ESP8266
            log_debug("Error reading subdata woof: %s", submap_woof.c_str());
            log_debug("EXIT: Output handler");
#else
            log_error("Error reading subdata woof: %s", submap_woof.c_str());
            log_error("EXIT: Output handler");
#endif
            return sub_error;
        }

        const std::string subscriber_woof = generate_woof_path(SUBN_EVNT_WF_TYPE, sub.ns, sub.id);
        subscription_event subevent(sub.ns, sub.id, sub.port, result->itr);

        const unsigned long put_result = woof_put(subscriber_woof, SUBSCRIPTION_EVENT_HANDLER, &subevent);
        log_debug(
            "[namespace:%d][node_id:%d][iteration:%lu] create subscription event: [namespace:%lu][node_id:%d][port:%d]",
            node_namespace,
            node_id,
            result->itr,
            subevent.ns,
            subevent.id,
            subevent.port);

        // add to the buffer if it is a remote woof which could not be put
        if (put_result == (unsigned long)-1 && !subscriber_woof.rfind("woof://", 0)) {
            log_debug("[namespace:%d][node_id:%d][iteration:%lu] send failed, buffer event: "
                      "[namespace:%lu][node_id:%d][port:%d]",
                      node_namespace,
                      node_id,
                      result->itr,
                      subevent.ns,
                      subevent.id,
                      subevent.port);
            event_buffer.push_back(subevent);
        }
    }

    // keep retrying to send the subscription events in the buffer until empty based on the selected retry mechanism
    enum RetryType retry_type = get_curr_retry_type();

    int retry_itr = 0;
    while (!event_buffer.empty()) {
        const size_t event_count = event_buffer.size();
        for (size_t event_idx = 0; event_idx < event_count; event_idx++) {
            const subscription_event subevent = event_buffer.front();
            event_buffer.pop_front();

            const std::string subscriber_woof =
                generate_woof_path(SUBN_EVNT_WF_TYPE, subevent.ns, subevent.id);
            const unsigned long put_result = woof_put(subscriber_woof, SUBSCRIPTION_EVENT_HANDLER, &subevent);
            log_debug("[namespace:%d][node_id:%d][iteration:%lu] create retry subscription event: "
                      "[namespace:%lu][node_id:%d][port:%d]",
                      node_namespace,
                      node_id,
                      result->itr,
                      subevent.ns,
                      subevent.id,
                      subevent.port);

            /* add back the buffer if it is a remote woof which could not be put */
            if (WooFInvalid(put_result)) {
                log_debug("[namespace:%d][node_id:%d][iteration:%lu] retry failed for subscription event: "
                          "[namespace:%lu][node_id:%d][port:%d]",
                          node_namespace,
                          node_id,
                          result->itr,
                          subevent.ns,
                          subevent.id,
                          subevent.port);
                event_buffer.push_back(subevent);
            }
        }

        if (!event_buffer.empty()) {
            retry_sleep(retry_type, retry_itr);
            retry_itr++;
        }
    }

    log_debug("END: Output handler");
    return 0;
}

#ifdef ESP8266
extern "C" int Output_handler(char* wf, unsigned long seqno, void* ptr) {
        int ret = output_handler(wf,seqno,ptr);
        return(ret);
}
#endif
