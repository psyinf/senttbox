#pragma once


namespace common {

constexpr bool isDebug()
{
    bool is_debug = false;
#ifdef _DEBUG
    is_debug = true;
#endif
    return is_debug;
}

} //namespace common