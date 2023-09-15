#ifndef _CGE_UTILS_
#define _CGE_UTILS_

#include <unistd.h>
#include <string>

namespace Utils
{

static std::string getProgramPath()
{
    char buffer[1000];
#ifdef WIN32	
    GetModuleFileName(NULL, buffer, 1000);
#elif defined(__APPLE__)
    uint32_t bufferSize = sizeof(buffer);
    _NSGetExecutablePath(buffer, &bufferSize);
#else
    char szTmp[32];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    int bytes = std::min((int)readlink(szTmp, buffer, 1000), 999);
    buffer[bytes] = '\0';
#endif
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

}

#endif