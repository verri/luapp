#ifndef LUAPP_TYPE_HPP_INCLUDED
#define LUAPP_TYPE_HPP_INCLUDED

extern "C"
{
#include <lua.h>
}

#include <string>

namespace lua
{

struct nil {};

using boolean = int;
using number = lua_Number;
using integer = lua_Integer;
using string = std::string;

class function
{

};

class userdata
{

};

class thread
{

};

class table
{

};

} // namespace lua

#endif // LUAPP_TYPE_HPP_INCLUDED
