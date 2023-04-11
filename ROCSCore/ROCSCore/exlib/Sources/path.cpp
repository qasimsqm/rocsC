#include "exlib/path.h"

using namespace std;

namespace ex {
namespace path {

EXLIB_API std::string join(std::initializer_list<std::string> il)
{
    std::vector<std::string> result;
    for (auto it: il) {
        if (*it.rbegin() == PATH_SEP) {
            result.push_back(std::string(it.begin(), it.end() - 1));
        } else {
            result.push_back(it);
        }
    }

    return ex::join_string(result, std::string(1, PATH_SEP));
}

EXLIB_API string join(const string& path1, const string& path2)
{
    vector<string> v;
    v.push_back(path1); v.push_back(path2);
    return join(v.begin(), v.end());
}

EXLIB_API string basename(const string& filename)
{
    string::size_type pos = filename.find_last_of(PATH_SEP);
    if (pos == string::npos) {
        return filename;
    } else {
        return filename.substr(pos + 1);
    }
}

EXLIB_API string dirname(const string& filename)
{
    string::size_type pos = filename.find_last_of(PATH_SEP);
    if (pos == string::npos) {
        return "";
    } else {
        return filename.substr(0, pos);
    }
}

EXLIB_API pair<string, string> split(const string& filename)
{
    string::size_type pos = filename.find_last_of(PATH_SEP);
    if (pos == string::npos) {
        return make_pair(filename, "");
    } else {
        return make_pair(filename.substr(0, pos), filename.substr(pos + 1));
    }
}

EXLIB_API pair<string, string> splitext(const string& filename)
{
    string::size_type pos = filename.find_last_of('.');
    if (pos == string::npos) {
        return make_pair(filename, "");
    } else {
        return make_pair(filename.substr(0, pos), filename.substr(pos));
    }
}

EXLIB_API string basename_no_ext(const string& filename)
{
    return splitext(basename(filename)).first;
}

EXLIB_API string extension(const string& filename)
{
    return splitext(basename(filename)).second;
}


#ifndef _WIN32
std::map<int, std::string> MkdirErrorCodes(
    {
        { EACCES, "Search permission is denied on a component of the path prefix, or write permission is denied on the parent directory of the directory to be created." },
        { EEXIST, "The named file exists." },
        { ELOOP, "A loop exists in symbolic links encountered during resolution of the path argument." },
        { EMLINK, "The link count of the parent directory would exceed {LINK_MAX}." },
        { ENAMETOOLONG, "The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}." },
        { ENOENT, "A component of the path prefix specified by path does not name an existing directory or path is an empty string." },
        { ENOSPC, "The file system does not contain enough space to hold the contents of the new directory or to extend the parent directory of the new directory." },
        { ENOTDIR, "A component of the path prefix is not a directory." },
        { EROFS, "The parent directory resides on a read-only file system." }
});

EXLIB_API void makedirs(const std::string &pathname)
{
    auto subdirectories = ex::split_string(pathname, std::string(1, PATH_SEP));
    std::vector<std::string> pathsToCreate;
    size_t startingIndex; 
    
    if (subdirectories[0] == "")
    {
        // path is absolute, beginning with '/'
        pathsToCreate.push_back(std::string(1, PATH_SEP) + subdirectories.at(1));
        startingIndex = 2;
    } else
    {
        // relative path
        pathsToCreate.push_back(subdirectories[0]);
        startingIndex = 1;
    }
     
    if (subdirectories.size() > startingIndex)
    {
        for (size_t i = startingIndex; i < subdirectories.size(); i++)
        {
            pathsToCreate.push_back(
                ex::path::join(pathsToCreate[i-1], subdirectories[i]));
        }
    }
     
    for (auto subdir: pathsToCreate)
    {
        auto result = mkdir(subdir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
        if (result == -1)
        {
            if ( errno == EEXIST)
            {
                continue;
            } else
            {
                std::string errorMessage; 
                try
                {
                    errorMessage = MkdirErrorCodes.at(errno);
                } catch (std::out_of_range &)
                {
                    errorMessage = "Unknown error occurred.";
                }

                throw PathError(errorMessage);
            }
        }
    }
}

#endif // not defined _WIN32

}
}
