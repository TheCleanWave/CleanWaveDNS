#include "CWP.h"

void ProtocolThread() {
	DebugPrint(DEBUG_TYPE::INFO_MSG, "Started CWP protocol worker thread.");

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (sock < 0) {
		DebugPrint(DEBUG_TYPE::ERROR_MSG, "Error creating protocol TCP socket");
		exit(1);
	}

	sockaddr_in saIn = { };
	saIn.sin_port = htons(PROTOCOL_PORT);
	saIn.sin_family = AF_INET;
	inet_pton(AF_INET, LHOST, &saIn.sin_addr.s_addr);

	if (bind(sock, (const struct sockaddr*)&saIn, sizeof(saIn)) < 0) {
		DebugPrint(DEBUG_TYPE::ERROR_MSG, std::string("Error binding protocol TCP socket on port: ") + std::to_string(PROTOCOL_PORT));
		exit(1);
	}

	if (listen(sock, MAX_CONN) < 0) {
		DebugPrint(DEBUG_TYPE::ERROR_MSG, "Error starting TCP listened");
		exit(1);
	}

	DebugPrint(DEBUG_TYPE::INFO_MSG, std::string("WCP Protocol listening on port: ") + std::to_string(PROTOCOL_PORT));

	while (true) {
		sockaddr_in clientAddr = { };
		socklen_t clientSize = sizeof(sockaddr_in);
		int client = accept(sock, (sockaddr*)&clientAddr, &clientSize);

		char clientIp[INET6_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientIp, sizeof(clientIp));
		UINT clientPort = ntohs(clientAddr.sin_port);

		DebugPrint(DEBUG_TYPE::INFO_MSG, std::string("(CWP) New connection from: ") + std::string(clientIp) + ':' + std::to_string(clientPort));
	}
}