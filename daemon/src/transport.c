//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#include <daemon/transport.h>
#include <common/logging.h>
#include <common/utils.h>

#include <stdlib.h>

typedef struct cpg_name    cpg_name_t;
typedef struct cpg_address cpg_address_t;
typedef struct cpg_ring_id cpg_ring_id_t;

static void deliver_cb(cpg_handle_t      handle,
                       const cpg_name_t *groupName,
                       uint32_t          nodeid,
                       uint32_t          pid,
                       void             *msg,
                       size_t            msg_len)
{
    debug("deliver_cb()");
}

static void confchg_cb(cpg_handle_t         handle,
                       const cpg_name_t    *groupName,
                       const cpg_address_t *member_list,
                       size_t               member_list_entries,
                       const cpg_address_t *left_list,
                       size_t               left_list_entries,
                       const cpg_address_t *joined_list,
                       size_t               joined_list_entries)
{
    debug("confchg_cb()");
}

static void totem_confchg_cb(cpg_handle_t    handle,
                             cpg_ring_id_t   ring_id,
                             uint32_t        member_list_entries,
                             const uint32_t *member_list)
{
    debug("totem_confchg_cb()");
}

static cpg_model_v1_data_t model_v1_data = {
    .model                = CPG_MODEL_V1,
    .cpg_deliver_fn       = deliver_cb,
    .cpg_confchg_fn       = confchg_cb,
    .cpg_totem_confchg_fn = totem_confchg_cb,
    .flags                = CPG_MODEL_V1_DELIVER_INITIAL_TOTEM_CONF
};

transport_t *transport_init(const char *group_name)
{
    if (group_name == nullptr)
        return nullptr;

    transport_t *context = allocate(transport_t);
    if (context == nullptr)
        return nullptr;

    context->cpg_name = strdup(group_name);
    if (context->cpg_name == nullptr)
        goto failure;

    cs_error_t        rv         = CS_OK;
    cpg_model_t       model      = model_v1_data.model;
    cpg_model_data_t *model_data = (cpg_model_data_t *)&model_v1_data;

    rv = cpg_model_initialize(&context->cpg, model, model_data, nullptr);
    if (rv != CS_OK) {
        error("Failed to initialize cpg_model: %s: %s", cs_strerror(rv), strerrno);
        goto failure;
    }

    // TODO: Handle errors.
    cpg_name_t name;
    name.length = strlen(context->cpg_name);
    strncpy(name.value, context->cpg_name, sizeof(name.value));

    rv = cpg_join(context->cpg, &name);
    if (rv != CS_OK) {
        error("Failed to join cpg: %s: %s", cs_strerror(rv), strerrno);
        goto failure;
    }

    rv = cpg_local_get(context->cpg, &context->own_id);
    if (rv != CS_OK) {
        error("Failed to get local id: %s: %s", cs_strerror(rv), strerrno);
        goto failure;
    }

    return context;

failure:
    transport_deinit(context);
    return nullptr;
}

int transport_descriptor(transport_t *transport)
{
    if (transport == nullptr)
        return -1;

    int descriptor = -1;

    cs_error_t rv = cpg_fd_get(transport->cpg, &descriptor);
    if (rv != CS_OK) {
        error("Failed to get CPG file descriptor: %s", cs_strerror(rv));
        return -1;
    }

    return descriptor;
}

void transport_deinit(transport_t *context)
{
    if (context == nullptr)
        return;

    cpg_finalize(context->cpg);
    free((void *)context->cpg_name);
    free(context);
}
