#include "lib.h"

namespace lib
{
	std::vector<std::string> create_file_list(const char* sPath_)
	{
		std::vector<std::string> vsFileList;

		for (const auto& folder : std::filesystem::directory_iterator(sPath_))
			if (folder.is_directory())
				for (const auto& pedrFile : std::filesystem::directory_iterator(folder))
					vsFileList.push_back(folder.path().string() + "\\" + pedrFile.path().filename().string());
			else
				vsFileList.push_back(folder.path().string());

		return vsFileList;
	}

}