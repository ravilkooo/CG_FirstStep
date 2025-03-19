#include "StringHelper.h"

std::wstring StringHelper::StringToWide(std::string str)
{

    std::wstring wide_string(str.begin(), str.end());
    return wide_string;
}
