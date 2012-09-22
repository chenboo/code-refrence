#ifndef __LIBNET_H__
#define __LIBNET_H__

#ifdef LIBNET_EXPORTS
#define LIBNET_API __declspec(dllexport)
#else
#define LIBNET_API __declspec(dllimport)
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#include "TaskArch.h"
#include "URLParser.h"
#include "SocketUtils.h"
#include "HostNameResolver.h"
#include "IpAddress.h"
#include "NetworkEvent.h"
#include "UDPSocket.h"
#include "ClientSocket.h"
#include "ReliableMulticast.h"

#endif //__LIBNET_H__