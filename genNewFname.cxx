/*
 * Sin-Yaw Wang <swang24@scu.edu>
 */
#include <string.h>
#include <sstream>
#include <sys/stat.h>
namespace csenbmp {

// generate filename in "x<n>.y" style
// x and y were whatever in existence, add a number before the dot
// in consecustive tries if already in existence
std::string genNewFname(const std::string oldfname) {
    struct stat st;
    std::string::size_type dotpos = oldfname.find(".");
    int n = 2;
    std::stringstream ss;
    std::string newfn=oldfname;
    while (stat(newfn.c_str(), &st) == 0) {
        ss.str(std::string("")); // clear the sstream content
        ss << n++;  // read in an integer, since it is a "string stream", becomes a string
        newfn = oldfname.substr(0, dotpos) + ss.str() + oldfname.substr(dotpos, oldfname.npos);
    }

    return newfn;
}
}
