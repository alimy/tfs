/*
 * (C) 2007-2010 Alibaba Group Holding Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: compare_crc.h 719 2011-08-22 02:09:46Z chuyu@taobao.com $
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *      - initial release
 *
 */
#ifndef TFS_TOOLS_COMPARE_CRC_H_
#define TFS_TOOLS_COMPARE_CRC_H_

#include <string>
#include <vector>
#include <libgen.h>
#include "common/internal.h"
#include "common/statistics.h"
#include "common/directory_op.h"
#include "new_client/tfs_session.h"
#include "new_client/tfs_file.h"
#include "new_client/fsname.h"
#include "common/client_manager.h"
#include "common/new_client.h"

struct StatStruct
{
  int64_t total_count_;
  int64_t succ_count_;
  int64_t fail_count_;
  int64_t error_count_;
  int64_t unsync_count_;
  int64_t skip_count_;
};

struct log_file
{
  FILE** fp_;
  const char* file_;
};

int get_crc_from_tfsname_list(tfs::client::TfsClient& old_tfs_client, tfs::client::TfsClient& new_tfs_client,
    const char* filename_list, std::string& modify_time);
int get_crc_from_blockid(tfs::client::TfsClient& old_tfs_client, tfs::client::TfsClient& new_tfs_client,
    const char* block_list, std::string& modify_time);

static StatStruct cmp_stat_;

#endif