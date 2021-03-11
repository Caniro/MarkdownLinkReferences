#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <io.h>

using namespace std;

const int& filename_length = 4096;

//#define _MY_DEBUG

void    parseFile(const std::string& filename, std::ofstream& ofs)
{
    if (filename.size() < 3 || \
        filename[filename.size() - 3] != '.' || \
        filename[filename.size() - 2] != 'm' || \
        filename[filename.size() - 1] != 'd')
    {
#ifdef _MY_DEBUG
        std::cerr << "Not .md file\n";
#endif
        return;
    }

    std::ifstream ifs(filename);
    if (!ifs.is_open())
    {
        std::cerr << "Fail to open file\n";
        return;
    }

    // root directory's name (*VSCode : Workspace directory)
    const char* parent_dir = "StudyingNote";

    size_t start_relative_path = filename.find(parent_dir) + strlen(parent_dir);
    string relative_path = filename.substr(start_relative_path, \
        filename.size() - (start_relative_path));

    while (ifs)
    {
        std::string referenced;
        std::getline(ifs, referenced);

        unsigned int index = referenced.find("](");

        if (index != std::string::npos)
        {
            index += 2;
            if (referenced[index] != '/')
                return;
            referenced = referenced.substr(index, referenced.size() - index - 1);

            std::cout << "| [" << relative_path << "](" << relative_path << \
                ") | [" << referenced << "](" << referenced << ") | \n";
            ofs << "| [" << relative_path << "](" << relative_path << \
                ") | [" << referenced << "](" << referenced << ") | \n";
        }
    }

    ifs.close();
}

void    appendCstring(char* str, const std::string& to_append)
{
    string  tmp(str);
    tmp.append(to_append);
    strcpy_s(str, filename_length, tmp.c_str());
}

void    getPath(char* path)
{
    GetCurrentDirectoryA(filename_length, path);
#ifdef _MY_DEBUG
    cout << "Start path : " << path << endl;
    appendCstring(path, "\\StudyingNote");
#else
    appendCstring(path, "");
#endif
}

bool    isDirectory(struct _finddata_t fd)
{
    if (fd.attrib & _A_SUBDIR)
        return true;
    return false;
}

void    fileSearch(char* path, std::ofstream& ofs)
{
    struct _finddata_t fd;
    intptr_t handle;

    if (strlen(path) > filename_length - 3)
        return;
    appendCstring(path, "\\*");

    if ((handle = _findfirst(path, &fd)) == -1L)
    {
        cerr << "No file in directory!" << endl;
        return;
    }

    path[strlen(path) - 1] = '\0';

    do
    {
        if (fd.name[0] == '.')
            continue;
        if (isDirectory(fd))
        {
            char child_path[filename_length + 2];
            child_path[0] = '\0';
            appendCstring(child_path, string(path) + fd.name);
#ifdef _MY_DEBUG
            cout << "child_path : " << child_path << '\n';
#endif
            fileSearch(child_path, ofs);
        }
        else
        {
#ifdef _MY_DEBUG
            cout << "\tFile name : " << string(path) + fd.name << '\n';
#endif
            parseFile(string(path) + fd.name, ofs);
        }
    } while (_findnext(handle, &fd) == 0);
    _findclose(handle);
}

int     main()
{
    char path[filename_length];

    ofstream ofs("References.md");
    if (!ofs)
    {
        cerr << "Fail to open output file\n";
        return 1;
    }

    getPath(path);

    fileSearch(path, ofs);

    ofs.close();
}
