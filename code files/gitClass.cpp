#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

#include "commitNodeList.cpp"
using namespace std;

//---------------------
#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define END "\033[0m"
//---------------------

class gitClass
{
public:
    commitNodeList list;
    void gitInit();
    void gitAdd();
    void gitAdd(string files[], int arrSize);
    void gitCommit(string msg);
    void gitRevert(string commitHash);
    void gitLog();
    // void gitStatus();
};

void gitClass::gitInit()
{
    filesystem::create_directory(".git");
    filesystem::create_directory(".git/staging_area");
    filesystem::create_directory(".git/commits");
}

void gitClass::gitAdd()
{
    const auto copyOptions = filesystem::copy_options::update_existing | filesystem::copy_options::recursive;

    for (const auto &dirEntry : filesystem::directory_iterator(filesystem::current_path()))
    {
        auto fileName = filesystem::path(dirEntry).filename();
        if (fileName != "git" && fileName != ".git")
        {
            if (filesystem::is_directory(fileName))
            {
                filesystem::copy(dirEntry, filesystem::current_path() / ".git" / "staging_area" / fileName, copyOptions);
            }
            else
            {
                filesystem::copy(dirEntry, filesystem::current_path() / ".git" / "staging_area" / "", copyOptions);
            }
        }
    }
}

void gitClass::gitAdd(string files[], int arrSize)
{
    const auto copyOptions = filesystem::copy_options::update_existing | filesystem::copy_options::recursive;

    for (int i = 0; i < arrSize; i++)
    {
        if (filesystem::exists(files[i]))
        {
            int last_index = files[i].find_last_of('/');
            if (last_index != string::npos)
            {
                auto newDir = filesystem::current_path() / ".git" / "staging_area" / files[i].substr(0, last_index);
                filesystem::create_directories(newDir);
                filesystem::copy(filesystem::current_path() / files[i], filesystem::current_path() / ".git" / "staging_area" / files[i], copyOptions);
            }
            else
            {
                if (filesystem::is_directory(files[i]))
                {
                    filesystem::copy(filesystem::current_path() / files[i], filesystem::current_path() / ".git" / "staging_area" / files[i], copyOptions);
                }
                else
                {
                    filesystem::copy(filesystem::current_path() / files[i], filesystem::current_path() / ".git" / "staging_area", copyOptions);
                }
            }
        }
        else
        {
            cout << files[i] << RED "does not exist!" END << endl;
        }
    }
}

void gitClass::gitCommit(string msg)
{
    list.addOnTail(msg);
}

void gitClass::gitRevert(string commitHash)
{
    list.revertCommit(commitHash);
}

void gitClass::gitLog()
{
    list.printCommitList();
}

// void gitClass::gitStatus()
// {
//     list.printCommitStatus();
// }
