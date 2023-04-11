#include "exlib/listdir.h"

using namespace std;

namespace ex {

#if defined(__APPLE__) || defined(__linux)
EXLIB_API vector<string> listdir(const string& path)
{
    vector<string> results;
    DIR *dir_ptr = opendir(path.c_str());
    if (!dir_ptr) throw BadDir(ex::errmsg(errno));
    dirent *entry = nullptr;
    while (true) {
        entry = readdir(dir_ptr);
        if (entry) {
            results.push_back(entry->d_name);
        } else {
            break;
        }
    }

    return results;
}
#elif defined(_WIN32)
namespace boostfs = boost::filesystem;

EXLIB_API vector<string> listdir(const string& path)
{
    vector<string> results;
    auto boost_path = boostfs::path(path);
    boostfs::directory_iterator end;

    try {
        boostfs::directory_iterator i(boost_path);
        while (i != end) {
            results.push_back(i->path().filename().string());
            i++;
        }
    } catch (const std::exception &ex) {
        throw BadDir(ex.what());
    }

    return results;
}
#else
#error No implementation of listdir.
#endif

} // namespace ex
