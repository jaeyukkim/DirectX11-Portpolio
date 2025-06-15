#pragma once

class Path
{
public:
    static bool ExistFile(string InPath);
    static bool ExistFile(wstring InPath);

    static bool ExistDirectory(string InPath);
    static bool ExistDirectory(wstring InPath);

    static string Combine(string InPath1, string InPath2);
    static wstring Combine(wstring InPath1, wstring InPath2);

    static string Combine(vector<string> InPaths);
    static wstring Combine(vector<wstring> InPaths);

    static string GetDirectoryName(string InPath);
    static wstring GetDirectoryName(wstring InPath);

    static string GetExtension(string InPath);
    static wstring GetExtension(wstring InPath);

    static string GetFileName(string InPath);
    static wstring GetFileName(wstring InPath);

    static string GetFileNameWithoutExtension(string InPath);
    static wstring GetFileNameWithoutExtension(wstring InPath);

    static bool IsRelativePath(string InPath);
    static bool IsRelativePath(wstring InPath);

    static void CreateFolder(string path);
    static void CreateFolder(wstring path);

    static void CreateFolders(string path);
    static void CreateFolders(wstring path);

    
};