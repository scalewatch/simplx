/**
 * @author Valerian Vives <valerian.vives@scalewatch.com>
 * @file tcpconnector.hpp
 * @brief tcp connector top-level header
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#pragma once

#include "connector/tcp/client/client.hpp"
#include "connector/tcp/common/networkcalls.hpp"
#include "connector/tcp/common/network.hpp"
#include "connector/tcp/server/server.hpp"
#include "simplx.h"

using simplx::Engine;
using simplx::connector::tcp::TcpClient;
using simplx::connector::tcp::NetworkCalls;
using simplx::connector::tcp::Network;
using simplx::connector::tcp::TcpServer;

using fd_t = int64_t;
