#include "utils.h"

int parseDate(struct tm *now, char *buffer, const char *format){
    int year = now->tm_year + 1900;
    int month = now->tm_mon + 1;
    int day = now->tm_mday;

    int n = sprintf(buffer, format, year, month, day);
    return n;

    // string ll(buffer);
    // string year = to_string(now->tm_year + 1900);
    // string month = ISTR(now->tm_mon + 1);
    // string day = ISTR(now->tm_mday);
    // return year + month + day;

}

int parseDateTime(struct tm *now, char *buffer, const char *format){
    int hour = now->tm_hour;
    int minute = now->tm_min;
    int sec = now->tm_sec;

    int n = sprintf(buffer, format, hour, minute, sec);
    return n;
//     string hour = ISTR(now->tm_hour);
//     string minute = ISTR(now->tm_min);
//     string sec = ISTR(now->tm_sec);
//     return hour + "_" + minute + "_" + sec;
}

bool isDirExist(const string& path)
{
#if defined(_WIN32)
    struct _stat info;
    if (_stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & _S_IFDIR) != 0;
#else 
    struct stat info;
    if (stat(path.c_str(), &info) != 0)
    {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
#endif
}


bool makePath(const string& path)
{
#if defined(_WIN32)
    int ret = _mkdir(path.c_str());
#else
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
#endif
    if (ret == 0)
        return true;

    switch (errno)
    {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            int pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
                return false;
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
#if defined(_WIN32)
        return 0 == _mkdir(path.c_str());
#else 
        return 0 == mkdir(path.c_str(), mode);
#endif

    case EEXIST:
        // done!
        return isDirExist(path);

    default:
        return false;
    }
}