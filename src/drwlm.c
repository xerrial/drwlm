#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include <pthread.h>

#include <corosync/cpg.h>
#include <corosync/corotypes.h>

#define DRWLM_CPG_NAME "drwlm_cpg"

typedef enum
{
    NL = 0,
    RD,
    IW,
    WR,
    RWLOCK_MODES_NUM
} rwlock_mode_t;

bool modes_compatible(rwlock_mode_t current, rwlock_mode_t desired)
{
    // TODO: можно небольшими изменениями в таблице реализовать
    // Read-preferring или Write-preferring политики.

    // Текущая таблица - Write-preferring.
    static const bool compatibility_table[RWLOCK_MODES_NUM][RWLOCK_MODES_NUM] =
    {
        //  Desired:
        //  NL     RD     IW     WR       Current:
        { true,  true,  true,  true }, // NL
        { true,  true,  true, false }, // RD
        { true, false,  true,  true }, // IW
        { true, false,  true, false }  // WR
    };

    return compatibility_table[current][desired];
}

typedef unsigned char node_id_t;
typedef unsigned int  xa_id_t;

typedef enum {
    GRANTED = 0,
    DENIED
} reply_t;

typedef struct
{
    node_id_t     from;
    xa_id_t       xid;
    rwlock_mode_t mode;
} request_t;

typedef struct
{
    node_id_t to;
    node_id_t from;
    xa_id_t   xid;
    reply_t   reply;
} response_t;

typedef struct
{
    rwlock_mode_t mode;
    bool          transitioning;
    rwlock_mode_t desired_mode;
    xa_id_t       pending_xid;
    unsigned      granted_count;
} lock_t;

void on_request(request_t *req)
{
    return;
}

void on_response(response_t *resp)
{
    return;
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

void deliver_cb (cpg_handle_t      handle,
                 const cpg_name_t *groupName,
                 uint32_t          nodeid,
                 uint32_t          pid,
                 void             *msg,
                 size_t            msg_len)
{
    printf("deliver_cb(): message (len=%zu) "
           "group '%.*s' "
           "from %" PRIu32 "/%" PRIu32 " "
           ": '%.*s'\n",
           msg_len,
           groupName->length, groupName->value,
           nodeid, pid,
           msg_len, (const char *)msg);
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
    printf("confchg_cb()\n");
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
        fprintf(stderr, "Failed to inititalize cpg_model\n");
        return EXIT_FAILURE;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, dispatch_thread, &handle);

    cpg_name_t cpg_name;
    cpg_name.length = strlen(DRWLM_CPG_NAME);
    strncpy(cpg_name.value, DRWLM_CPG_NAME, cpg_name.length);

    if ((csrv = cpg_join(handle, &cpg_name)) != CS_OK)
    {
        fprintf(stderr, "Failed to join cpg\n");
        goto exit;
    }

exit:
    cpg_finalize(handle);

    return csrv == CS_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
