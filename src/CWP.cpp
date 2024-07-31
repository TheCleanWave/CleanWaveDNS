#include "CWP.h"

void ProtocolThread() {
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (sock < 0) {
		DebugPrint(DEBUG_TYPE::ERROR_MSG, "Error creating protocol TCP socket");
		exit(1);
	}

	sockaddr_in saIn = { };
	saIn.sin_port = htons(PROTOCOL_PORT);
	saIn.sin_family = AF_INET;
	inet_pton(AF_INET, LHOST, &saIn.sin_addr.s_addr);

	
}