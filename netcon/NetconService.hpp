/*
 * ZeroTier One - Network Virtualization Everywhere
 * Copyright (C) 2011-2015  ZeroTier, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * --
 *
 * ZeroTier may be used and distributed under the terms of the GPLv3, which
 * are available at: http://www.gnu.org/licenses/gpl-3.0.html
 *
 * If you would like to embed ZeroTier into a commercial application or
 * redistribute it in a modified binary form, please contact ZeroTier Networks
 * LLC. Start here: http://www.zerotier.com/
 */

#include <sys/poll.h>
#include <string>

#include "../osdep/Phy.hpp"
#include "NetconEthernetTap.hpp"

#include "Intercept.h"
#include "LWIPStack.hpp"

#ifndef _NETCON_SERVICE_HPP
#define _NETCON_SERVICE_HPP

using namespace std;

namespace ZeroTier {

  class NetconEthernetTap;

  // Helper class for passing reference to Phy to LWIP callbacks
  class Larg
  {
  public:
    NetconEthernetTap *tap;
    PhySocket *sock;
    Larg(NetconEthernetTap *_tap, PhySocket *_sock) : tap(_tap), sock(_sock) {}
  };

  enum NetconConnectionType { RPC, BUFFER };

  // prototypes
  class NetconClient;
  class NetconConnection;

  //
  class NetconConnection
  {
  public:
    int their_fd;
    unsigned char buf[DEFAULT_READ_BUFFER_SIZE];
    int idx;
    NetconConnectionType type;
    struct tcp_pcb *pcb;
    PhySocket *sock;
    NetconClient *owner;

    NetconConnection(NetconConnectionType type, PhySocket *sock) : type(type), sock(sock) {}
  };

  //
  class NetconClient
  {
  public:
    vector<NetconConnection*> connections;

    int tid;
    bool waiting_for_retval;
    NetconConnection *rpc;
    NetconConnection* unmapped_conn;

    NetconConnection *addConnection(NetconConnectionType type, PhySocket *sock)
    {
      NetconConnection *new_conn = new NetconConnection(type, sock);
      new_conn->owner = this;
      return new_conn;
    }

    // Check data and RPC connections
    NetconConnection *getConnection(PhySocket *sock)
    {
      if(sock && sock == rpc->sock) {
        return rpc;
      }
      for(size_t i=0; i<connections.size(); i++) {
        if(sock == connections[i]->sock) { return connections[i];}
      }
      return NULL;
    }

    //
    NetconConnection *getConnectionByTheirFD(int fd)
    {
      for(size_t i=0; i<connections.size(); i++) {
        if(connections[i]->their_fd == fd) { return connections[i]; }
      }
      return NULL;
    }

    //
    NetconConnection *getConnectionByPCB(struct tcp_pcb *pcb)
    {
      for(size_t i=0; i<connections.size(); i++) {
        if(connections[i]->pcb == pcb) { return connections[i]; }
      }
      return NULL;
    }

    NetconConnection *containsPCB(struct tcp_pcb *pcb)
    {
      for(size_t i=0; i<connections.size(); i++) {
        if(connections[i]->pcb == pcb) { return connections[i]; }
      }
      return NULL;
    }

    void closeConnection(NetconConnection *c)
    {
      // close all connections
      // -- pcb
      // -- PhySocket
    }

    void closeConnection(PhySocket *sock)
    {
      
    }

    void closeClient()
    {

    }
  };
} // namespace ZeroTier

#endif