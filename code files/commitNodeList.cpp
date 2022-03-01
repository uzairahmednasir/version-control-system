#include <iostream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <ctime>
#include <vector>
using namespace std;

// ================  Global Functions  ==================
string gen_random(const int len)
{
    srand((unsigned)time(NULL) * getpid());
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
    {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

string get_time()
{
    time_t t = std::time(0); // get time now
    tm *now = std::localtime(&t);
    string dateTime = to_string(now->tm_year + 1900) + "/" +
                      to_string(now->tm_mon + 1) + "/" +
                      to_string(now->tm_mday) + " " +
                      to_string(now->tm_hour) + ":" +
                      to_string(now->tm_min) + "\n";

    return dateTime;
}
// =========================================================

class commitNode
{
private:
    string commitID;
    string commitMsg;
    string nextCommitID;
    commitNode *next;

    void createCommitNode()
    {
        // create commit dir, create commitInfo.txt, copy files
        filesystem::create_directory(filesystem::current_path() / ".git" / "commits" / commitID);
        auto path = filesystem::current_path() / ".git" / "commits" / commitID / "commitInfo.txt";
        ofstream write(path.string());
        write << "1." + commitID + "\n" +
                     "2." + commitMsg + "\n" +
                     "3." + get_time() + "\n";

        auto STAGING_AREA_PATH = filesystem::path(filesystem::current_path() / ".git" / "staging_area");
        const auto copyOptions = filesystem::copy_options::update_existing | filesystem::copy_options::recursive;
        filesystem::copy(STAGING_AREA_PATH, filesystem::current_path() / ".git" / "commits" / commitID / "Data", copyOptions);
    }

public:
    commitNode()
    {
        this->next = NULL;
    }
    commitNode(string commitID, string commitMsg)
    {
        this->commitID = commitID;
        this->commitMsg = commitMsg;
        this->next = NULL;
        createCommitNode();
    }
    commitNode(string commitId)
    {
        // msg in not populated.
        checkNextCommitId();
        this->commitID = commitId;
        next = NULL;
    }

    void revertCommitNode(string fromHash)
    {
        filesystem::create_directories(filesystem::current_path() / ".git" / "commits" / getCommitID() / "Data");
        auto nextCommitPath = filesystem::current_path() / ".git" / "commits" / getCommitID() / "commitInfo.txt";
        auto copyFrom = filesystem::current_path() / ".git" / "commits" / fromHash / "Data";
        ofstream write(nextCommitPath.string());
        write << "1." + commitID + "\n" +
                     "2." + commitMsg + "\n" +
                     "3." + get_time() + "\n";
        const auto copyOptions = filesystem::copy_options::recursive;
        // cout << "from: "  << copyFrom << " ---- " << "copy to: " << filesystem::current_path() / ".git" / "commits" / getCommitID() / "Data" << endl;
        filesystem::copy(copyFrom, filesystem::current_path() / ".git" / "commits" / getCommitID() / "Data", copyOptions);
    }

    void readNodeInfo()
    {
        string info;
        auto path = filesystem::current_path() / ".git" / "commits" / getCommitID() / "commitInfo.txt";
        ifstream file(path.string());
        while (getline(file, info))
        {
            if (info[0] == '1')
            {
                this->setCommitID(info.substr(2));
            }
            if (info[0] == '2')
            {
                this->setCommitMsg(info.substr(2));
            }
        }
    }

    string getCommitMsg()
    {
        return this->commitMsg;
    }
    void setCommitMsg(string commitMsg)
    {
        this->commitMsg = commitMsg;
    }

    void setCommitID(string commitID)
    {
        this->commitID = commitID;
    }
    string getCommitID()
    {
        return this->commitID;
    }

    void setNext(commitNode *node)
    {
        this->next = node;
    }
    commitNode *getNext()
    {
        return next;
    }

    void setNextCommitID(string nextCommitId)
    {
        this->nextCommitID = nextCommitId;
    }
    void writeNextCommitID(string nextCommitID)
    {
        setNextCommitID(nextCommitID);
        auto path = filesystem::current_path() / ".git" / "commits" / getCommitID() / "nextCommitInfo.txt";
        ofstream write(path.string());
        write << nextCommitID;
    }
    string checkNextCommitId()
    {
        filesystem::path tempPath(filesystem::current_path() / ".git" / "commits" / getCommitID() / "nextCommitInfo.txt");
        bool exists = filesystem::exists(tempPath);
        if (exists)
        {
            string info;
            ifstream file(tempPath);
            while (getline(file, info))
            {
                this->nextCommitID = info;
                break;
            }
            file.close();
            return nextCommitID;
        }
        return "";
    }
    string getNextCommitId()
    {
        return this->nextCommitID;
    }
};

class commitNodeList
{
private:
    commitNode *HEAD;
    commitNode *TAIL;

    bool checkHead()
    {
        // check if HEAD commit exists
        auto tempDir = filesystem::current_path() / ".git" / "commits" / "0x1111";
        return filesystem::exists(tempDir);
    }

public:
    commitNodeList()
    {
        setHead(NULL);
        setTail(NULL);
        if (checkHead())
        {
            setHead(new commitNode("0x1111"));
        }
    }

    commitNode *getHead()
    {
        return this->HEAD;
    }
    void setHead(commitNode *HEAD)
    {
        this->HEAD = HEAD;
    }

    commitNode *getTail()
    {
        return this->TAIL;
    }
    void setTail(commitNode *TAIL)
    {
        this->TAIL = TAIL;
    }

    void addOnTail(string msg)
    {
        if (!checkHead())
        {
            commitNode *newNode = new commitNode("0x1111", msg);
            setHead(newNode);
        }
        else
        {
            string commitID = gen_random(8);
            commitNode *currNode = getHead();
            while (currNode != NULL)
            {
                string nextCommitId = currNode->checkNextCommitId();

                if (nextCommitId.length() < 8)
                {
                    commitNode *newNode = new commitNode(commitID, msg);
                    currNode->writeNextCommitID(commitID);
                    currNode = NULL;
                }
                else
                {
                    commitNode *newNode = new commitNode();
                    newNode->setCommitID(nextCommitId);
                    currNode = newNode;
                }
            }
        }
    }

    void revertCommit(string commitHash)
    {
        commitNode *commitNodeToRevert;
        if (!checkHead())
        {
            cout << "Nothing to Revert to " << endl;
        }
        else
        {
            bool error = true;
            string commitID = gen_random(8);
            commitNode *currNode = getHead();
            while (currNode != NULL)
            {
                string nextCommitId = currNode->checkNextCommitId();
                if (commitHash == currNode->getCommitID())
                {
                    commitNodeToRevert = currNode;
                    error = false;
                }

                if (nextCommitId.length() < 8)
                {
                    if (!error)
                    {
                        commitNodeToRevert->readNodeInfo();
                        commitNode *newNode = new commitNode();
                        newNode->setCommitID(commitID);
                        newNode->setCommitMsg(commitNodeToRevert->getCommitMsg());
                        newNode->revertCommitNode(commitNodeToRevert->getCommitID());

                        currNode->writeNextCommitID(commitID);
                    }
                    currNode = NULL;
                }
                else
                {
                    commitNode *newNode = new commitNode();
                    newNode->setCommitID(nextCommitId);
                    currNode = newNode;
                }
            }

            if (error == true)
            {
                cout << "does not match any Commit Hash" << endl;
            }
        }
    }

    void printCommitList()
    {
        commitNode *currNode = getHead();
        while (currNode != NULL)
        {
            string nextCommitId = currNode->checkNextCommitId();
            filesystem::path commitPath(filesystem::current_path() / ".git" / "commits" / currNode->getCommitID() / "commitInfo.txt");
            string info;
            ifstream file(commitPath.string());
            while (getline(file, info))
            {
                if (info[0] == '1')
                {
                    cout << "Commit ID:    " << info.substr(2) << endl;
                }
                if (info[0] == '2')
                {
                    cout << "Commit Msg:   " << info.substr(2) << endl;
                }
                if (info[0] == '3')
                {
                    cout << "Date & Time:  " << info.substr(2) << endl;
                }
            }
            file.close();
            cout << "============================\n\n";

            if (nextCommitId.length() < 8)
            {
                currNode = NULL;
            }
            else
            {
                commitNode *newNode = new commitNode();
                newNode->setCommitID(nextCommitId);
                currNode = newNode;
            }
        }
    }

    // void printCommitStatus()
    // {
    //     // vector<string> staging_area_files;
    //     // for (const filesystem::path &file : filesystem::recursive_directory_iterator(filesystem::current_path() / ".git" / "staging_area"))
    //     // {
    //     //     staging_area_files.push_back(file.string().substr( file.string().find_last_of('/') + 1 ));
    //     // }
    //     // vector<string> latest_commit_files;
    //     // for (const filesystem::path &file : filesystem::recursive_directory_iterator(filesystem::current_path() / ".git" / getTail()))
    //     // {
    //     //     latest_commit_files.push_back(file.string().substr( file.string().find_last_of('/') + 1 ));
    //     // }

    //     // if (staging_area_files.size() == latest_commit_files.size())
    //     // {
    //     //     cout << "the branch is up-to-date";
    //     // }
    //     // else
    //     // {
    //     //     cout << "untracked files: \n";
    //     //     for (auto i = v.begin(); i != v.end(); i++)
    //     //     {
    //     //     }
    //     // }
    // }
};
