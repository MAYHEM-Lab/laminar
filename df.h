#ifndef DF_H
#define DF_H

#ifdef ESP8266
#define CSPOTDEVICE
#include "df_operations.h"
#include "ts_types.h"
#include "ts_type.h"
#else
#include "operation_system/df_operations.h"
#include "type_system/ts_types.h"
#include "type_system/ts_type.h"
#endif

#include <cmath>
#include <cstring>
#include <iostream>

#define DFWOOFTYPES                                                                                                    \
    WOOF_TYPE(OUT_WF_TYPE)                                                                                        \
    WOOF_TYPE(SUBN_EVNT_WF_TYPE)                                                                           \
    WOOF_TYPE(SUBR_MAP_WF_TYPE)                                                                                \
    WOOF_TYPE(SUBR_DATA_WF_TYPE)                                                                               \
    WOOF_TYPE(SUBN_MAP_WF_TYPE)                                                                              \
    WOOF_TYPE(SUBN_DATA_WF_TYPE)                                                                             \
    WOOF_TYPE(SUBN_POS_WF_TYPE)                                                                              \
    WOOF_TYPE(NODES_WF_TYPE)                                                                                         \
    WOOF_TYPE(HOST_ID_WF_TYPE)                                                                                       \
    WOOF_TYPE(HOSTS_WF_TYPE)                                                                                         \
    WOOF_TYPE(SETUP_ST_WF_TYPE)                                                                                   \
    WOOF_TYPE(NODE_SNC_WF_TYPE)                                                                                        \
    WOOF_TYPE(NODE_ST_WF_TYPE)

enum DFWoofType {
#define WOOF_TYPE(name) name,
    DFWOOFTYPES
#undef WOOF_TYPE
        DFWOOFTYPES_N
};

static const char* DFWOOFTYPE_STR[DFWOOFTYPES_N] = {
#define WOOF_TYPE(name) [name] = #name,
    DFWOOFTYPES
#undef WOOF_TYPE
};

enum RetryType {
    RETRY_IMMEDIATE = 0,
    RETRY_INTERVAL,
    RETRY_LINEAR_BACKOFF,
    RETRY_EXPONENTIAL_BACKOFF,
    RETRY_LOG_BACKOFF,
};

enum SetupState {
    SETUP_STARTED = 0,
    SETUP_FINISHED,
};

struct operand {
    struct ts_value operand_value {
        .type = TS_UNINITIALIZED, .value = {}
    };                 // the value of the operand
//    unsigned long itr; // the iteration for which it is the result
    unsigned long long itr; // uint is common to x86 and Rpi

#define DF_TIMING
#ifdef DF_TIMING
    unsigned int timestamp;
#endif // DF_TIMING

    explicit operand(struct ts_value* _value = nullptr, unsigned long long itr = 1)
        : itr(itr) {
        if (_value != nullptr) {
            operand_value = *_value;
        }
    }
};

struct cached_output {
    operand op;
    unsigned long long seq; // CSPOT seq in output woof
    int input_ns;
    int input_id;

    // Defaults execution iteration and seq to 0 so initial access is thrown out and updated
    // We are now storing the source of the cached outputs so we can fetch later in case of aggregate data 
    explicit cached_output(const operand& op = operand(nullptr, 0), unsigned long long seq = 0, int input_ns = -1, int input_id = -1)
        : op(op)
        , seq(seq)
        , input_ns(input_ns)
        , input_id(input_id) {
    }
};

struct subscriber {
    int ns;
    int id;
    int port;

    explicit subscriber(int dst_ns = 0, int dst_id = 0, int dst_port = 0)
        : ns(dst_ns)
        , id(dst_id)
        , port(dst_port) {
    }

    bool operator<(const subscriber& other) const {
        if (ns < other.ns) return true;
        if (ns > other.ns) return false;
        if (id < other.id) return true;
        if (id > other.id) return false;
        return port < other.port;
    }
};

struct subscription {
    int ns;
    int id;
    int port;

    explicit subscription(int src_ns = 0, int src_id = 0, int dst_port = 0)
        : ns(src_ns)
        , id(src_id)
        , port(dst_port) {
    }

    bool operator<(const subscription& other) const {
        return port < other.port;
    }
};

struct subscription_event {
    int ns;
    int id;
    int port;
//    unsigned long itr; 
    unsigned long long itr; // uint64 is common to x86 and Rpi

//    explicit subscription_event(int ns = 0, int id = 0, int port = 0, unsigned long itr = 0)
    explicit subscription_event(int ns = 0, int id = 0, int port = 0, unsigned long long itr = 0)
        : ns(ns)
        , id(id)
        , port(port)
        , itr(itr) {
    }
};

struct node_snc_event {
    int ns;
    int node_id;
    int host_id;
    unsigned long long itr; // uint64 is common to x86 and Rpi

//    explicit subscription_event(int ns = 0, int id = 0, int port = 0, unsigned long itr = 0)
    explicit node_snc_event(int ns = 0, int node_id = 0, int host_id = 0, unsigned long long itr = 0)
        : ns(ns)
        , node_id(node_id)
        , host_id(host_id)
        , itr(itr) {
    }
};



struct node {
    int id;
    int host_id;
    struct df_operation operation;

    explicit node(int id = 0,
                  int host_id = 0,
                  struct df_operation operation = {.category = DF_INTERNAL, .operation = DF_INTERNAL_OPERAND})
        : id(id)
        , host_id(host_id)
        , operation(operation) {
    }

    bool operator<(const node& other) const {
        return id < other.id;
    }
};


struct host {
    int host_id;
    char host_url[200];
    enum RetryType retry_type;

    explicit host(const int host_id_ = 0,
                  const char host_url_[200] = "",
                  enum RetryType retry_type_ = RETRY_EXPONENTIAL_BACKOFF) {
//		    enum RetryType retry_type_ = RETRY_INTERVAL) {
//		    enum RetryType retry_type_ = RETRY_IMMEDIATE) {
        host_id = host_id_;
        strcpy(host_url, host_url_);
        retry_type = retry_type_;
    }

    bool operator<(const host& other) const {
        return host_id < other.host_id;
    }
};


struct execution_state {
//    unsigned long itr; // Current execution iteration
    unsigned long long itr; // Current execution iteration
    bool exec;

    explicit execution_state(unsigned long long itr = 1, bool exec = false)
        : itr(itr), exec(exec) {
    }
};


// #define DEBUG_DF

#ifdef DEBUG_DF
#define DEBUG_PRINT(str)                                                                                               \
    { std::cout << str << std::endl << std::flush; }
#else
#define DEBUG_PRINT(str)                                                                                               \
    {}
#endif

#endif
