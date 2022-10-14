#pragma once

#include "Platform.hpp"

#ifdef __WINDOWS__

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
// Exclude rarely-used stuff from Windows headers Windows Header Files:
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

// Link to winsock2 libraries
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define SocketLastError errno

#endif

#if defined(__LINUX__) || defined(__MACOS__)

#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define SOCKET int
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
#define SD_RECEIVE              0x00
#define SD_SEND                 0x01
#define SD_BOTH                 0x02
#define SocketLastError         errno

#endif
