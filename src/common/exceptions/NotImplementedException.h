#pragma once

#include <exceptions>


class NotImplementedException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};
