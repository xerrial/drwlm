//
// DRWLM: Distributed Read-Write Lock Manager
// Copyright (c) 2025-2026, The DRWLM contributors
//
// SPDX-License-Identifier: BSD-3-Clause
//
#ifndef DRWLM_COMMON_DEFS_H
#define DRWLM_COMMON_DEFS_H

#pragma once

const char *pidfile_path = "/var/run/drwlmd.pid";
const char *socket_path  = "/var/run/drwlmd.sock";

const char *lockspace_name = "drwlm_lockspace";

#endif // !DRWLM_COMMON_DEFS_H
