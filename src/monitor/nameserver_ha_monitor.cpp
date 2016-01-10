/*
 * (C) 2007-2010 Alibaba Group Holding Limited.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: nameserver_ha_monitor.cpp 719 2011-08-22 02:09:46Z chuyu@taobao.com $
 *
 * Authors:
 *   duolong <duolong@taobao.com>
 *      - initial release
 *
 */
#include <stdio.h>
#include <Memory.hpp>

#include "common/new_client.h"
#include "common/client_manager.h"
#include "common/base_packet_streamer.h"
#include "common/base_packet_factory.h"
#include "message/heart_message.h"
#include "message/message_factory.h"
#include "nameserver/ns_define.h"

using namespace tfs::message;
using namespace tfs::common;
using namespace tfs::nameserver;
namespace tfs
{
  namespace monitor
  {
    static NsStatus get_name_server_running_status(const uint64_t ip_port, const int32_t switch_flag)
    {
      const int64_t TIMEOUT = 500;
      NsStatus status = NS_STATUS_NONE;
      NewClient* client = NewClientManager::get_instance().create_client();
      if (NULL != client)
      {
        HeartBeatAndNSHeartMessage heart_msg;
        heart_msg.set_ns_switch_flag_and_status(switch_flag, 0);
        tbnet::Packet* rsp_msg = NULL;
        if (TFS_SUCCESS == send_msg_to_server(ip_port, client, &heart_msg, rsp_msg, TIMEOUT))
        {
          if (HEARTBEAT_AND_NS_HEART_MESSAGE == rsp_msg->getPCode())
          {
            status = static_cast<NsStatus> (dynamic_cast<HeartBeatAndNSHeartMessage*> (rsp_msg)->get_ns_status());
          }
          else
          {
            TBSYS_LOG(ERROR, "unknow packet pcode %d", rsp_msg->getPCode());
          }
        }
        else
        {
          TBSYS_LOG(ERROR, "send packet error");
        }
        NewClientManager::get_instance().destroy_client(client);
      }
      return status;
    }
  }
}

using namespace tfs::monitor;

static void usage(const char* bin)
{
  fprintf(stderr, "usage: %s -i ipaddr -p port [-s swith flag on=1, off= 0] [-n]\n", bin);
}

int main(int argc, char *argv[])
{
  std::string ip;
  int32_t port = 0;
  int32_t i = 0;
  int32_t switch_flag = 0;
  bool set_log_level = false;
  while ((i = getopt(argc, argv, "ni:p:s:")) != EOF)
  {
    switch (i)
    {
    case 'i':
      ip = optarg;
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 'n':
      set_log_level = true;
      break;
    case 's':
      switch_flag = atoi(optarg);
      break;
    default:
      usage(argv[0]);
      return TFS_ERROR;
    }
  }

  if (ip.empty() || ip == " ")
  {
    usage(argv[0]);
    return TFS_ERROR;
  }
  if (port <= 0 || port > 0xffff)
  {
    usage(argv[0]);
    return TFS_ERROR;
  }
  if (set_log_level)
  {
    TBSYS_LOGGER.setLogLevel("ERROR");
  }
  static MessageFactory packet_factory;
  static BasePacketStreamer packet_streamer;
  packet_streamer.set_packet_factory(&packet_factory);
  NewClientManager::get_instance().initialize(&packet_factory, &packet_streamer);

  uint64_t hostid = tbsys::CNetUtil::strToAddr(const_cast<char*> (ip.c_str()), port);
  int32_t iret  = NS_STATUS_NONE;
  NsStatus status = get_name_server_running_status(hostid, switch_flag);
  if ((status > NS_STATUS_UNINITIALIZE) && (status <= NS_STATUS_INITIALIZED))
  {
    iret = 0;
  }
  else
  {
    TBSYS_LOG(ERROR, "ping nameserver failed, get status(none)");
    iret = status;
  }
  return iret;
}
