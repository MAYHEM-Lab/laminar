#ifdef ESP8266
#include <ESP8266WiFi.h>
#endif
#include "df_interface.h"

#include "df.h"

#include <chrono>
#include <iostream>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <vector>

#ifdef ESP8266
#define OUTPUT_WOOF_SIZE 10
#define SUBSCRIPTION_EVENTS_WOOF_SIZE 10
#define SUBSCRIPTION_EVENTS_SNC_WOOF_SIZE 10
#define NODE_STATE_WOOF_SIZE 10
#define SUBSCRIPTION_POS_WOOF_SIZE 10
#else
#define OUTPUT_WOOF_SIZE 3000
#define SUBSCRIPTION_EVENTS_WOOF_SIZE 3000
#define SUBSCRIPTION_EVENTS_SNC_WOOF_SIZE 3000
#define NODE_STATE_WOOF_SIZE 3000
#define SUBSCRIPTION_POS_WOOF_SIZE 3000
#endif

// {namspace --> entries}
std::map<int, int> subscribe_entries;
// {namespace --> {node_id --> [subscribers...]}}
std::map<int, std::map<int, std::set<subscriber>>> subscribers;
// {namespace --> {node_id --> [subscriptions...]}}
std::map<int, std::map<int, std::set<subscription>>> subscriptions;
// {namespace --> [nodes...]}
std::map<int, std::set<node>> nodes;
// set of host structs for url extraction
std::set<host> hosts;

enum SetupState setup_state;

int get_curr_host_id() {
    int curr_host_id;
    int err = woof_get(generate_woof_path(HOST_ID_WF_TYPE), &curr_host_id, 1);
    if (err < 0) {
        std::cout << "Error obtaining current host id " << std::endl;
        exit(1);
    }
    return curr_host_id;
}

enum RetryType get_curr_retry_type() {

    host h;
    int curr_host_id = get_curr_host_id();

    int err = woof_get(generate_woof_path(HOSTS_WF_TYPE), &h, curr_host_id);
    if (err < 0) {
        std::cout << "Error reading the host retry info for host id: " << std::to_string(curr_host_id) << std::endl;
        exit(1);
    }

    return h.retry_type;
}

void retry_sleep(enum RetryType retry_type, int retry_itr) {
    if (retry_type == RETRY_IMMEDIATE) {
        return;
    } else if (retry_type == RETRY_INTERVAL) {
#ifdef ESP8266
	delay(10000);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
#endif
    } else if (retry_type == RETRY_LINEAR_BACKOFF) {
        int ms = std::min((retry_itr)*2000, 32000);
#ifdef ESP8266
	delay(ms);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
    } else if (retry_type == RETRY_EXPONENTIAL_BACKOFF) {
        int ms = std::min((int(pow(2, retry_itr)) * 1000) + (rand() % 100), 32000);
#ifdef ESP8266
	delay(ms);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#endif
    }
}

std::string generate_woof_path(
    const DFWoofType woof_type, const int ns, const int node_id, int host_id, const int port_id) {
    std::string host_url;
    std::string woof_prefix = "lmr";

    // if it is a global woof to a host then ns is -1
    if (ns == -1) {
        return woof_prefix + "." + DFWOOFTYPE_STR[woof_type];
    }

    // namespace level static woof return directly.
    if (node_id == -1) {
        return woof_prefix + "-" + std::to_string(ns) + "." + DFWOOFTYPE_STR[woof_type];
    }

    if (host_id == -1) {
        // get the host of the node from the node level woofs
        std::string nodes_woof_path =
            host_url + woof_prefix + "-" + std::to_string(ns) + "." + DFWOOFTYPE_STR[NODES_WF_TYPE];
        node n;
        int err = woof_get(nodes_woof_path, &n, node_id);
        if (err < 0) {
            std::cout << "Error reading the node info for node id: " << std::to_string(node_id)
                      << " node ns : " << std::to_string(ns) << std::endl;
            exit(1);
        }
        host_id = n.host_id;
    }

    // assign a host url only if it is not local; extract from HOSTS_WOOF
    if (host_id != get_curr_host_id()) {
        host h;
        int err = woof_get(woof_prefix + "." + DFWOOFTYPE_STR[HOSTS_WF_TYPE], &h, host_id);
        if (err < 0) {
            std::cout << "Error reading the host info for host id: " << std::to_string(host_id) << std::endl;
            exit(1);
        }
        host_url.assign(h.host_url);
    }

    std::string woof_path = host_url + woof_prefix + "-" + std::to_string(ns) + "." + DFWOOFTYPE_STR[woof_type] + "." +
                            std::to_string(node_id);

    if (woof_type == SUBN_POS_WF_TYPE) {
        woof_path = woof_path + "." + std::to_string(port_id);
    }

    return woof_path;
}

std::string generate_woof_host_url(int host_id) {
    std::string host_url;
    host h;
    std::string woof_prefix = "lmr";

    if(host_id != get_curr_host_id()) {
    	int err = woof_get(woof_prefix + "." + DFWOOFTYPE_STR[HOSTS_WF_TYPE], &h, host_id);
    	if (err < 0) {
        	std::cout << "Error read the host info for URL for host id: " << std::to_string(host_id) << std::endl;
        	exit(1);
    	}
//printf("generate_woof_host_url: %s for id %d\n",h.host_url,host_id);
    	return(h.host_url);
    } else {
	return("");
    }
}

/**
 * Will retrieve laminar node_id from woof path:
 * woof://host_url/laminar-namespace.woof_type.node_id -> node_id
 * @param woof_path complete woof_path as reference
 * @return
 */
unsigned long get_node_id_from_woof_path(const std::string& woof_path) {
    const size_t last_dot = woof_path.find_last_of('.');
    const std::string node_id_str = woof_path.substr(last_dot + 1);
    return std::stoul(node_id_str);
}

/**
 * Will retrieve laminar namespace from woof path:
 * woof://host_url/laminar-namespace.woof_type.node_id -> namespace
 * @param woof_path complete woof_path as reference
 * @return
 */
int get_ns_from_woof_path(const std::string& woof_path) {
    const size_t dash = woof_path.find('-');
    const std::string ns_plus_ending_str = woof_path.substr(dash + 1);
    const size_t first_dot = ns_plus_ending_str.find('.');
    const std::string ns_str = ns_plus_ending_str.substr(0, first_dot);
    return std::stoi(ns_str);
}

void laminar_init() {
#ifndef ESP8266
    WooFInit();
#endif

    std::string setup_state_woof = generate_woof_path(SETUP_ST_WF_TYPE);
    // check if setup state exists; if not then create it and set state as STARTED
    const unsigned long current_state_sequence_number = woof_last_seq(setup_state_woof);
    if (WooFInvalid(current_state_sequence_number)) {
        woof_create(setup_state_woof, sizeof(enum SetupState), 2);
        setup_state = SETUP_STARTED;
        woof_put(setup_state_woof, "", &setup_state);
    } else {
        woof_get(setup_state_woof, &setup_state, current_state_sequence_number, false);
    }
}


void woof_create(const std::string& name, const unsigned long element_size, const unsigned long history_size) {
#ifndef ESP8266
    WooFInit();
#endif
    const int err = WooFCreate(name.c_str(), element_size, history_size);
    if (err < 0) {
        std::cerr << "ERROR -- creation of " << name << " failed\n";
        exit(1);
    }
}

unsigned long woof_put(const std::string& name, const std::string& handler, const void* element) {
    unsigned long seqno;
    if (handler.empty()) {
        seqno = WooFPut(name.c_str(), nullptr, element);
    } else {
        seqno = WooFPut(name.c_str(), handler.c_str(), element);
    }

    if (WooFInvalid(seqno)) {
        std::cerr << "ERROR -- put to " << name << " failed\n";
    }
    return seqno;
}

int woof_get(const std::string& name, void* element, unsigned long seq_no, bool print_err) {
    int err = WooFGet(name.c_str(), element, seq_no);
    if (err < 0 && print_err) {
        std::cerr << "ERROR -- get [" << seq_no << "] from " << name << " failed\n";
    }
    return err;
}

unsigned long woof_last_seq(const std::string& name) {
    return WooFGetLatestSeqno(name.c_str());
}

std::vector<std::string> split(const std::string& s, char delim = ',') {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = s.find(delim);
    std::string value;

    while (end != std::string::npos) {
        value = s.substr(start, end - start);
        result.push_back(value);
        start = end + 1;
        end = s.find(delim, start);
    }

    value = s.substr(start, end);
    result.push_back(value);
    return result;
}

void add_host(const int host_id, const std::string& host_ip, const std::string& woof_path, enum RetryType retry_type) {
    // global info stored in every host
    const std::string host_url = "woof://" + host_ip + woof_path;
    char host_url_c_str[200];
    strcpy(host_url_c_str, host_url.c_str());
    hosts.insert(host(host_id, host_url_c_str, retry_type));
}

void set_host(int host_id) {
    if (setup_state == SETUP_FINISHED) {
        return;
    }

    const std::string hosts_woof_path = generate_woof_path(HOST_ID_WF_TYPE);
    woof_create(hosts_woof_path, sizeof(int), 1);
    woof_put(hosts_woof_path, "", &host_id);
}

void add_node(const int ns, const int host_id, const int id, const struct df_operation operation) {
    // global info stored in every host
    nodes[ns].insert(node(id, host_id, operation));

    if (setup_state == SETUP_FINISHED) {
        return;
    }

    const int curr_host_id = get_curr_host_id();

    // create node related info only for current host
    if (host_id == curr_host_id) {
        woof_create(generate_woof_path(OUT_WF_TYPE, ns, id, host_id), sizeof(operand), OUTPUT_WOOF_SIZE);

        // Create subscription_events woof
        woof_create(generate_woof_path(SUBN_EVNT_WF_TYPE, ns, id, host_id),
                    sizeof(subscription_event),
                    SUBSCRIPTION_EVENTS_WOOF_SIZE);

#ifndef CSPOTDEVICE
	// create sync woof to ensure handler is fired only once on multithreaded platforms
        woof_create(generate_woof_path(NODE_SNC_WF_TYPE, ns, id, host_id),
                    sizeof(node_snc_event),
                    SUBSCRIPTION_EVENTS_SNC_WOOF_SIZE);
#endif

        // Create node state woof
        const std::string node_state_woof = generate_woof_path(NODE_ST_WF_TYPE, ns, id, host_id);
        execution_state init_state;
        // TODO: node_state_woof should be of size 1, but CSPOT hangs when
        // writing to full woof (instead of overwriting), so the size has
        // been increased temporarily as a stop-gap measure while testing
        woof_create(node_state_woof, sizeof(execution_state), NODE_STATE_WOOF_SIZE);
        woof_put(node_state_woof, "", &init_state);
    }
}

void add_operand(const int ns, const int host_id, const int id) {
    struct df_operation operation = {.category = DF_INTERNAL, .operation = DF_INTERNAL_OPERAND};
    // global info stored in every host
    nodes[ns].insert(node(id, host_id, operation));

    if (setup_state == SETUP_FINISHED) {
        return;
    }

    const int curr_host_id = get_curr_host_id();
    // Create output woof if the operand belongs to this host only
    if (host_id == curr_host_id) {
        woof_create(generate_woof_path(OUT_WF_TYPE, ns, id, host_id), sizeof(operand), OUTPUT_WOOF_SIZE);
    }
}

void fire_operand(const int ns, const int id, const operand* const op, const bool trigger_handler) {
    std::string output_woof = generate_woof_path(OUT_WF_TYPE, ns, id);
    unsigned long long curr_itr = (unsigned long long)woof_last_seq(output_woof);
    // fire only if the iteration number is 1 more then the previous iteration
printf("fire_operand: output_woof: %s, value: %f\n",
		output_woof.c_str(),
		op->operand_value.value.ts_double);

    if (curr_itr + 1 == op->itr) {
        if (!trigger_handler) {
            woof_put(output_woof, "", op);
        } else {
            woof_put(output_woof, OUTPUT_HANDLER, op);
        }
    }
}

int get_result(const int ns, const int id, operand* const res, const unsigned long itr) {
    std::string woof_name = generate_woof_path(OUT_WF_TYPE, ns, id);
    int ierr;
    unsigned long iseqno;

    // wait till output log has atleast itr number of results
    while(1) {
//printf("wf: %s, iseqno: %lu, itr: %lu\n",woof_name.c_str(),iseqno,itr);
//fflush(stdout);
    	iseqno = woof_last_seq(woof_name);
	if(iseqno == (unsigned long)-1) {
        	std::cerr << "ERROR -- get last seqno for itr " << itr << " failed\n";
		return(-1);
	}
	if(iseqno >= itr) {
		break;
	}
	sleep(1);
    }
   // while (woof_last_seq(woof_name) < itr) {sleep(1);}
    //while(WooFGetLatestSeqno(woof_name.c_str()) < itr){}

    // keep checking down till the result is of iteration itr
    operand last_op;
    unsigned long seqno = itr;
    ierr = woof_get(woof_name, &last_op, seqno);
    if(ierr < 0) {
        std::cerr << "ERROR -- get [" << itr << "] at " << iseqno << " from " << woof_name << " failed\n";
	return(-1);
    }
//printf("lastop: wf: %s, seqno: %lu, itr: %lu lastitr: %lu\n",woof_name.c_str(),seqno,itr,last_op.itr);
//fflush(stdout);

    while (last_op.itr != (unsigned long long)itr) {
        // std::cout << last_op.itr << " : " << itr << " : " << seqno <<  std::endl;
        // wait till the next seqno is populated with result
        seqno++;
	while(1) {
		iseqno = woof_last_seq(woof_name);
//printf("lastop iseq: wf: %s, seqno: %lu, iseqno: %lu itr: %lu lastitr: %lu\n",woof_name.c_str(),seqno,iseqno,itr,last_op.itr);
//fflush(stdout);
		if(iseqno == (unsigned long)-1) {
        		std::cerr << "ERROR -- get last seqno  from " << woof_name << " failed\n";
			return(-1);
		}
		if(iseqno >= seqno) {
			break;
		}
		sleep(1);
	}
        //while (woof_last_seq(woof_name) < seqno) {sleep(1);}
        //while (WooFGetLatestSeqno(woof_name.c_str()) < seqno) {}
        ierr = woof_get(woof_name, &last_op, seqno);
//printf("get iseq: wf: %s, seqno: %lu, iseqno: %lu itr: %lu lastitr: %lu\n",woof_name.c_str(),seqno,iseqno,itr,last_op.itr);
//fflush(stdout);
	if(ierr < 0) {
        std::cerr << "ERROR -- get [" << itr << "] at " << seqno << " from " << woof_name << " failed\n";
		return(-1);
	}
    }

    // fill the result when ready
    int err = woof_get(woof_name, res, seqno);
    if (err < 0) {
        std::cerr << "ERROR -- get [" << itr << "] from " << woof_name << " failed\n";
    }
    return err;
}


void subscribe(const int dst_ns, const int dst_id, const int dst_port, const int src_ns, const int src_id) {
    subscribers[src_ns][src_id].insert(subscriber(dst_ns, dst_id, dst_port));
    subscriptions[dst_ns][dst_id].insert(subscription(src_ns, src_id, dst_port));

    subscribe_entries[src_ns]++;
    subscribe_entries[dst_ns]++;
}

void subscribe(const std::string& dst_addr, const std::string& src_addr) {
    std::vector<std::string> dst = split(dst_addr, ':');
    std::vector<std::string> src = split(src_addr, ':');

    subscribe(std::stoi(dst[0]), std::stoi(dst[1]), std::stoi(dst[2]), std::stoi(src[0]), std::stoi(src[1]));
}

void laminar_setup() {
    // Recreate state before crash
    if (setup_state == SETUP_FINISHED) {
        auto start = std::chrono::high_resolution_clock::now();
        int node_count = 0;

        for (const auto& [ns, value] : nodes) {
            for (auto& node : nodes[ns]) {
                // if operand (node) then ignore
                if (node.operation.category == DF_INTERNAL && node.operation.operation == DF_INTERNAL_OPERAND) {
                    continue;
                }
                node_count++;

                // get the execution state of the node
                execution_state exec_state;
                std::string state_woof_path = generate_woof_path(NODE_ST_WF_TYPE, ns, node.id, node.host_id);
#if 0
                const unsigned long current_state_sequence_number = woof_last_seq(state_woof_path);
                woof_get(state_woof_path, &exec_state, current_state_sequence_number);
#endif
		woof_get(state_woof_path, &exec_state, 0);
                std::string subscriber_woof = generate_woof_path(SUBN_EVNT_WF_TYPE, ns, node.id);

                subscription_event subevent(ns, node.id, -1, exec_state.itr);
                woof_put(subscriber_woof, SUBSCRIPTION_EVENT_HANDLER, &subevent);
                }
            }

        std::cout << "Nodes : " << node_count << " Duration : "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() -
                                                                           start)
                         .count()
                  << " ms " << std::endl;
    } else {
        // setup the host related info
        // TODO: setup check for host_url size
        woof_create(generate_woof_path(HOSTS_WF_TYPE), sizeof(host), hosts.size());
        for (auto& host : hosts) {
            woof_put(generate_woof_path(HOSTS_WF_TYPE), "", &host);
        }

        // setup all the namespaces
        int curr_host_id = get_curr_host_id();
        for (const auto& [ns, value] : nodes) {
            // Create woof to hold node data
            woof_create(generate_woof_path(NODES_WF_TYPE, ns), sizeof(node), nodes[ns].size());
            for (auto& node : nodes[ns]) {
                woof_put(generate_woof_path(NODES_WF_TYPE, ns), "", &node);
            }

            // Create woof hashmaps to hold subscribers
            woof_create(generate_woof_path(SUBR_MAP_WF_TYPE, ns), sizeof(unsigned long), nodes[ns].size());
            woof_create(generate_woof_path(SUBR_DATA_WF_TYPE, ns), sizeof(subscriber), subscribe_entries[ns]);
            unsigned long current_data_pos = 1;
            for (size_t i = 1; i <= nodes[ns].size(); i++) {
                // Add entry in map (idx = node id, val = start idx in subscriber_data)
                woof_put(generate_woof_path(SUBR_MAP_WF_TYPE, ns), "", &current_data_pos);

                for (auto& sub : subscribers[ns][i]) {
                    woof_put(generate_woof_path(SUBR_DATA_WF_TYPE, ns), "", &sub);
                    current_data_pos++;
                }
            }

            // Create woof hashmaps to hold subscriptions
            woof_create(generate_woof_path(SUBN_MAP_WF_TYPE, ns), sizeof(unsigned long), nodes[ns].size());
            woof_create(
                generate_woof_path(SUBN_DATA_WF_TYPE, ns), sizeof(subscription), subscribe_entries[ns]);
            current_data_pos = 1;
            for (size_t i = 1; i <= nodes[ns].size(); i++) {
                // Add entry in map (idx = node id, val = start idx in subscription_data)
                woof_put(generate_woof_path(SUBN_MAP_WF_TYPE, ns), "", &current_data_pos);

                for (auto& sub : subscriptions[ns][i]) {
                    woof_put(generate_woof_path(SUBN_DATA_WF_TYPE, ns), "", &sub);
                    current_data_pos++;
                }
            }

            int node_id = 1;
            for (auto& node : nodes[ns]) {
                // Add woofs to hold last used seq in subscription output woof
                if (curr_host_id == node.host_id) {
                    for (size_t port = 0; port < subscriptions[ns][node_id].size(); port++) {
                        woof_create(generate_woof_path(SUBN_POS_WF_TYPE, ns, node_id, -1, port),
                                    sizeof(cached_output),
                                    SUBSCRIPTION_POS_WOOF_SIZE);
                    }
                }
                node_id++;
            }
        }

        // when setup is finished set state as finished
        enum SetupState finished_setup_state = SETUP_FINISHED;
        woof_put(generate_woof_path(SETUP_ST_WF_TYPE), "", &finished_setup_state);
    }
}

void laminar_reset() {
    // {namspace --> entries}
    subscribe_entries = std::map<int, int>();
    // {namespace --> {id --> [subscribers...]}}
    subscribers = std::map<int, std::map<int, std::set<subscriber>>>();
    // {namespace --> {id --> [subscriptions...]}}
    subscriptions = std::map<int, std::map<int, std::set<subscription>>>();
    // {namespace --> [nodes...]}
    nodes = std::map<int, std::set<node>>();
    // set of host structs for url extraction
    hosts = std::set<host>();
}

std::string graphviz_representation() {
    std::string g = "digraph G {\n\tnode [shape=\"record\", style=\"rounded\"];";

    // Add nodes
    for (auto& [ns, ns_nodes] : nodes) {
        g += "\n\tsubgraph cluster_" + std::to_string(ns) + " { ";
        g += "\n\t\tlabel=\"Subgraph #" + std::to_string(ns) + "\";";

        auto n = ns_nodes.begin();
        auto s = subscriptions[ns].begin();
        while (n != ns_nodes.end()) {
            g += "\n\t\tnode_" + std::to_string(ns) + "_" + std::to_string(n->id) + " [label=\"{";
            // Add ports
            if (!(n->operation.category == DF_INTERNAL && n->operation.operation == DF_INTERNAL_OPERAND)) {
                g += "{";
                for (size_t port = 0; port < s->second.size(); port++) {
                    std::string p = std::to_string(port);
                    g += "<";
                    g += p;
                    g += "> ";
                    g += p;
                    if (port < s->second.size() - 1) {
                        g += '|';
                    }
                }
                g += "}|";
            }

            char* operation_string = operation_to_string(n->operation);
            g += "<out>[" + std::string(operation_string);
            free(operation_string);
            g += "]\\nNode #" + std::to_string(n->id) + "}\"];";

            n++;

            if (!(n->operation.category == DF_INTERNAL && n->operation.operation == DF_INTERNAL_OPERAND)) {
                s++;
            }
        }

        g += "\n\t}";
    }

    // Add edges
    for (auto& [ns, ns_subscriptions] : subscriptions) {
        for (auto& [id, subs] : ns_subscriptions) {
            for (auto s : subs) {
                g += "\n\tnode_" + std::to_string(s.ns) + "_" + std::to_string(s.id) + ":out -> ";
                g += "node_" + std::to_string(ns) + "_" + std::to_string(id) + ":" + std::to_string(s.port) + ";";
            }
        }
    }

    g += "\n}";

    return g;
}
