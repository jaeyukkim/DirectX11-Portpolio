#pragma once

#include <filesystem>
#include <json/json.h>

#include <fstream>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <directxtk/WICTextureLoader.h>
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")
#include <Assimp/Importer.hpp>
#include <Assimp/PostProcess.h>
#include <Assimp/Scene.h>
#include <locale>
#include <codecvt>



#include "Libraries/FileSystem/BinaryFile.h"
#include "FileSystem/String.h"
#include "FileSystem/Path.h"
#include "FileSystem/Converter.h"
