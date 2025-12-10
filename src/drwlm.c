#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>

#include <sys/uio.h>

#include <pthread.h>

#include <corosync/cpg.h>
#include <corosync/corotypes.h>

#include "types.h"
#include "lock.h"
#include "protocol.h"

#define DRWLM_CPG_NAME "drwlm_lockspace"

int send_msg(cpg_handle_t handle, void *msg, size_t len)
{
    cs_error_t csrv = CS_OK;

    struct iovec vec = { msg, len };
    int vec_len = 1;

    if ((csrv = cpg_mcast_joined(handle, CPG_TYPE_FIFO, &vec, vec_len)) != CS_OK)
    {
        fprintf(stderr, "Failed to mcast joined: %s (%s)\n", cs_strerror(csrv), strerror(errno));
    }

    return csrv;
}

static int g_own_id = -1;
static int g_nodes_count = -1;

node_id_t own_id()
{
    assert(g_own_id != -1);
    return g_own_id;
}

size_t nodes_count()
{
    assert(g_nodes_count != -1);
    return g_nodes_count;
}

xa_id_t next_xid()
{
    static xa_id_t xid = 0;
    return xid++;
}

static lock_t lock = {
    .mode = RWLOCK_MODE_NL,
    .transitioning = false,
    .desired_mode = RWLOCK_MODE_NL,
    .pending_xid = 0,
    .granted_count = 0
};

void on_request(cpg_handle_t handle, request_t *req)
{
    printf("REQUEST(from=%u, xid=%u, mode=%s)\n",
            req->from,
            req->xid,
            rwlock_mode_to_str(req->mode));

    if (req->from == own_id())
    {
        lock.transitioning = true;
        lock.desired_mode = req->mode;
        lock.pending_xid = req->xid;
        lock.granted_count = 1;
    }
    else
    {
        response_t resp = {
            .header.type = RESPONSE,
            .to = req->from,
            .from = own_id(),
            .xid = req->xid,
            .reply = modes_compatible(lock.mode, req->mode) ? GRANTED : DENIED
        };

        if (send_msg(handle, &resp, sizeof(response_t)) != CS_OK)
            fprintf(stderr, "Failed to send response\n");
    }

    return;
}

void on_response(cpg_handle_t handle, response_t *resp)
{
    if (resp->to != own_id())
        return;

    printf("RESPONSE(to=%u, from=%u, xid=%u, reply=%s)\n",
            resp->to,
            resp->from,
            resp->xid,
            reply_to_str(resp->reply));

    if (resp->xid != lock.pending_xid)
        return;

    if (resp->reply == DENIED)
    {
        lock.transitioning = false;
        lock.pending_xid   = 0;
        lock.desired_mode  = RWLOCK_MODE_NL;

        return;
    }
    else if (resp->reply == GRANTED)
    {
        lock.granted_count++;

        if (lock.granted_count == nodes_count())
        {
            lock.mode = lock.desired_mode;
            lock.desired_mode = RWLOCK_MODE_NL;
            lock.pending_xid = 0;
            lock.transitioning = false;
            lock.granted_count = 0;
        }

        return;
    }
}

static void *dispatch_thread(void *arg)
{
    cpg_handle_t handle = *(cpg_handle_t *)arg;
	cpg_dispatch(handle, CS_DISPATCH_BLOCKING);
	return NULL;
}

typedef struct cpg_name    cpg_name_t;
typedef struct cpg_address cpg_address_t;
typedef struct cpg_ring_id cpg_ring_id_t;

void deliver_cb(cpg_handle_t      handle,
                const cpg_name_t *groupName,
                uint32_t          nodeid,
                uint32_t          pid,
                void             *msg,
                size_t            msg_len)
{
    switch (((message_header_t *)msg)->type)
    {
    case REQUEST:
        on_request(handle, msg);
        break;
    case RESPONSE:
        on_response(handle, msg);
        break;
    default:
        fprintf(stderr, "Unexpected message type '%d'", ((message_header_t *)msg)->type);
        break;
    }
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
    printf("confchg_cb(member_list_entries=%zu)\n", member_list_entries);
    g_nodes_count = member_list_entries;
}

static void totem_confchg_cb(cpg_handle_t    handle,
                             cpg_ring_id_t   ring_id,
                             uint32_t        member_list_entries,
                             const uint32_t *member_list)
{
    printf("totem_confchg_cb()\n");
}

static cpg_model_v1_data_t model_data = {
    .cpg_deliver_fn       = deliver_cb,
    .cpg_confchg_fn       = confchg_cb,
    .cpg_totem_confchg_fn = totem_confchg_cb,
    .flags                = CPG_MODEL_V1_DELIVER_INITIAL_TOTEM_CONF
};

int main(void)
{
    cs_error_t csrv = CS_OK;
    cpg_handle_t handle;

    csrv = cpg_model_initialize(&handle,
                                CPG_MODEL_V1,
                                (cpg_model_data_t *)&model_data,
                                NULL);
    if (csrv != CS_OK)
    {
        fprintf(stderr, "Failed to inititalize cpg_model: %s (%s)\n", cs_strerror(csrv), strerror(errno));
        return EXIT_FAILURE;
    }

    cpg_name_t cpg_name;
    cpg_name.length = strlen(DRWLM_CPG_NAME);
    strncpy(cpg_name.value, DRWLM_CPG_NAME, cpg_name.length);

    if ((csrv = cpg_join(handle, &cpg_name)) != CS_OK)
    {
        fprintf(stderr, "Failed to join cpg: %s (%s)\n", cs_strerror(csrv), strerror(errno));
        goto exit;
    }

    if ((csrv = cpg_local_get(handle, &g_own_id)) != CS_OK)
    {
        fprintf(stderr, "Failed to get local id: %s (%s)\n", cs_strerror(csrv), strerror(errno));
        goto exit;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, dispatch_thread, &handle);

    char buffer[256];
    while (fgets(buffer, 256, stdin) != NULL)
    {
        rwlock_mode_t mode = RWLOCK_MODE_NL;

        if (!strncmp(buffer, "NL", 2))
            mode = RWLOCK_MODE_NL;
        else if (!strncmp(buffer, "RD", 2))
            mode = RWLOCK_MODE_RD;
        else if (!strncmp(buffer, "IW", 2))
            mode = RWLOCK_MODE_IW;
        else if (!strncmp(buffer, "WR", 2))
            mode = RWLOCK_MODE_WR;
        else
        {
            fprintf(stderr, "Wrong input");
            continue;
        }

        request_t req = {
            .header.type = REQUEST,
            .from = own_id(),
            .mode = mode,
            .xid = next_xid()
        };

        if (send_msg(handle, &req, sizeof(request_t)) != CS_OK)
            fprintf(stderr, "Failed to send request\n");
    }

    pthread_join(thread, NULL);

exit:
    cpg_finalize(handle);

    return csrv == CS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
