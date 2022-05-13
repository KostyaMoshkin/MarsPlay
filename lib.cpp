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


	void unpackColor(lib::iPoint3D  vColor256_, float* vColorRGB_)
	{
		vColorRGB_[0] = 1.0f * (vColor256_.r & 0xff) / 255;
		vColorRGB_[1] = 1.0f * ((vColor256_.g >> 8) & 0xff) / 255;
		vColorRGB_[2] = 1.0f * (vColor256_.b >> 16) / 255;
	}

	//------------------------------------------------------------------------------------------



}