#pragma once
#include <iostream>
#include <string>
#include <vector>
#ifdef WIN32
#include <Windows.h>
#else
typedef unsigned int UINT;
#endif
#include <fstream>
#include <algorithm>

#include "ConsoleUtils.h"

class BlockList {
private:
	std::string m_filename;
	std::vector<std::string> m_blockedSites;
	UINT m_siteCount;
public:
	BlockList(std::string filename);

	bool Read();
	bool IsBlacklisted(std::string site);
};