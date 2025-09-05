#ifndef DF_INTERFACE_H
#define DF_INTERFACE_H

#include "df.h"
#include "woofc.h"

#include <map>
#include <set>
#include <string>
#pragma once
#include <string_view>

#define OUTPUT_HANDLER "output_handler"
#define SUBSCRIPTION_EVENT_HANDLER "subscription_event_handler"

void laminar_init(std::string appid_arg="APP");
void laminar_setup();
void laminar_reset();

void woof_create(const std::string& name, unsigned long element_size, unsigned long history_size);
unsigned long woof_put(const std::string& name, const std::string& handler, const void* element);
int woof_get(const std::string& name, void* element, unsigned long seq_no, bool print_error = true);
unsigned long woof_last_seq(const std::string& name);

void set_app_id(std::string app_id);
void set_host(int host_id);
void add_host(int host_id,
              const std::string& host_ip,
              const std::string& woof_path,
              enum RetryType retry_type = RETRY_EXPONENTIAL_BACKOFF);

void add_node(int ns, int host_id, int id, struct df_operation operation);
void add_operand(int ns, int host_id, int id);

void fire_operand(int ns, int id, const operand* op, bool trigger_handler = true);
int get_result(int ns, int id, operand* res, unsigned long itr);

void subscribe(const std::string& dst_addr, const std::string& src_addr);
void subscribe(int dst_ns, int dst_id, int dst_port, int src_ns, int src_id);

int get_curr_host_id();
enum RetryType get_curr_retry_type();
void retry_sleep(enum RetryType retry_type, int retry_itr);
int get_ns_from_woof_path(const std::string& woof_path);
unsigned long get_node_id_from_woof_path(const std::string& woof_path);
int set_app_id_from_woof_path(const std::string& woof_path);
std::string generate_woof_path(DFWoofType woof_type, int ns = -1, int node_id = -1, int host_id = -1, int port_id = -1);
std::string generate_woof_host_url(int host_id);
constexpr std::string_view ns_prefix = "lmr";

std::string graphviz_representation();

#endif // DF_INTERFACE_H
