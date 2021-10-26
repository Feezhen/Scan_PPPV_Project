#include "utils.h"

using namespace std;

string int2str(int val)
{
    ostringstream out;
    out << val;
    return out.str();
}

int str2int(const string& val)
{
    istringstream in(val.c_str());
    int ret = 0;
    in >> ret;
    return ret;
}

string getCurrentTime()
{
    time_t t = time(NULL);
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y-%m-%d-%H-%M", localtime(&t));
    return ch;
}
