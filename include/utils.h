#include <iomanip>
#include <sys/stat.h>
#include <ctime>
#include <string>
#if defined(_WIN32)
#include <direct.h>
#endif

using namespace std;

#define ISTR( x ) static_cast< std::ostringstream &> ( \
( ostringstream() <<  setw(2) << setfill('0') << x)).str() 

string parseDate(struct tm *now);

string parseDateTime(struct tm *now);

bool isDirExist(const string& path);

bool makePath(const string& path);