#include "HeaderCollection.h"
#include "Path.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

bool Path::ExistFile(string InPath)
{
	return ExistFile(String::ToWString(InPath));
}

bool Path::ExistFile(wstring InPath)
{
	DWORD fileValue = GetFileAttributes(InPath.c_str());

	return fileValue < 0xFFFFFFFF;
}

bool Path::ExistDirectory(string InPath)
{
	return ExistDirectory(String::ToWString(InPath));
}

bool Path::ExistDirectory(wstring InPath)
{
	DWORD attribute = GetFileAttributes(InPath.c_str());

	BOOL temp = (attribute != INVALID_FILE_ATTRIBUTES &&
		(attribute & FILE_ATTRIBUTE_DIRECTORY));

	return temp == TRUE;
}

string Path::Combine(string InPath1, string InPath2)
{
	return InPath1 + InPath2;
}

wstring Path::Combine(wstring InPath1, wstring InPath2)
{
	return InPath1 + InPath2;
}

string Path::Combine(vector<string> InPaths)
{
	string temp = "";
	for (string name : InPaths)
		temp += name;

	return temp;
}

wstring Path::Combine(vector<wstring> InPaths)
{
	wstring temp = L"";
	for (wstring name : InPaths)
		temp += name;

	return temp;
}

string Path::GetDirectoryName(string InPath)
{
	String::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(0, index + 1);
}

wstring Path::GetDirectoryName(wstring InPath)
{
	String::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(0, index + 1);
}

string Path::GetExtension(string InPath)
{
	String::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('.');

	return InPath.substr(index + 1, InPath.length());;
}

wstring Path::GetExtension(wstring InPath)
{
	String::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('.');

	return InPath.substr(index + 1, InPath.length());;
}

string Path::GetFileName(string InPath)
{
	String::Replace(&InPath, "\\", "/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(index + 1, InPath.length());
}

wstring Path::GetFileName(wstring InPath)
{
	String::Replace(&InPath, L"\\", L"/");
	size_t index = InPath.find_last_of('/');

	return InPath.substr(index + 1, InPath.length());
}

string Path::GetFileNameWithoutExtension(string InPath)
{
	string fileName = GetFileName(InPath);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

wstring Path::GetFileNameWithoutExtension(wstring InPath)
{
	wstring fileName = GetFileName(InPath);

	size_t index = fileName.find_last_of('.');
	return fileName.substr(0, index);
}

bool Path::IsRelativePath(string InPath)
{
	return IsRelativePath(String::ToWString(InPath));
}

bool Path::IsRelativePath(wstring InPath)
{
	BOOL b = PathIsRelative(InPath.c_str());

	return b >= TRUE;
}

void Path::CreateFolder(string path)
{
	CreateFolder(String::ToWString(path));
}

void Path::CreateFolder(wstring path)
{
	if (ExistDirectory(path) == false)
		CreateDirectory(path.c_str(), NULL);
}

void Path::CreateFolders(string path)
{
	CreateFolders(String::ToWString(path));
}

void Path::CreateFolders(wstring path)
{
	String::Replace(&path, L"\\", L"/");

	vector<wstring> folders;
	String::SplitString(&folders, path, L"/");

	wstring temp = L"";
	for (wstring folder : folders)
	{
		temp += folder + L"/";

		CreateFolder(temp);
	}
}
