//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2026, Dmitry Kuzmenko <dimass1129@gmail.com>
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_DAEMON_COROSYNC_H
#define DRWLM_DAEMON_COROSYNC_H

#pragma once

#include <corosync/cpg.h>
#include <corosync/corotypes.h>
#include <corosync/corodefs.h>

typedef struct
{
    /**
     * @brief Corosync CPG name.
     */
    const char *cpg_name;

    /**
     * @brief Corosync CPG handle.
     */
    cpg_handle_t cpg;

    /**
     * @brief Corosync own node id.
     *
     * @todo Define type for node id.
     */
    unsigned own_id;
} corosync_context_t;

corosync_context_t *corosync_init(const char *group_name);
void corosync_deinit(corosync_context_t *context);


#endif // !DRWLM_DAEMON_COROSYNC_H
