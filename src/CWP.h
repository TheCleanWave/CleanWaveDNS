#pragma once
#ifdef WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#else
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
typedef unsigned int UINT;
#endif

#include "ConsoleUtils.h"

#define LHOST "127.0.0.1"
#define PROTOCOL_PORT 18450

void ProtocolThread();