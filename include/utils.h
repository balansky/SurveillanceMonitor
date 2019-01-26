#include <iomanip>
#include <sys/stat.h>
#include <ctime>
#include <string>
#include <cstdio>
#include <algorithm>
#if defined(_WIN32)
#include <direct.h>
#endif

using namespace std;

#define ISTR( x ) static_cast< ostringstream &> ( \
( ostringstream() <<  setw(2) << setfill('0') << x)).str() 


#define SSTR( x ) static_cast< ostringstream & >( \
( ostringstream() << std::dec << x ) ).str()


int parseDate(struct tm *now, char *buffer, const char *format="%d-%02d-%02d");

int parseDateTime(struct tm *now, char *buffer, const char *format="%02d:%02d:%02d");

bool isDirExist(const string& path);

bool makePath(const string& path);