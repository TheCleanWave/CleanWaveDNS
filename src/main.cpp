#include <iostream>
#include <string>

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
#include "BlockList.h"

#define VERSION 0.1

#define PORT 53

bool g_bQuit = false;

char PRIMARY_DNS[] = "8.8.8.8";

BlockList* g_blockList = nullptr;

int main() {
	std::cout << "CleanWave. The most powerful DNS for privacy and ad blocking. Version: " << VERSION << std::endl;
	std::cout << "Made by Preciado" << std::endl << std::endl;

#ifdef WIN32
	WSADATA wsa;
	int wsaErr = WSAStartup(MAKEWORD(2, 2), &wsa);
	DebugPrint(DEBUG_TYPE::DEBUG_MSG, "Windows: Initializing WSA");
	if (wsaErr != 0) {
		DebugPrint(DEBUG_TYPE::FATAL_MSG, " Windows: Could not initialize WSADATA");
		return 1;
	}
#endif

	DebugPrint(DEBUG_TYPE::INFO_MSG, "Initializing socket");
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		DebugPrint(DEBUG_TYPE::FATAL_MSG, "Error creating the socket");
		return 1;
	}

	sockaddr_in saIn = { };
	saIn.sin_family = AF_INET;
	saIn.sin_port = htons(PORT);
	saIn.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock, (sockaddr*)&saIn, sizeof(saIn)) == -1) {
		DebugPrint(DEBUG_TYPE::FATAL_MSG, "Error binding to port");
		return 1;
	}
	DebugPrint(DEBUG_TYPE::DEBUG_MSG, "Server bound on port " + std::to_string(PORT));
	DebugPrint(DEBUG_TYPE::DEBUG_MSG, "Ready for entry connections...");

	g_blockList = new BlockList("blocklist");
	if (!g_blockList->Read()) {
		DebugPrint(DEBUG_TYPE::ERROR_MSG, "No blocklist or empty blocklist.");
		return 1;
	}

	while (!g_bQuit) {
		char data[1024];
		sockaddr_in sender = { };
		socklen_t senderSize = sizeof(sender);
		UINT received = recvfrom(sock, data, sizeof(data), 0, (sockaddr*)&sender, &senderSize);

		if (received < 12) {
			DebugPrint(DEBUG_TYPE::ERROR_MSG, "Invalid DNS packet length received. Skipping.");
			continue;
		}

		int questionStart = 12;

		/* Search for the QNAME end. */
		int i = questionStart;
		while (i < received && data[i] != 0) {
			i += data[i] + 1;
		}

		/* Check if the QNAME end is not bigger than our received data. */
		if (i > received) {
			DebugPrint(DEBUG_TYPE::ERROR_MSG, "Invalid DNS packet received.");
			continue;
		}

		/* Extract our QNAME */
		std::string qname;
		int len = data[questionStart];
		int offset = questionStart + 1;

		while (len > 0) {
			qname.append(&data[offset], len);
			offset += len;
			len = data[offset];
			if (len > 0) {
				qname.append(".");
			}
			offset++;
		}

		/* Extract QTYPE and QCLASS */
		UINT qtype = (data[offset] << 8) | data[offset + 1];
		UINT qclass = (data[offset + 2] << 8) | data[offset + 3];

		bool bIsBlackListed = g_blockList->IsBlacklisted(qname);
		
		/* Only proceed if QTYPE is an A RECORD */
		if (qtype == 1 && qclass == 1 && bIsBlackListed) {
			DebugPrint(DEBUG_TYPE::INFO_MSG, "Received query for an A RECORD for: " + qname);

			char response[512];

			/* Copy our query ID */
			response[0] = data[0];
			response[1] = data[1];

			/* Set our flags */
			response[2] = 0x81;
			response[3] = 0x80;

			/* Copy questions count */
			response[4] = data[4];
			response[5] = data[5];

			/* Set answers count */ 
			response[6] = 0x00;
			response[7] = 0x01;

			/* Set authoritative responses count */
			response[8] = 0x00;
			response[9] = 0x00;

			/* Set additional responses count */
			response[10] = 0x00;
			response[11] = 0x00;

			/* Copy our question */
			int questionLen = received - 12;
			memcpy(&response[12], &data[12], questionLen);
			int responseLen = 12 + questionLen;

			/* Pointer to our QNAME */
			response[responseLen++] = 0xc0;
			response[responseLen++] = 0x0c;

			/* Set our type (A Type record) */
			response[responseLen++] = 0x00;
			response[responseLen++] = 0x01;

			/* Set our class (IN [Internet])) */
			response[responseLen++] = 0x00;
			response[responseLen++] = 0x01;

			/* Set our TTL */
			response[responseLen++] = 0x00;
			response[responseLen++] = 0x00; 
			response[responseLen++] = 0x00;
			response[responseLen++] = 0x3c;

			/* Set our response size (4 bytes for IPv4) */
			response[responseLen++] = 0x00;
			response[responseLen++] = 0x04;

			/* DEBUG: Google ip */
			response[responseLen++] = 0x7F;
			response[responseLen++] = 0x00; 
			response[responseLen++] = 0x00;
			response[responseLen++] = 0x01;

			sendto(sock, response, responseLen, 0, (sockaddr*)&sender, sizeof(sockaddr_in));
		}
		else {
			DebugPrint(DEBUG_TYPE::INFO_MSG, "Received query for an A RECORD for: " + qname);

			int forwardSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			sockaddr_in forwardIn = { };
			forwardIn.sin_family = AF_INET;
			forwardIn.sin_port = htons(PORT);
			inet_pton(AF_INET, PRIMARY_DNS, &forwardIn.sin_addr.s_addr);

			if (sendto(forwardSock, data, received, 0, (sockaddr*)&forwardIn, sizeof(forwardIn)) == -1) {
				DebugPrint(DEBUG_TYPE::ERROR_MSG, "Error forwarding the query to the primary DNS");
				continue;
			}

			char forwardResponse[512];
			sockaddr_in resAddr;
			socklen_t resLength = sizeof(resAddr);
			int forwardLength = recvfrom(forwardSock, forwardResponse, sizeof(forwardResponse), 0, (sockaddr*)&resAddr, &resLength);

			if (forwardLength == -1) {
				std::cout << "[ERROR] Socket error received" << std::endl;
				DebugPrint(DEBUG_TYPE::ERROR_MSG, "Socket error received");

				continue;
			}

			sendto(sock, forwardResponse, forwardLength, 0, (sockaddr*)&sender, sizeof(sender));
		}
	}

	return 0;
}