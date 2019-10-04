/**
 * @file platform_linux.h
 * @brief linux-specific OS wrapper
 * @copyright 2019 Scalewatch (www.scalewatch.com). All rights reserved.
 * Please see accompanying LICENSE file for licensing terms.
 */

#pragma once

#include <bitset>
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <pthread.h>
#include <cstdint>
#include <sys/time.h>
#include <unistd.h>

namespace simplx
{
// import into namespace
using std::vector;
using std::string;

string systemErrorToString(int);

int	Soft_stoi(const string &s, const int def = -1);

string  GetHostnameIP(const string &name);

vector<string> exec_command(const string cmd_s, const bool strip_whitespace_f = true);

string currentISOTimeUTC(const string &fmt_s = "%Y-%m-%d %H:%M:%S");
string currentISOTimeLocal(const string &fmt_s = "%Y-%m-%d %H:%M:%S");

    

} // namespace simplx

// nada mas
