#include "gt.hpp"
#include "packet.h"
#include "server.h"
#include "utils.h"

using namespace std;

extern  "C"
{
#include "Lua542\include\lua.h"
#include "Lua542\include\lauxlib.h"
#include "Lua542\include\lualib.h"
    //#include "Lua542\include\luaconf.h"
}

#ifdef _WIN32
#pragma comment(lib, "Lua542/liblua54.a")
#endif

int lua_ConsoleLog(lua_State* L)
{
    string a = (string)lua_tostring(L, 1);
    //cout << a << endl;
    gt::send_log(a);
    return NULL;
}

int lua_SendPacket(lua_State* L)
{
    auto arg1 = (int)lua_tonumber(L, 1);
    auto arg2 = (string)lua_tostring(L, 2);
    g_server->send(false, arg2, arg1);
    return NULL;
}

void gt::luaexecute()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_register(L, "ConsoleLog", lua_ConsoleLog);
    lua_register(L, "SendPacket", lua_SendPacket);


    int r = luaL_dofile(L, "Script.lua");

    if (r == LUA_OK)
    {

    }
    else
    {
        string errormsg = lua_tostring(L, -1);
        gt::send_log(errormsg);
    }
    lua_close(L);
}



std::string gt::flag = "ch";
bool gt::resolving_uid2 = false;
bool gt::connecting = false;
bool gt::in_game = false;
bool gt::ghost = false;

void gt::send_log(std::string text) {
    g_server->send(true, "action|log\nmsg|" + text, NET_MESSAGE_GAME_MESSAGE);
}

void gt::solve_captcha(std::string text) {
    //Get the sum :D
    utils::replace(text,
        "set_default_color|`o\nadd_label_with_icon|big|`wAre you Human?``|left|206|\nadd_spacer|small|\nadd_textbox|What will be the sum of the following "
        "numbers|left|\nadd_textbox|",
        "");
    utils::replace(text, "|left|\nadd_text_input|captcha_answer|Answer:||32|\nend_dialog|captcha_submit||Submit|", "");
    auto number1 = text.substr(0, text.find(" +"));
    auto number2 = text.substr(number1.length() + 3, text.length());
    int result = atoi(number1.c_str()) + atoi(number2.c_str());
    send_log("Solved captcha as `2" + std::to_string(result) + "``");
    g_server->send(false, "action|dialog_return\ndialog_name|captcha_submit\ncaptcha_answer|" + std::to_string(result));
}
