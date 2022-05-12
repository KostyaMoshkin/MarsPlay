#include <string>
#include <fstream>
#include <streambuf>

#include "ShaderLoader.h"

#include "resource.h"

#include <Windows.h>
#include <WinUser.h>

namespace shader {

	std::string ShaderLoader::get_ShaderBody(int nhaderResourceID_)
	{
		HMODULE handle = ::GetModuleHandleW(NULL);

		if (handle == nullptr)
			return std::string("");

		HRSRC rc = ::FindResource(handle, MAKEINTRESOURCEW(nhaderResourceID_), MAKEINTRESOURCEW(TEXTFILE));

		if (rc == nullptr)
			return std::string("");

		HGLOBAL rcData = ::LoadResource(handle, rc);

		if (rcData == nullptr)
			return std::string("");

		const char * cShaderBody = static_cast<const char*>(::LockResource(rcData));

		std::string sSahderBody(cShaderBody);

		return sSahderBody;
	}

}
