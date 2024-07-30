#include "BlockList.h"

BlockList::BlockList(std::string filename) {
	this->m_filename = filename;
	this->m_siteCount = 0;
}

bool BlockList::Read() {
	std::ifstream file(this->m_filename);
	if (file.is_open()) {
		std::string temp;
		while (std::getline(file, temp, '\n')) {
			this->m_siteCount++;
			this->m_blockedSites.push_back(temp);
		}

		DebugPrint(DEBUG_TYPE::INFO_MSG, "Loaded sites: " + std::to_string(this->m_siteCount));

		if (this->m_siteCount <= 0) {
			return false;
		}

		return true;
	}
	else {
		return false;
	}

}
bool BlockList::IsBlacklisted(std::string site) {
	if (std::find(this->m_blockedSites.begin(), this->m_blockedSites.end(), site) != this->m_blockedSites.end()) {
		return true;
	}

	return false;
}