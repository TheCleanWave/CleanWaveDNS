#pragma once
#include <iostream>
#include "Colors.h"

enum DEBUG_TYPE {
	INFO_MSG,
	DEBUG_MSG,
	ERROR_MSG,
	FATAL_MSG
};

inline void DebugPrint(DEBUG_TYPE type, std::string msg) {
	std::string printedMsg;

	switch (type) {
	case DEBUG_TYPE::INFO_MSG:
		printedMsg.append(GREEN + std::string("[INFO] ") + RESET + WHITE + msg + RESET);
		break;
	case DEBUG_TYPE::DEBUG_MSG:
		printedMsg.append(YELLOW + std::string("[DEBUG] ") + RESET + WHITE + msg + RESET);
		break;
	case DEBUG_TYPE::FATAL_MSG:
		printedMsg.append(BGRED + std::string("[FATAL] ") + RESET + WHITE + msg + RESET);
		break;
	case DEBUG_TYPE::ERROR_MSG:
		printedMsg.append(RED + std::string("[ERROR] ") + RESET + WHITE + msg + RESET);
		break;
	default:
		break;
	}

	std::cout << printedMsg << std::endl;
}