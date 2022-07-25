#include "events.h"
#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <thread>
#include "gt.hpp"
#include "math.h"
#include "proton/hash.hpp"
#include "proton/rtparam.hpp"
#include "proton/variant.hpp"
#include "proxyDialog.h"
#include "server.h"
#include "utils.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include "HTTPRequest.hpp"
using namespace events::out;
using namespace std;

bool events::out::variantlist(gameupdatepacket_t* packet) {
    variantlist_t varlist{};
    varlist.serialize_from_mem(utils::get_extended(packet));
    std::PRINTS("varlist: %s\n", varlist.print().c_str());
    return false;
}
bool events::out::pingreply(gameupdatepacket_t* packet) {
    //since this is a pointer we do not need to copy memory manually again
    packet->m_vec2_x = 1000.f;  //gravity
    packet->m_vec2_y = 250.f;   //move speed
    packet->m_vec_x = 64.f;     //punch range
    packet->m_vec_y = 64.f;     //build range
    packet->m_jump_amount = 0;  //for example unlim jumps set it to high which causes ban
    packet->m_player_flags = 0; //effect flags. good to have as 0 if using mod noclip, or etc.
    return false;
}

bool find_command(std::string chat, std::string name) {
    bool found = chat.find("/" + name) == 0;
    if (found)
        gt::send_log("`6" + chat);
    return found;
}
void force() {
    Sleep(200);
    fastdrop = false;
}
void DropItem(int itemid, int count) {
    fastdrop = true;
    std::string packetC = "action|drop\nitemID|" + to_string(itemid);
    g_server->send(false, packetC);
    Sleep(50);
    std::string packetD = "action|dialog_return\ndialog_name|drop_item\nitemID|" + to_string(itemid) + "\ncount|" + to_string(count) + "\n";
    g_server->send(false, packetD);
    thread(force).detach();
}
bool events::out::worldoptions(std::string option) {
    std::string username = "all";
    for (auto& player : g_server->m_world.players) {
        auto name_2 = player.name.substr(2); //remove color
        std::transform(name_2.begin(), name_2.end(), name_2.begin(), ::tolower);
        if (name_2.find(username)) {
            auto& bruh = g_server->m_world.local;
            if (option == "pull") {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4)) {
                    g_server->send(false, "action|input\n|text|/pull " + plyr);
                }
            }
            if (option == "kick") {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4)) {
                    g_server->send(false, "action|input\n|text|/kick " + plyr);
                }
            }
            if (option == "ban") {
                string plyr = player.name.substr(2).substr(0, player.name.length() - 4);
                if (plyr != bruh.name.substr(2).substr(0, player.name.length() - 4)) {
                    g_server->send(false, "action|input\n|text|/ban " + plyr);
                }
            }
        }
    }

    return true;
}
bool events::out::generictext(std::string packet) {
    std::PRINTS("Generic text: %s\n", packet.c_str());
    auto& world = g_server->m_world;
    rtvar var = rtvar::parse(packet);
    if (!var.valid())
        return false;
    if (packet.find("roulette2|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("te2|") + 4, packet.size());
            std::string number = aaa.c_str();
            autopull = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("autotomsgg_kasaak") != -1) {
        try {
            int msg_1 = std::stoi(packet.substr(packet.find("message_1|") + 10, packet.length() - packet.find("message_1|") - 1));
            std::string msg_2 = packet.substr(packet.find("message_2|") + 10, packet.length() - packet.find("message_2|") - 1);
            std::string delimeter = " ";
            currentmsg = delimeter + msg_2;
            gt::send_log("`9Message set to: `2" + currentmsg);
            if (msg_1 > 0) {
                automsg = true;
            }
            else {
                automsg = false;
            }
        }
        catch (std::exception) { gt::send_log("Critical Error : Something Error. Try Again Later"); }
    }
    if (packet.find("colored_text") != -1) {
        try {
            int msg_1 = std::stoi(packet.substr(packet.find("message_8|") + 10, packet.length() - packet.find("message_1|") - 1));
            std::string msg_2 = packet.substr(packet.find("message_9|") + 10, packet.length() - packet.find("message_2|") - 1);
            std::string delimeter = "";
            spam1 = delimeter + msg_2;
            if (msg_1 > 0) {
                spam = true;
                gt::send_log("`4enabled colored text");
            }
            else {
                spam = false;
            }
        }
        catch (std::exception) { gt::send_log("Critical Error : Something Error. Try Again Later"); }
    }
    if (packet.find("autobanpull") != -1) {
        try {
            int autoban = std::stoi(packet.substr(packet.find("pullnam_3|") + 10, packet.length() - packet.find("pullnam_3|") - 1));
            int autopulis = std::stoi(packet.substr(packet.find("pullnam_4|") + 10, packet.length() - packet.find("pullnam_4|") - 1));
            if (autoban > 0) {
                banas = true;
            }
            else {
                banas = false;
            }
            if (autopulis > 0) {
                pulas = true;
            }
            else {
                pulas = false;
            }
        }
        catch (std::exception) { gt::send_log("Critical Error : Something Error. Try Again Later"); }
    }
    if (packet.find("pullby_name") != -1) {
        try {
            int pullbyname = std::stoi(packet.substr(packet.find("pullnam_1|") + 10, packet.length() - packet.find("message_1|") - 1));
            std::string autopulas = packet.substr(packet.find("pullnam_2|") + 10, packet.length() - packet.find("message_2|") - 1);
            std::string basik = " ";
            pulln = basik + autopulas;
            gt::send_log("`9auto pull set to:`2" + pulln);
            if (pullbyname > 0) {
                pullbynames = true;
            }
            else {
                pullbynames = false;
            }
        }
        catch (std::exception) { gt::send_log("Critical Error : Something went wrong. Try Again Later"); }
    }
    if (packet.find("banby_name") != -1) {
        try {
            int banbyname = std::stoi(packet.substr(packet.find("pullban_1|") + 10, packet.length() - packet.find("pullban_1|") - 1));
            std::string autobanas = packet.substr(packet.find("pullban_2|") + 10, packet.length() - packet.find("pullban_2|") - 1);
            std::string basik = " ";
            bann = basik + autobanas;
            gt::send_log("`9auto ban set to:`2" + bann);
            if (banbyname > 0) {
                banbynames = true;
            }
            else {
                banbynames = false;
            }
        }
        catch (std::exception) { gt::send_log("Critical Error : Something went wrong. Try Again Later"); }
    }

    if (packet.find("fastdrop|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("rop|") + 4, packet.size());
            std::string number = aaa.c_str();
            fastdrop = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("fasttrash|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("ash|") + 4, packet.size());
            std::string number = aaa.c_str();
            fasttrash = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("visualspin|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("pin|") + 4, packet.size());
            std::string number = aaa.c_str();
            visualspin = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("takeakt|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("akt|") + 4, packet.size());
            std::string number = aaa.c_str();
            autoaccs = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("fastmode|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("ode|") + 4, packet.size());
            std::string number = aaa.c_str();
            fastmmode = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }

    if (packet.find("autotax|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("tax|") + 4, packet.size());
            std::string number = aaa.c_str();
            taxsystem = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("wltroll1|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("ll1|") + 4, packet.size());
            std::string number = aaa.c_str();
            wltroll = stoi(number);
            if (wltroll == true) {
                gt::send_log("`9worldlock`` trolling mode is now `2on");
            }
            else {
                gt::send_log("`9worldlock`` trolling mode is now `4off");
            }
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }

    if (packet.find("taxamount|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("unt|") + 4, packet.size());
            std::string number = aaa.c_str();
            yuzde = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("roulette5") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("te5|") + 4, packet.size());
            std::string number = aaa.c_str();
            while (!number.empty() && isspace(number[number.size() - 1]))
                number.erase(number.end() - (76 - 0x4B));
            ruletsayi = std::stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("dicespeed|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("eed|") + 4, packet.size());
            std::string number = aaa.c_str();
            dicespeed = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("worldbanmod|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("mod|") + 4, packet.size());
            std::string number = aaa.c_str();
            worldbanjoinmod = stoi(number);
            if (worldbanjoinmod == true) {
                gt::send_log("`9Mode search for `#@Moderators `9and `cGuardians `9successfuly started");
            }
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("colortext|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("ext|") + 4, packet.size());
            std::string number = aaa.c_str();
            coloredtext = stoi(number);
            if (coloredtext == true) {
                gt::send_log("`#enabled colored text");
            }
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("wrenchpull|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("pull|") + 4, packet.size());
            std::string number = aaa.c_str();
            while (!number.empty() && isspace(number[number.size() - 1]))
                number.erase(number.end() - (76 - 0x4B));
            wrenchpull = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("autohosts|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("sts|") + 4, packet.size());
            std::string number = aaa.c_str();
            autohosts = stoi(number);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("countryzz|") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("yzz|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));

            variantlist_t varlist{ "OnTextOverlay" };
            if (packet.find("buttonClicked|countrylist") != -1) {
                std::string paket;
                paket =
                    "\nadd_label_with_icon|big|Country List|left|3394|"
                    "\nadd_spacer|small"
                    "\nadd_textbox|`clt: `#Lithuania|left|2480|"
                    "\nadd_textbox|`ctr: `#Turkey|left|2480|"
                    "\nadd_textbox|`cen: `#USA|left|2480|"
                    "\nadd_textbox|`ckr: `#Korean|left|2480|"
                    "\nadd_textbox|`cid: `#Indonesia|left|2480|"
                    "\nadd_textbox|`caf: `#Afghanistan|left|2480|"
                    "\nadd_textbox|`cal: `#Albania|left|2480|"
                    "\nadd_textbox|`cdz: `#Algeria|left|2480|"
                    "\nadd_textbox|`cas: `#American Samoa|left|2480|"
                    "\nadd_textbox|`cad: `#Andorra|left|2480|"
                    "\nadd_textbox|`cao: `#Angola|left|2480|"
                    "\nadd_textbox|`cai: `#Anguilla|left|2480|"
                    "\nadd_textbox|`caq: `#Antarctica|left|2480|"
                    "\nadd_textbox|`cag: `#Antigua and Barbuda|left|2480|"
                    "\nadd_textbox|`car: `#Argentina|left|2480|"
                    "\nadd_textbox|`cam: `#Armenia|left|2480|"
                    "\nadd_textbox|`cth: `#Thailand|left|2480|"
                    "\nadd_textbox|`ces: `#Spain|left|2480|"
                    "\nadd_textbox|`cso: `#Somalia|left|2480|"
                    "\nadd_textbox|`cse: `#Sweden|left|2480|"
                    "\nadd_quick_exit|"
                    "\nend_dialog|end|Cancel|Okay|";
                variantlist_t liste{ "OnDialogRequest" };
                liste[1] = paket;
                g_server->send(true, liste);
            }
            else {
                country = aaa.c_str();

                varlist[1] = "`9Relog to Change The `2Flag";
                g_server->send(true, varlist);
                gt::flag = country;
                gt::send_log("`4Relog to see the changes");
                //g_server->disconnect(true);
            }
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }

    if (packet.find("buttonClicked|autopuli") != -1) {
        if (pullbynames == true) {
            pullfal = "1";
        }
        else {
            pullfal = "0";
        }
        std::string pull;
        pull =
            "add_label_with_icon|big|Auto Pull Page|left|2246|"
            "\nadd_spacer|small"
            "\nadd_checkbox|pullnam_1|`2Enable Auto Pull|" +
            pullfal +
            "|"
            "\nadd_text_input|pullnam_2|Name||20|"
            "\nadd_textbox|`#This person will be `3pulled `#once he enters the world|left|2480|"
            "\nend_dialog|pullby_name|Cancel|okay|";
        variantlist_t pot{ "OnDialogRequest" };
        pot[1] = pull;
        g_server->send(true, pot);
        return true;
    }
    if (packet.find("buttonClicked|autobani") != -1) {
        if (banbynames == true) {
            banfal = "1";
        }
        else {
            banfal = "0";
        }
        std::string ban;
        ban =
            "add_label_with_icon|big|Auto ban Page|left|2242|"
            "\nadd_spacer|small"
            "\nadd_checkbox|pullban_1|`4Enable Auto ban|" +
            banfal +
            "|"
            "\nadd_text_input|pullban_2|Name||20|"
            "\nadd_textbox|`#This person will be `4banned `#once he enters the world|left|2480|"
            "\nend_dialog|banby_name|Cancel|okay|";
        variantlist_t pot{ "OnDialogRequest" };
        pot[1] = ban;
        g_server->send(true, pot);
        return true;
    }
    if (packet.find("buttonClicked|howtouse") != -1) {
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|`4How to use auto host?|left|7188|"
            "\nadd_textbox|`9Firstly set Player`c(1)`9 position|left|2480|"
            "\nadd_textbox|`9Then set Player`c(1) Display Position|left|2480|"
            "\nadd_textbox|`9Then set Player`c(2)`9 Position|left|2480|"
            "\nadd_textbox|`9Then set Player`c(2) Display position|left|2480|"
            "\nadd_textbox|`9And Finally set `cBack`9 position|left|2480|"
            "\nend_dialog|end|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
    }
    if (packet.find("buttonClicked|gfloat") != -1) {
        g_server->send(false, "action|dialog_return\ndialog_name|floatingItems\nbuttonClicked|floatingItems");
        return true;
    }
    if (packet.find("buttonClicked|gblock") != -1) {
        g_server->send(false, "action|dialog_return\ndialog_name|statsblock\nbuttonClicked|worldBlocks");
        return true;
    }
    if (packet.find("buttonClicked|player1xy") != -1) {
        auto& bruh = g_server->m_world.local;
        int playerx = bruh.pos.m_x / 32;
        int playery = bruh.pos.m_y / 32;
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|`cPlayer`9(1)`c Position|left|274|"
            "\nadd_textbox|`cYour Current Position: `9(`c" +
            std::to_string(playerx) + "`9,`c" + std::to_string(playery) +
            "`9)|left|2480|"
            "\nadd_text_input|playerkatu|`9X:||3|" +
            player1x + "\nadd_text_input|player2zzw|`9Y:||3|" + player1y +
            "\nadd_quick_exit|"
            "\nend_dialog|end|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
    }
    if (packet.find("buttonClicked|player2xy") != -1) {
        auto& bruh = g_server->m_world.local;
        int playerx = bruh.pos.m_x / 32;
        int playery = bruh.pos.m_y / 32;
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|`cPlayer`9(2)`c Position|left|274|"
            "\nadd_textbox|`cYour Current Position: `9(`c" +
            std::to_string(playerx) + "`9,`c" + std::to_string(playery) +
            "`9)|left|2480|"
            "\nadd_text_input|player2zzs|`9X:||3|" +
            player2y + "\nadd_text_input|player2zzk|`9Y:||3|" + player2x +
            "\nadd_quick_exit|"
            "\nend_dialog|end|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
    }
    if (packet.find("buttonClicked|player1box") != -1) {
        auto& bruh = g_server->m_world.local;
        int playerx = bruh.pos.m_x / 32;
        int playery = bruh.pos.m_y / 32;
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|`cPlayer`9(1) Display Box`c Position|left|274|"
            "\nadd_textbox|`cYour Current Position: `9(`c" +
            std::to_string(playerx) + "`9,`c" + std::to_string(playery) +
            "`9)|left|2480|"
            "\nadd_text_input|labadiena|`9X:||3|" +
            player1boxx + "\nadd_text_input|player1boxs|`9Y:||3|" + player1boxy +
            "\nadd_quick_exit|"
            "\nend_dialog|end|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
    }
    if (packet.find("buttonClicked|player2boxs") != -1) {
        auto& bruh = g_server->m_world.local;
        int playerx = bruh.pos.m_x / 32;
        int playery = bruh.pos.m_y / 32;
        std::string paket;
        paket =
            "\nadd_label_with_icon|big|`cPlayer`9(2) Display Box`c Position|left|274|"
            "\nadd_textbox|`cYour Current Position: `9(`c" +
            std::to_string(playerx) + "`9,`c" + std::to_string(playery) +
            "`9)|left|2480|"
            "\nadd_text_input|player1boxk|`9X:||3|" +
            player2boxx + "\nadd_text_input|player1boxa|`9Y:||3|" + player2boxy +
            "\nadd_quick_exit|"
            "\nend_dialog|end|Cancel|Okay|";
        variantlist_t liste{ "OnDialogRequest" };
        liste[1] = paket;
        g_server->send(true, liste);
        return true;
    }
    if (packet.find("buttonClicked|optionzzz") != -1) {
        try {
            Dialog a;
            a.addLabelWithIcon("Options Panel", 758, LABEL_BIG);
            a.addSpacer(SPACER_SMALL);
            a.addCheckbox("roulette2", "`2Auto Pull", autopull);
            a.addCheckbox("wrenchpull", "`2Wrench " + mode + "", wrenchpull);
            a.addCheckbox("fastdrop", "`2Enable Fast Drop", fastdrop);
            a.addCheckbox("fasttrash", "`2Enable Fast Trash", fasttrash);
            a.addCheckbox("worldbanmod", "`2Exit world when mod joins", worldbanjoinmod);
            a.addCheckbox("autohosts", "`2Enable Show `9X`2,`4Y `2Position", autohosts);
            a.addInputBox("saveworld", "`2Save World:", saveworld, 12);
            a.endDialog("end", "Okey", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = a.finishDialog();
            g_server->send(true, liste);
        }
        catch (exception a) { gt::send_log("`4Critical Error: `2override detected"); }
    }
    if (packet.find("xvault111|") != -1) {
        std::string aaa = packet.substr(packet.find("111|") + 4, packet.size());
        try {
            posx = std::stoi(aaa.c_str());
            gt::send_log("posx: " + std::to_string(posx));
        }
        catch (exception a) { gt::send_log("`4Critical Error: `9Please enter numbers only."); }
    }
    if (packet.find("saveworld") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("rld|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            saveworld = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("speedchng") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("hng|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            speedas = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("labadiena") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("ena|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            player1boxx = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("backstd") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("std|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            backas = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("backsw") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("ksw|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            backas1 = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("player1boxs") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("oxs|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            player1boxy = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("player1boxa") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("oxa|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            player2boxy = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("player1boxk") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("oxk|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            player2boxx = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("player2zzw") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("zzw|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            player1y = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("playerkatu") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("atu|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            player1x = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("player2zzs") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("zzs|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            player2x = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("player2zzk") != -1) {
        try {
            std::string aaa = packet.substr(packet.find("zzk|") + 4, packet.size());
            while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
                aaa.erase(aaa.end() - (76 - 0x4B));
            player2y = aaa;
        }
        catch (exception a) {}
    }
    if (packet.find("2vault222|") != -1) {
        std::string aaa = packet.substr(packet.find("222|") + 4, packet.size());
        while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
            aaa.erase(aaa.end() - (76 - 0x4B));
        try {
            posy = std::stoi(aaa.c_str());
            gt::send_log("posy: " + std::to_string(posy));
        }
        catch (exception a) { gt::send_log("`4Critical Error: `9Please enter numbers only."); }
    }
    if (packet.find("buttonClicked|iso11pul1") != -1) {
        mode = "`5Pull"; //bannj
    }
    if (packet.find("buttonClicked|bannj") != -1) {
        mode = "`4Ban";
    }
    if (packet.find("buttonClicked|kicj") != -1) {
        mode = "`4Kick";
    }
    if (packet.find("iditemzz1|") != -1) {
        std::string aaa = packet.substr(packet.find("zz1|") + 4, packet.size());
        while (!aaa.empty() && isspace(aaa[aaa.size() - 1]))
            aaa.erase(aaa.end() - (76 - 0x4B));
        std::string number = aaa.c_str();
        try {
            iditemm = std::stoi(aaa.c_str());

        }
        catch (exception a) { gt::send_log("`4Critical Error: `9Please enter numbers only."); }
    }
    if (packet.find("buttonClicked|killall") != -1) {
        worldoptions("kick");
    }
    if (packet.find("buttonClicked|banall") != -1) {
        worldoptions("ban");
    }
    if (packet.find("buttonClicked|pullall") != -1) {
        worldoptions("pull");
    }
    if (packet.find("buttonClicked|ubaworld") != -1) {
        g_server->send(false, "action|input\n|text|/uba");
    }
    if (wrenchpull == true) {
        if (packet.find("action|wrench") != -1) {
            g_server->send(false, packet);

            std::string str = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
            std::string gta5 = str.substr(0, str.find("|"));
            if (mode == "`5Pull") {
                g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + gta5 + "|\nnetID|" + gta5 + "|\nbuttonClicked|pull");
            }
            if (mode == "`4Kick") {
                g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + gta5 + "|\nnetID|" + gta5 + "|\nbuttonClicked|kick");
            }
            if (mode == "`4Ban") {
                g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + gta5 + "|\nnetID|" + gta5 + "|\nbuttonClicked|worldban");
            }
            variantlist_t varlist{ "OnTextOverlay" };
            varlist[1] = "Successfuly " + mode + "`` netID: " + gta5;
            g_server->send(true, varlist);
            return true;
        }
    }
    if (var.get(0).m_key == "action" && var.get(0).m_value == "input") {
        if (var.size() < 2)
            return false;
        if (var.get(1).m_values.size() < 2)
            return false;

        if (!world.connected)
            return false;

        auto chat = var.get(1).m_values[1];

        if (find_command(chat, "name ")) { //ghetto solution, but too lazy to make a framework for commands.
            std::string name = "``" + chat.substr(6) + "``";
            variantlist_t va{ "OnNameChanged" };
            va[1] = name;
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("name set to: " + name);
            return true;
        }
        else if (find_command(chat, "doctor")) {
            variantlist_t va{ "OnNameChanged" };
            va[1] = "`4Dr. " + name;
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("name set to: `4Dr. " + name);
            return true;
        }
        else if (find_command(chat, "legend")) {
            variantlist_t va{ "OnNameChanged" };
            va[1] = "``" + name + " of Legend``";
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("name set to: " + name + " of Legend");
            return true;
            bool cordinates = false;
        }
        else if (find_command(chat, "cordinates")) {
            cordinates = true;
            auto& bruh = g_server->m_world.local;
            int playerx = bruh.pos.m_x / 32;
            int playery = bruh.pos.m_y / 32;
            variantlist_t va{ "OnNameChanged" };
            va[1] = bruh.name + " `4[" + std::to_string(playerx) + "," + std::to_string(playery) + "]";
            g_server->send(true, va, bruh.netid, -1);
            gt::send_log("`9Enabled show `4x,y`9 mode");
            return true;
            /*} else if (find_command(chat, "crash")) {
            daw = true;
            gt::send_log("`9Cashed by xuviuos");
            while (daw = true) {
                // no cooldown = "0"
                g_server->send(false, "action|input\n|text|/buy blue gem lock");
                for (auto& player : g_server->m_world.players) {
                    gameupdatepacket_t packet{ 0 };
                    packet.m_type = PACKET_SET_ICON_STATE;
                    packet.m_packet_flags = 8;
                    packet.m_int_data = utils::random(0, 2);
                    packet.m_vec2_x = utils::random(0, 2);
                    packet.m_vec_x = utils::random(0, 2);
                    packet.m_netid = player.netid;
                    for (int i = 0; i < 999; i++) {
                    }
                }
            }*/

            bool dropwl = false;
        }
        else if (find_command(chat, "dropwl ")) {
            std::string cdropcount = chat.substr(8);
            dropwl = true;
            g_server->send(false, "action|drop\n|itemID|242");
            g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|242|\ncount|" + cdropcount); //242
            gt::send_log("`9Dropping `c" + cdropcount + "`9 wls...");
            return true;
            bool daw = false;
        }
        else if (find_command(chat, "game ")) {
            std::string cdropcount = chat.substr(6);
            daw = true;
            if (daw == true) {
                int bruh = stoi(cdropcount) % 10;
                int bruh2 = stoi(cdropcount);
                if (bruh == 1) {
                    bruh2 = bruh2 - 1;
                }
                if (bruh == 2) {
                    bruh2 = bruh2 - 2;
                }
                if (bruh == 3) {
                    bruh2 = bruh2 - 3;
                }
                if (bruh == 4) {
                    bruh2 = bruh2 - 4;
                }
                if (bruh == 5) {
                    bruh2 = bruh2 + 5;
                }
                if (bruh == 6) {
                    bruh2 = bruh2 + 4;
                }
                if (bruh == 7) {
                    bruh2 = bruh2 + 3;
                }
                if (bruh == 8) {
                    bruh2 = bruh2 + 2;
                }
                if (bruh == 9) {
                    bruh2 = bruh2 + 1;
                }
                int bruh3 = bruh2 * yuzde / 100;
                int bruh4 = stoi(cdropcount) - bruh3;
                gt::send_log("`910% tax of`2 " + cdropcount + " `9is:`c " + to_string(bruh4));
            }
            return true;
            } else if (find_command(chat, "game1 ")) { //dropwl
            std::string cdropcount = chat.substr(7);
            dawas = true;
            if (dawas == true) {
                int bruh = stoi(cdropcount) % 5;
                int bruh2 = stoi(cdropcount);
                if (bruh == 1) {
                    bruh2 = bruh2 - 1;
                }
                if (bruh == 2) {
                    bruh2 = bruh2 - 2;
                }
                if (bruh == 3) {
                    bruh2 = bruh2 - 3;
                }
                if (bruh == 4) {
                    bruh2 = bruh2 - 4;
                }
                if (bruh == 5) {
                    bruh2 = bruh2 + 5;
                }
                if (bruh == 6) {
                    bruh2 = bruh2 + 4;
                }
                if (bruh == 7) {
                    bruh2 = bruh2 + 3;
                }
                if (bruh == 8) {
                    bruh2 = bruh2 + 2;
                }
                if (bruh == 9) {
                    bruh2 = bruh2 + 1;
                }
                int bruh3 = bruh2 * yuzde / 100;
                int bruh4 = stoi(cdropcount) - bruh3;
                gt::send_log("`95% tax of`2 " + cdropcount + " `9is:`c " + to_string(bruh4));
            }
            return true;

        }
        else if (find_command(chat, "win1")) {
            int posx = std::atoi(player1x.c_str());
            int posy = std::atoi(player1y.c_str());

            posx = std::atoi(player1x.c_str());
            posy = std::atoi(player1y.c_str());
            g_server->m_world.local.pos.m_x = posx * 32;
            g_server->m_world.local.pos.m_y = posy * 32;
            variantlist_t lost{ "OnSetPos" };
            lost[1] = g_server->m_world.local.pos;
            g_server->send(true, lost, g_server->m_world.local.netid, -1);
            gt::send_log("`9Teleporting to `cPlayer(1)");
            g_server->send(false, "action|input\n|text|/teleportto " + player1x + " " + player1y);
            return true;
        }
        else if (find_command(chat, "win2")) {
            int posx = std::atoi(player2x.c_str());
            int posy = std::atoi(player2y.c_str());

            posx = std::atoi(player2x.c_str());
            posy = std::atoi(player2y.c_str());
            g_server->m_world.local.pos.m_x = posx * 32;
            g_server->m_world.local.pos.m_y = posy * 32;
            variantlist_t lost{ "OnSetPos" };
            lost[1] = g_server->m_world.local.pos;
            g_server->send(true, lost, g_server->m_world.local.netid, -1);
            gt::send_log("`9Teleporting to `cPlayer(2)");
            g_server->send(false, "action|input\n|text|/teleportto " + player1x + " " + player1y);
            return true;
            } else if (find_command(chat, "take2")) {
            int posx = std::atoi(player2boxx.c_str());
            int posy = std::atoi(player2boxy.c_str());

            posx = std::atoi(player2boxx.c_str());
            posy = std::atoi(player2boxy.c_str());

            g_server->m_world.local.pos.m_x = posx * 32;
            g_server->m_world.local.pos.m_y = posy * 32;
            variantlist_t lost{ "OnSetPos" };
            lost[1] = g_server->m_world.local.pos;
            g_server->send(true, lost, g_server->m_world.local.netid, -1);
            gt::send_log("`9Took wls from `cPlayer(2)");
            g_server->send(false, "action|input\n|text|/teleportto " + player1x + " " + player1y);
            return true;
        }
        else if (find_command(chat, "back")) {
            int posx = std::atoi(backas.c_str());
            int posy = std::atoi(backas1.c_str());

            posx = std::atoi(backas.c_str());
            posy = std::atoi(backas1.c_str());
            ;
            g_server->m_world.local.pos.m_x = posx * 32;
            g_server->m_world.local.pos.m_y = posy * 32;
            variantlist_t lost{ "OnSetPos" };
            lost[1] = g_server->m_world.local.pos;
            g_server->send(true, lost, g_server->m_world.local.netid, -1);
            g_server->send(false, "action|input\n|text|/teleportto " + player1x + " " + player1y);
            return true;
        }
        else if (find_command(chat, "createacc")) {
            g_server->send(false, "action|growid");
            return true;

        }
        else if (find_command(chat, "maxlevel")) {
            std::string packet125level = country + "|showGuild|maxLevel";
            gt::send_log("`cSuccessfully Enabled `9Max level");

            variantlist_t packet123{ "OnCountryState" };
            packet123[1] = packet125level.c_str();
            g_server->send(true, packet123, g_server->m_world.local.netid, -1);
            return true;
            } else if (find_command(chat, "spawn ")) {
                    int id = stoi(chat.substr(6));
                    int x = g_server->m_world.local.pos.m_x / 32;
                    int y = (g_server->m_world.local.pos.m_y / 32) - 1;
                    g_server->addBlock(id, x, y);
                    g_server->addBlock(id, x - 1, y + 1);
                    g_server->addBlock(id, x + 1, y + 1);
                    g_server->addBlock(id, x, y + 2);
                    gt::send_log("Spawned With ID : " + to_string(id));
                    return true;
              
                    } else if (find_command(chat, "visp ")) {//visp
                            int id = std::stoi(chat.substr(6));
                            int sw = std::stoi(chat.substr(8));
                            int dw = std::stoi(chat.substr(10));
                            int x = g_server->m_world.local.pos.m_x / 32;
                            int y = (g_server->m_world.local.pos.m_y / 32) - 1;
                            int x = sw;
                            int y = dw;
                           // g_server->addBlock(id, x, y);
                            g_server->addBlock(id, x, sw);
                            g_server->addBlock(id, y, dw);
                            //g_server->addBlock(id, x, y);
                            gt::send_log("Spawned With ID : " + std::to_string(id));
                            return true;
        }
        else if (find_command(chat, "autohost")) {
            auto& bruh = g_server->m_world.local;
            int playerx = bruh.pos.m_x / 32;
            int playery = bruh.pos.m_y / 32;
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|`9Set position For `cAutoHost|left|276|"
                "|left|2480| \nadd_spacer|small\n"
                "\nadd_textbox|`#Back position Down Below|left|2480|"
                "\nadd_text_input|backstd|`9X:||3|" +
                backas + "\nadd_text_input|backsw|`9Y:||3|" + backas1 + "\nadd_textbox|`cYour Current Position: `9(`c" + std::to_string(playerx) + "`9,`c" +
                std::to_string(playery) +
                "`9)|left|2480|"
                "\nadd_textbox|`cCurent`9 (PLAYER 1`9) `c Position `9" +
                player1x + "`9:" + player1y +
                "|left|2480|"
                "\nadd_textbox|`cCurent`9 (PLAYER 2`9) `c Position `9" +
                player2x + "`9:" + player2y +
                "|left|2480|"
                "\nadd_button|player1xy|`9Set Player`c(1)`9 Position ``|noflags|0|0|"
                "\nadd_button|player2xy|`9Set Player`c(2)`9 Position ``|noflags|0|0|"
                "\nadd_button|player1box|`9Set Player`c(1) Display box`9 Position ``|noflags|0|0|"
                "\nadd_button|player2boxs|`9Set Player`c(2) Display box`9 Position ``|noflags|0|0|"
                "\nadd_button|howtouse|`2How to use``|noflags|0|0|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;

        }
        else if (find_command(chat, "growscan")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|`#Growscan|left|6016|"
                "\nadd_button|gfloat|`9Floating items``|noflags|0|0|"
                "\nadd_button|gblock|`9World blocks``|noflags|0|0|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
            bool dropdl = false;
        }
        else if (find_command(chat, "dropdl ")) {
            std::string cdropcount = chat.substr(8);
            dropdl = true;
            g_server->send(false, "action|drop\n|itemID|1796");
            g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|1796|\ncount|" + cdropcount); //1796
            gt::send_log("`9Dropping `c" + cdropcount + "`9 dls...");
            return true;
        }
        else if (find_command(chat, "mentor")) {
            std::string mn = "|showGuild|master";

            variantlist_t mentor{ "OnCountryState" };
            mentor[1] = mn.c_str();
            g_server->send(true, mentor, g_server->m_world.local.netid, -1);
            return true;
            bool dropbgl = false;
        }
        else if (find_command(chat, "dropbgl ")) {
            std::string cdropcount = chat.substr(9);
            dropbgl = true;
            g_server->send(false, "action|drop\n|itemID|7188");
            g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|7188|\ncount|" + cdropcount); //7188
            gt::send_log("`9Dropping `c" + cdropcount + "`9 bgls...");
            return true;
        }
        else if (find_command(chat, "automsg")) {
            if (automsg == true) {
                amx = "1";
            }
            else {
                amx = "0";
            }
            std::string msg;
            msg =
                "add_label_with_icon|big|Auto Msg options|left|32|"
                "\nadd_spacer|small"
                "\nadd_checkbox|message_1|Auto Message|" +
                amx +
                "|"
                "\nadd_spacer|small"
                "\nadd_textbox|`wEvery person that join's the same world as you are right now will receive a msg from you|left|2480|"
                "\nadd_textbox|`3Current msg text:" +
                currentmsg +
                "|left|2480|"
                "\nadd_text_input|message_2|Message : ||30|"
                "\nend_dialog|autotomsgg_kasaak|Cancel|Set|";
            variantlist_t send{ "OnDialogRequest" };
            send[1] = msg;
            g_server->send(true, send);
            return true;
        }
        else if (find_command(chat, "spin")) {
            Dialog a;
            a.addLabelWithIcon("Visual Spin Page", 758, LABEL_BIG);
            a.addSpacer(SPACER_SMALL);
            a.addCheckbox("visualspin", "Enable Visual Spin", visualspin);
            a.addInputBox("roulette5", "Number:", to_string(ruletsayi), 2);
            a.endDialog("end", "Okey", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = a.finishDialog();
            g_server->send(true, liste);

            return true;
            } else if (find_command(chat, "inventory")) {
                    for (server::Item& item : server::inventory) {
                        static int itemname = 1;
                        itemname = item.id + 1;

                        static int itemid = 1;
                        itemid = item.id + 1;

                        gt::send_log("Item name : " + gt::itemname[itemname] + " Item id : " + gt::itemid[itemid].c_str() +
                                     " Item amount : " + std::to_string(item.count));
                    }
                    return true;

                    } else if (find_command(chat, "pos")) {
                        auto& bruh = g_server->m_world.local;
                        int playerx = bruh.pos.m_x / 32;
                        int playery = bruh.pos.m_y / 32;
                        gt::send_log("`9Current Position X:`2 " + std::to_string(playerx));
                        gt::send_log("`2------------------------");
                        gt::send_log("`9Current Position Y:`2 " + std::to_string(playery));
                        return true;
        }
        else if (find_command(chat, "pos")) {
            xpos = !xpos;
            if (xpos)
                gt::send_log("`9Showing x,y mode enabled");
            else
                gt::send_log("`9Showing x,y mode disabled");
            return true;
            } else if (find_command(chat, "blink")) {
                blind = !blind;
                if (blind)
                    gt::send_log("`9enabled");
                else
                    gt::send_log("`9disabled");
                return true;

        }
        else if (find_command(chat, "autopull")) {
            if (pullbynames == true) {
                pullfal = "1";
            }
            else {
                pullfal = "0";
            }
            std::string pull;
            pull =
                "add_label_with_icon|big|Auto pull options|left|18|"
                "\nadd_spacer|small"
                "\nadd_checkbox|pullnam_1|`2Enable Auto Pull|" +
                pullfal +
                "|"
                "\nadd_text_input|pullnam_2|Target:||20|"
                "\nadd_textbox|`#This person will be pulled once he enters the world|left|2480|"
                "\nend_dialog|pullby_name|Cancel|okay|";
            variantlist_t pot{ "OnDialogRequest" };
            pot[1] = pull;
            g_server->send(true, pot);
            return true;
        }
        else if (find_command(chat, "flag ")) {
            int flag = atoi(chat.substr(6).c_str());
            variantlist_t va{ "OnGuildDataChanged" };
            va[1] = 1;
            va[2] = 2;
            va[3] = flag;
            va[4] = 3;
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("flag set to item id: " + std::to_string(flag));
            return true;
        }
        else if (find_command(chat, "ghost")) {
            gt::ghost = !gt::ghost;
            if (gt::ghost)
                gt::send_log("Ghost is now enabled.");
            else
                gt::send_log("Ghost is now disabled.");
            return true;
        }
        else if (find_command(chat, "take")) {
            int posx = std::atoi(player1boxx.c_str());
            int posy = std::atoi(player1boxy.c_str());

            posx = std::atoi(player1boxx.c_str());
            posy = std::atoi(player1boxy.c_str());

            g_server->m_world.local.pos.m_x = posx * 32;
            g_server->m_world.local.pos.m_y = posy * 32;
            variantlist_t lost{ "OnSetPos" };
            lost[1] = g_server->m_world.local.pos;
            g_server->send(true, lost, g_server->m_world.local.netid, -1);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            int posxk = std::atoi(player1boxx.c_str());
            int posyk = std::atoi(player1boxy.c_str());

            posxk = std::atoi(player2boxx.c_str());
            posyk = std::atoi(player2boxy.c_str());
            g_server->m_world.local.pos.m_x = posxk * 32;
            g_server->m_world.local.pos.m_y = posyk * 32;
            variantlist_t labas{ "OnSetPos" };
            labas[1] = g_server->m_world.local.pos;
            g_server->send(true, labas, g_server->m_world.local.netid, -1);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
            int sback = std::atoi(backas.c_str());
            int aback = std::atoi(backas1.c_str());

            sback = std::atoi(backas.c_str());
            aback = std::atoi(backas1.c_str());
            g_server->m_world.local.pos.m_x = sback * 32;
            g_server->m_world.local.pos.m_y = aback * 32;
            variantlist_t katu{ "OnSetPos" };
            katu[1] = g_server->m_world.local.pos;
            g_server->send(true, katu, g_server->m_world.local.netid, -1);
            gt::send_log("`9Took wls from `cPlayer(1) `9And `cPlayer(2)");
            return true;

        }
        else if (find_command(chat, "autoax")) {
            Dialog casino;
            casino.addLabelWithIcon("Auto Tax System", 758, LABEL_BIG);
            casino.addSpacer(SPACER_SMALL);
            if (taxsystem == true) {
                casino.addCheckbox("autotax", "Enable Auto Tax", CHECKBOX_SELECTED);
            }
            else {
                casino.addCheckbox("autotax", "Enable Auto Tax", CHECKBOX_NOT_SELECTED);
            }
            if (fastmmode == true) {
                casino.addCheckbox("fastmode", "Fast Drop Mode", CHECKBOX_SELECTED);
            }
            else {
                casino.addCheckbox("fastmode", "Fast Drop Mode", CHECKBOX_NOT_SELECTED);
            }
            casino.addInputBox("taxamount", "Tax Amount: %", std::to_string(yuzde), 2);
            casino.addQuickExit();
            casino.endDialog("end", "OK", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = casino.finishDialog();
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "options")) {
            Dialog a;
            a.addLabelWithIcon("Options Page", 758, LABEL_BIG);
            a.addSpacer(SPACER_SMALL);
            a.addCheckbox("roulette2", "`2Auto Pull", autopull);
            a.addCheckbox("wrenchpull", "`2Wrench " + mode + "", wrenchpull);
            a.addCheckbox("fastdrop", "`2Enable Fast Drop", fastdrop);
            a.addCheckbox("takeakt", "Enable auto take acc", autoaccs);
            a.addCheckbox("fasttrash", "`2Enable Fast Trash", fasttrash);
            a.addCheckbox("worldbanmod", "`2Exit world when mod joins", worldbanjoinmod);
            a.addCheckbox("autohosts", "`2Enable Show `9X`2,`4Y `2Position", autohosts);
            a.addInputBox("saveworld", "`2Save World:", saveworld, 12);
            a.endDialog("end", "Okey", "Cancel");
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = a.finishDialog();
            g_server->send(true, liste);
            return true;

        }
        else if (find_command(chat, "spam")) {
            if (spam == true) {
                swx = "1";
            }
            else {
                swx = "0";
            }
            std::string msg;
            msg =
                "add_label_with_icon|big|Spam Panel|left|32|"
                "\nadd_spacer|small"
                "\nadd_checkbox|message_8|`cEnable colored text|" +
                swx +
                "|"
                "\nadd_textbox|`2 To activate spam use `b//|left|2480|" +
                "|left|2480|"
                "\nadd_text_input|message_9|Spam text: ||30|"
                "\nend_dialog|colored_text|Cancel|Set|"; //"\nend_dialog|colored_text|Cancel|Set|";
            variantlist_t send{ "OnDialogRequest" };
            send[1] = msg;
            g_server->send(true, send);
            return true;
        }
        else if (find_command(chat, "/")) {
            const string wordList[8] = { "`2", "`3", "`4", "`#", "`9", "`8", "`c", "`6" };
            string word = wordList[rand() % 8];
            if (spam == true) {
                g_server->send(false, "action|input\n|text|" + word + spam1);
            }
            else {
                g_server->send(false, "action|input\n|text|" + spam1);
            }
            return true;
            } else if (find_command(chat, "/")) {
                const string wordList[8] = { "`2", "`3", "`4", "`#", "`9", "`8", "`c", "`6" };
                string word = wordList[rand() % 8];
                spaminterval = true;
                while (true) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(4000));
                    g_server->send(false, "action|input\n|text|" + word + spam1);
                }
        }
        else if (find_command(chat, "wrench")) {
            if (wrenchpull == true) {
                lidb = "1";
            }
            else {
                lidb = "0";
            }
            std::string paket;
            paket = "\nadd_label_with_icon|big|Wrench mode, Current mode is: " + mode +
                "|left|32|"
                "|left|2480| \nadd_spacer|small\n"
                "\nadd_button|kicj|`4Kick``|noflags|0|0|"
                "\nadd_button|bannj|`4Ban``|noflags|0|0|"
                "\nadd_button|iso11pul1|`5Pull``|noflags|0|0|"
                "\nadd_checkbox|wrenchpull|`#Enable Wrench Mode|" +
                lidb +
                "|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }
        if (autopull == true) {
            if (packet.find("/options") != -1) {
            }
            else {
                if (packet.find("text|") != -1) {
                    std::string str = packet.substr(packet.find("text|") + 5, packet.length() - packet.find("text|") - 1);
                    g_server->send(false, "action|input\n|text|/pull " + str);
                    return true;
                }
            }
            return true;
        }
        else if (find_command(chat, "auto")) {
            if (pulas == true) {
                allfal = "1";
            }
            else {
                allfal = "0";
            }
            if (banas == true) {
                pallfal = "1";
            }
            else {
                pallfal = "0";
            }
            std::string autod;
            autod =
                "\nadd_label_with_icon|big|Auto options|left|2250|"
                "\nadd_spacer|small"
                "\nadd_button|autopuli|`#Auto pull``|noflags|0|0|"
                "\nadd_button|autobani|`4Auto ban``|noflags|0|0|"
                "\nadd_textbox|`9This below will `#pull`9/`4ban`9 anyone who joins the world|left|2480|"
                "\nadd_checkbox|pullnam_4|`#Enable Auto Pull|" +
                allfal +
                "|"
                "\nadd_checkbox|pullnam_3|`4Enable Auto Ban|" +
                pallfal +
                "|"
                "\nadd_quick_exit|"
                "\nend_dialog|autobanpull|Cancel|okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = autod;
            g_server->send(true, liste);
            return true;

        }
        else if (find_command(chat, "country")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Country Panel|left|7188|"
                "\nadd_text_input|countryzz|`2Country Flag ID|" +
                country +
                "|3|"
                "\nadd_textbox|`2Be Clicking `4Country List `2You'll see every possible id's of country's that will be used in `4Country Flag ID`2 page|left|2480|"
                "\nadd_button|countrylist|`w`4Country List``|noflags|0|0|"
                "\nadd_quick_exit|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "config")) {
            std::string paket;
            paket =
                "add_label_with_icon|big|`9Server confgis|left|32|\nadd_spacer|small\n"
                "\nadd_text_input|serverip|`9Server IP|" +
                g_server->m_server +
                "|15|"
                "\nadd_text_input|serverport|`9Server Port|" +
                std::to_string(g_server->m_port) +
                "|5|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);

            return true;
        }
        else if (find_command(chat, "dicemode")) {
            if (dicemod == false) {
                dicemod = true;
                gt::send_log("`9AntiSpeed`` mode is now `2on");
            }
            else {
                gt::send_log("`9AntiSpeed`` mode is now `4off");
                dicemod = false;
            }
            return true;

        }
        else if (find_command(chat, "tp ")) {
            std::string name = chat.substr(4);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            for (auto& player : g_server->m_world.players) {
                auto name_2 = player.name.substr(2); //remove color
                std::transform(name_2.begin(), name_2.end(), name_2.begin(), ::tolower);
                if (name_2.find(name) == 0) {
                    gt::send_log("Teleporting to " + player.name);
                    variantlist_t varlist{ "OnSetPos" };
                    varlist[1] = player.pos;
                    g_server->m_world.local.pos = player.pos;
                    g_server->send(true, varlist, g_server->m_world.local.netid, -1);
                    break;
                }
            }
            return true;
        }
        else if (find_command(chat, "setx ")) {
            if (type2 > 2) {
                try {
                    setx = std::stoi(chat.substr(6));
                    gt::send_log("Set tileX to `5" + std::to_string(setx) + "`` successfuly");
                }
                catch (exception a) { gt::send_log("`4Critical Error: `9Please enter numbers only."); }
            }
            return true;
        }
        else if (find_command(chat, "sety ")) {
            if (type2 > 2) {
                try {
                    sety = std::stoi(chat.substr(6));
                    gt::send_log("Set tileY to `5" + std::to_string(sety) + "`` successfuly");
                }
                catch (exception a) { gt::send_log("`4Critical Error: `9Please enter numbers only."); }
            }

            return true;
        }
        else if (find_command(chat, "setid ")) {
            if (type2 > 2) {
                try {
                    id = std::stoi(chat.substr(6));
                    gt::send_log("Set DoorID to `5" + std::to_string(id) + "`` successfuly");
                }
                catch (exception a) { gt::send_log("`4Critical Error: `9Please enter numbers only."); }
            }
            return true;
        }
        else if (find_command(chat, "bypassdoor")) {
            if (type2 > 2) {
                g_server->send(false, "action|dialog_return\ndialog_name|door_edit\ntilex|" + std::to_string(setx) + "|\ntiley|" + std::to_string(sety) +
                    "|\ndoor_name|xuviuos\ndoor_target|xuviuos:123\ndoor_id|" + std::to_string(id) + "\ncheckbox_locked|1");
                gt::send_log("Hacking the door of ID of tileX `5" + std::to_string(setx) + " ``and tileY `5" + std::to_string(sety));
                gt::send_log("Please do /logs to check your `2ID");
            }

            return true;
        }
        else if (find_command(chat, "bypasspath")) {
            if (type2 > 2) {
                g_server->send(false, "action|dialog_return\ndialog_name|sign_edit\ntilex|" + std::to_string(setx) + "|\ntiley|" + std::to_string(sety) +
                    "|\nsign_text|" + std::to_string(id));
                gt::send_log("Hacking the door of ID of tileX `5" + std::to_string(setx) + " ``and tileY `5" + std::to_string(sety));
                gt::send_log("Please do /logs to check your `2ID");
            }
            return true;
             } else if (find_command(chat, "visdice")) {
            if (visdice == false) {
              visdice = true;
             gt::send_log("`9Dice Visual Mode `#Enabled");
            } else {
              visdice = false;
             gt::send_log("`9Dice Visual Mode `4Disabled");
            }
              return true;
            } else if (find_command(chat, "diceset ")) {
              try {
            sayi = std::stoi(chat.substr(5)) - 1;
              gt::send_log("`9Number set to `#" + chat.substr(5));
             } catch (std::exception) { gt::send_log("Critical error: use number"); }
              return true;
        }
        else if (find_command(chat, "save")) {
            g_server->send(false, "action|join_request\nname|" + saveworld, 3);
            return true;
        }
        else if (find_command(chat, "warp ")) {
            g_server->send(false, "action|join_request\nname|" + chat.substr(5), 3);
            gt::send_log("`9Warping to `#" + chat.substr(5));
            return true;
        }
        else if (find_command(chat, "skin ")) {
            int skin = atoi(chat.substr(5).c_str());
            variantlist_t va{ "OnChangeSkin" };
            va[1] = skin;
            g_server->send(true, va, world.local.netid, -1);
            return true;
        }
        else if (find_command(chat, "uid ")) {
             std::string name = chat.substr(5);
            gt::send_log("resolving uid for " + name);
             g_server->send(false, "action|input\n|text|/ignore /" + name);
            g_server->send(false, "action|friends");
             gt::resolving_uid2 = true;
            return true;
        }
        else if (find_command(chat, "dropall")) {
            gt::send_log("`9Dropping all items in inventory!");
            for (server::Item& item : server::inventory) {
                if (item.count > 0) {
                    if (item.id != 32 && item.id != 18)
                        DropItem(item.id, item.count);
                }
            }
            return true;
        }
        else if (find_command(chat, "world")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|World Commands|left|32|"
                "\nadd_spacer|small"
                "\nadd_button|killall|`4Kick All``|noflags|0|0|"
                "\nadd_button|banall|`4Ban All``|noflags|0|0|"
                "\nadd_button|pullall|`5Pull All``|noflags|0|0|"
                "\nadd_button|ubaworld|`5unban World``|noflags|0|0|"
                "\nadd_quick_exit|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }
        else if (find_command(chat, "proxy")) {
            std::string paket;
            paket =
                "\nadd_label_with_icon|big|Proxy Commands|left|32|"
                "\nadd_spacer|small"
                "\nadd_textbox|`b/createacc `9(creates account without going though tutorial)|left|242|"
                "\nadd_textbox|`b/autohost `9(auto host)|left|242|"
                "\nadd_textbox|`b/win1 `9(teleport u to player`c[1]`9)|left|242|"
                "\nadd_textbox|`b/win2 `9(teleport u to playerc`c[2]`9)|left|242|"
                "\nadd_textbox|`b/take `9(takes wls from player`c[1]`9 and player`c[2]`9)|left|242|"
                "\nadd_textbox|`b/back `9(teleports u back to your start position)|left|242|"
                "\nadd_textbox|`b/game `9(/game (ammount) Counts tax by 10%)|left|242|"
                //"\nadd_textbox|`b/game1 `9(/game1 (ammount) Counts tax by 5%)|left|242|"
                "\nadd_textbox|`b/pos `9(shows your x,y position)|left|242|"
                "\nadd_textbox|`b/mentor `9(gives mentor title)|left|2480|"
                "\nadd_textbox|`b/spin `9(visual spin)|left|2480|"
                "\nadd_textbox|`b/cordinates `9(Shows `4x,y`9 position)|left|242|"
                "\nadd_textbox|`b/automsg `9(sends msg to every person that joins same world as you are in|left|242|"
                "\nadd_textbox|`b/auto `9(auto pull,ban)|left|242|"
                "\nadd_textbox|`b/dropall `9(drops all items from inventory)|left|242|"
                "\nadd_textbox|`b/dropwl `9(/dropwl [count] Dropping wls With No Dialog)|left|242|"
                "\nadd_textbox|`b/dropdl `9(/dropdl [count] Dropping dls With No Dialog)|left|1796|"
                "\nadd_textbox|`b/dropbgl `9(/dropbgl [count] Dropping bgls With No Dialog)|left|7188|"
                "\nadd_textbox|`b/doctor `9(Sets your name to `4Dr. `9+your name)|left|2480|"
                "\nadd_textbox|`b/legend `9(Sets your name to your name + of legend)|left|2480|"
                "\nadd_textbox|`b/maxlevel `9(gives blue name)|left|242|"
                "\nadd_textbox|`b/spam `9(Auto Spam)|left|2480|"
                "\nadd_textbox|`b/world `9(shows world commands)|left|2480|"
                "\nadd_textbox|`b/wrench `9(Wrench Mode)|left|2480|"
                "\nadd_textbox|`b/autoac `9(auto tax)|left|2480|"
                "\nadd_textbox|`b/tp [name] `9(teleports to a player only in world) |left|2480|"
                "\nadd_textbox|`b/ghost `9(you can move without others see)|left|2480|"
                "\nadd_textbox|`b/warp [world] `9(warps to world like a `#supper supporter`9)|left|2480|"
                "\nadd_textbox|`b/flag [id] `9(sets flag to item id)|left|2480|"
                "\nadd_textbox|`b/skin [id] `9(sets your skin by id)|left|2480|"
                "\nadd_textbox|`b/name [name] `9(change name)|left|2480|"
                "\nadd_textbox|`b/options `9(features)|left|2480|"
                "\nadd_textbox|`b/country `9(Changes your country flag)|left|2480|"
                "\nadd_textbox|`b/save`9(save world editable in `8/options)|left|2480|"
                "\nadd_quick_exit|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket;
            g_server->send(true, liste);
            return true;
        }
        return false;
    }

    if (packet.find("game_version|") != -1) {
        rtvar var = rtvar::parse(packet);
        auto mac = utils::generate_mac();
        if (g_server->m_server == "213.179.209.168") {
            http::Request request{ "http://a104-125-3-135.deploy.static.akamaitechnologies.com/growtopia/server_data.php" };
            const auto response = request.send("POST", "version=3.95&protocol=163&platform=0", { "Host: www.growtopia1.com" });
            rtvar var1 = rtvar::parse({ response.body.begin(), response.body.end() });
            if (var1.find("meta"))
                g_server->meta = var1.get("meta");
            //gt changed system ,meta encrypted with aes.
            //this decrypted meta content : request of time.
            //need send request then get meta ,for current meta
        }
        var.set("meta", g_server->meta);
        var.set("country", gt::flag);
        var.set("rid", "01A5293038300C580610492AAB3B1228");
        packet = var.serialize();
        gt::in_game = false;
        PRINTS("Spoofing login info\n");
        g_server->send(false, packet);
        return true;
    }

    return false;
}



bool events::out::gamemessage(std::string packet) {
    PRINTS("Game message: %s\n", packet.c_str());
    if (packet == "action|quit") {
        g_server->quit();
        if (packet == "action|refresh_item_data\n") {
            g_server->send(false, "action|enter_game\n", 3);
            PRINTS("Skipping Updating Items\n");
            return true;
        }
    }

    return false;
}
int sayi = 1;
bool events::out::state(gameupdatepacket_t* packet) {
    if (!g_server->m_world.connected)
        return false;
    auto& bruh = g_server->m_world.local;
    if (autohosts == true) {
        int playerx = bruh.pos.m_x / 32;
        int playery = bruh.pos.m_y / 32;
        variantlist_t va{ "OnNameChanged" };
        va[1] = bruh.name + " `4[" + std::to_string(playerx) + "," + std::to_string(playery) + "]";
        g_server->send(true, va, bruh.netid, -1);
    }
    else if (xpos == true) {
        int playerx = bruh.pos.m_x / 32;
        int playery = bruh.pos.m_y / 32;
        variantlist_t va{ "OnNameChanged" };
        va[1] = bruh.name + " `4[" + std::to_string(playerx) + "," + std::to_string(playery) + "]";
        g_server->send(true, va, bruh.netid, -1);
    }
    else {
        variantlist_t va{ "OnNameChanged" };
        va[1] = bruh.name;
        g_server->send(true, va, bruh.netid, -1);
    }
    g_server->m_world.local.pos = vector2_t{ packet->m_vec_x, packet->m_vec_y };
    std::PRINTS("local pos: %.0f %.0f\n", packet->m_vec_x, packet->m_vec_y);
    if (gt::ghost)
        return true;
    return false;
}
std::string paket =
"add_label_with_icon|big|`5The Proxy Gazette``|left|5016|"
//"\nadd_image_button|banner|interface/large/wtr_lvl11_xualqu.rttex|bannerlayout|||"
"\nadd_spacer|small|\n\nadd_url_button||`1Join Discord Server``|NOFLAGS|https://discord.gg/TJykYqTMYf |Open link?|0|0|"
"UCLZOpt3XfT1Pp9U4JUC6SzQ|Would you like to open this in Youtube?|"
"\nadd_image_button|iotm_layout_4|interface/large/gazette/gazette_5columns_social_btn06.rttex|5imageslayout|https://discord.gg/TJykYqTMYf |Would you like to join "
"our "
"discord?|"
"\nadd_spacer|big|"
"\nadd_spacer|small|"
"\nadd_textbox|`4Specify your most wanted updates by joining our discord|left|5016|"
"\nadd_label_with_icon|big|`5Proxy v2.1 Change Logs``|left|7188|"
"\nadd_textbox|`b/dropall `9(drops all items from inventory)|left|242|"
"\nadd_textbox|`b/pos `9(shows your x,y position)|left|242|"
"\nadd_textbox|`b/spin `9(visual spin)|left|2480|"
"\nadd_textbox|`b/mentor `9(gives mentor title)|left|2480|"
"\nadd_label_with_icon|big|`5Proxy v2 Change Logs``|left|7188|"
"\nadd_textbox|`b/maxlevel `9(gives blue name)|left|242|"
"\nadd_textbox|`b/createacc `9(creates account without going though tutorial)|left|242|"
"\nadd_textbox|`b/autohost `9(auto host)|left|242|"
"\nadd_textbox|`b/win1 `9(teleport u to player`c[1]`9)|left|242|"
"\nadd_textbox|`b/win2 `9(teleport u to playerc`c[2]`9)|left|242|"
"\nadd_textbox|`b/take `9(takes wls from player`c[1]`9 and player`c[2]`9)|left|242|"
"\nadd_textbox|`b/back `9(teleports u back to your start position)|left|242|"
"\nadd_textbox|`b/game `9(/game (ammount) Counts tax by 10%)|left|242|"
//"\nadd_textbox|`b/cordinates `9(Shows `4x,y`9 position)|left|242|"
"\nadd_label_with_icon|big|`5Proxy v1.9 Change Logs``|left|7188|"
"\nadd_textbox|`9Added /game|left|5016|"
"\nadd_textbox|`9Added /auto|left|5016|"
"\nadd_textbox|`9Added /automsg|left|5016|"
"\nadd_label_with_icon|big|`5Proxy v1.8 Change Logs``|left|7188|"
// "\nadd_spacer|small|"
"\nadd_textbox|`9Added /dropwl|left|5016|"
"\nadd_textbox|`9Added /dropdl|left|5016|"
"\nadd_textbox|`9Added /dropbgl|left|5016|"
"\nadd_label_with_icon|big|`5Proxy v1.7 Change Logs``|left|7188|"
// "\nadd_spacer|small|"
"\nadd_textbox|`9Added /doctor|left|5016|"
"\nadd_textbox|`9Added /legend|left|5016|"
"\nadd_textbox|`9Added /spam|left|5016|"
"\nnadd_textbox|`9Added /wrench|left|5016|"
"\nadd_quick_exit|"
"\nset_survey_enabled|1"
//"\nadd_spacer|big"
// "\nadd_spacer|small"
"\nend_dialog|end|Cancel|Okay|";
bool events::in::variantlist(gameupdatepacket_t* packet) {
    variantlist_t varlist{};
    auto extended = utils::get_extended(packet);
    extended += 4; //since it casts to data size not data but too lazy to fix this
    varlist.serialize_from_mem(extended);
    std::PRINTC("varlist: %s\n", varlist.print().c_str());
    auto func = varlist[0].get_string();

    //probably subject to change, so not including in switch statement.
    if (func.find("OnSuperMainStartAcceptLogon") != -1)
        gt::in_game = true;
    switch (hs::hash32(func.c_str())) {
        //solve captcha
    case fnv32("onShowCaptcha"): {

    } break; //OnPlayPositioned
    case fnv32("OnRequestWorldSelectMenu"): {
        auto& world = g_server->m_world;
        world.players.clear();
        world.local = {};
        world.connected = false;
        world.name = "EXIT";
    } break;

    case fnv32("OnSendToServer"): g_server->redirect_server(varlist); return true;

    case fnv32("OnConsoleMessage"): {
        auto wry = varlist[1].get_string(); //std::this_thread::sleep_for(std::chrono::milliseconds(50));
        g_server->send(false, "action|input\n|text|/back");
         if (back == true) {
            if (take == true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|input\n|text|/back");
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                g_server->send(false, "action|input\n|text|/take1");
            }
            }
        if (autoaccs == true) {
            if (wry.find("wants to add you to a World Lock. Wrench yourself to accept.") != -1) {
                g_server->send(false, "action|dialog_return\ndialog_name|acceptaccess");
                g_server->send(false, "action|dialog_return\ndialog_name|acceptlock");
            }
        }
        if (worldbanjoinmod == true) {
            if (wry.find("Removed your access from all locks.") != -1) {
                gt::send_log("`$Leaving the world due to having Mod bypass on and due to having a `#mod `$in the world!");
                g_server->send(false, "action|join_request\nname|exit", 3);
            }
        }
        if (visualspin == true) {
            if (wry.find("`7[```w" + name + "`` spun the wheel and got") != -1) {
                if (std::to_string(ruletsayi) == "0") {
                    varlist[1] = "`7[```w" + name + "`` spun the wheel and got `2" + std::to_string(ruletsayi) + "``!`7]``";
                }
                else {
                    varlist[1] = "`7[```w" + name + "`` spun the wheel and got `4" + std::to_string(ruletsayi) + "``!`7]``";
                }
                g_server->send(true, varlist, -1, 1900);
                return true;
            }
            else {
                if (wry.find("`7[```2" + name + "`` spun the wheel and got") != -1) {
                    if (std::to_string(ruletsayi) == "0") {
                        varlist[1] = "`7[```2" + name + "`` spun the wheel and got `2" + std::to_string(ruletsayi) + "``!`7]``";
                    }
                    else {
                        varlist[1] = "`7[```2" + name + "`` spun the wheel and got `4" + std::to_string(ruletsayi) + "``!`7]``";
                    }
                    g_server->send(true, varlist, -1, 1900);
                    return true;
                }
                else {
                    varlist[1] = packets + varlist[1].get_string();
                    g_server->send(true, varlist);
                    return true;
                }
            }
        }
        else {
            varlist[1] = packets + varlist[1].get_string();
            g_server->send(true, varlist);
            return true;
        }
    } break;
    case fnv32("OnTalkBubble"): {
        auto wry = varlist[2].get_string();
        if (visualspin == true) {
            if (wry.find("`7[```w" + name + "`` spun the wheel and got") != -1) {
                if (std::to_string(ruletsayi) == "0") {
                    varlist[2] = "`7[```w" + name + "`` spun the wheel and got `2" + std::to_string(ruletsayi) + "``!`7]``";
                }
                else {
                    varlist[2] = "`7[```w" + name + "`` spun the wheel and got `4" + std::to_string(ruletsayi) + "``!`7]``";
                }
                g_server->send(true, varlist, -1, 1900);
                return true;
            }
            else {
                if (wry.find("`7[```2" + name + "`` spun the wheel and got") != -1) {
                    if (std::to_string(ruletsayi) == "0") {
                        varlist[2] = "`7[```2" + name + "`` spun the wheel and got `2" + std::to_string(ruletsayi) + "``!`7]``";
                    }
                    else {
                        varlist[2] = "`7[```2" + name + "`` spun the wheel and got `4" + std::to_string(ruletsayi) + "``!`7]``";
                    }
                    g_server->send(true, varlist, -1, 1900);
                    return true;
                }
            }
        }
        else {
            g_server->send(true, varlist);
            return true;
        }
    } break;
    case fnv32("OnDialogRequest"): {
        auto content = varlist[1].get_string();

        if (dropwl == true) {
            if (content.find("Drop") != -1) {
                dropwl = false;
                return true;
            }
        }
        else if (dropdl == true) {
            if (content.find("Drop") != -1) {
                dropdl = false;
                return true;
            }
        }
        else if (dropbgl == true) {
            if (content.find("Drop") != -1) {
                dropbgl = false;
                return true;
            }
        }


        if (gt::resolving_uid2 && (content.find("friend_all|Show offline") != -1 || content.find("Social Portal") != -1) ||
            content.find("Are you sure you wish to stop ignoring") != -1) {
            return true;
        }
        else if (content.find("add_label_with_icon|small|Remove Your Access From World|left|242|") != -1) {
            if (worldbanjoinmod == true) {
                g_server->send(false, "action|dialog_return\ndialog_name|unaccess");
                return true;
            }
        }
        else if (gt::resolving_uid2 && content.find("Ok, you will now be able to see chat") != -1) {
            gt::resolving_uid2 = false;
            return true;
        }
        else if (gt::resolving_uid2 && content.find("`4Stop ignoring") != -1) {
            int pos = content.rfind("|`4Stop ignoring");
            auto ignore_substring = content.substr(0, pos);
            auto uid = ignore_substring.substr(ignore_substring.rfind("add_button|") + 11);
            auto uid_int = atoi(uid.c_str());
            if (uid_int == 0) {
                gt::resolving_uid2 = false;
                gt::send_log("name resolving seems to have failed.");
            }
            else {
                gt::send_log("`9Target UID: `#" + uid);
                gt::send_log("`9worldlock troll UID set to: `#" + uid);
                uidz = stoi(uid);
                g_server->send(false, "action|dialog_return\ndialog_name|friends\nbuttonClicked|" + uid);
                g_server->send(false, "action|dialog_return\ndialog_name|friends_remove\nfriendID|" + uid + "|\nbuttonClicked|remove");
            }
            return true;
        }
        if (wltroll == true) {
            if (content.find("add_label_with_icon|big|`wEdit World Lock``|left|242|") != -1) {
                int x = std::stoi(content.substr(content.find("embed_data|tilex|") + 17, content.length() - content.find("embed_data|tilex|") - 1));
                int y = std::stoi(content.substr(content.find("embed_data|tiley|") + 17, content.length() - content.find("embed_data|tiley|") - 1));
                g_server->send(false, "action|dialog_return\ndialog_name|lock_edit\ntilex|" + std::to_string(x) + "|\ntiley|" + std::to_string(y) + "|\ncheckbox_" +
                    to_string(uidz) +
                    "|0\ncheckbox_public|0\ncheckbox_disable_music|0\ntempo|100\ncheckbox_disable_music_render|0\ncheckbox_set_as_home_world|"
                    "0\nminimum_entry_level|1");
                return true;
            }
        }
        if (content.find("add_label_with_icon|big|`wEdit World Lock``|left|") != -1) {
            gt::send_log("find world lock x,y path.");
            wlx = std::stoi(content.substr(content.find("embed_data|tilex|") + 17, content.length() - content.find("embed_data|tilex|") - 1));
            wly = std::stoi(content.substr(content.find("embed_data|tiley|") + 17, content.length() - content.find("embed_data|tiley|") - 1));
        }
        if (dicemod == true) {
            if (content.find("add_label_with_icon|big|`wEdit Dice Block``|left|456|") != -1) {
                int x = std::stoi(content.substr(content.find("embed_data|tilex|") + 17, content.length() - content.find("embed_data|tilex|") - 1));
                int y = std::stoi(content.substr(content.find("embed_data|tiley|") + 17, content.length() - content.find("embed_data|tiley|") - 1));
                if (content.find("add_checkbox|checkbox_public|Usable by public|0") != -1) {
                    gt::send_log("`4 (X:" + to_string(x) + " Y:" + to_string(y) + ")`9 Dice is `2Public");
                    g_server->send(false, "action|dialog_return\ndialog_name|boombox_edit\ntilex|" + to_string(x) + "|\ntiley|" + to_string(y) +
                        "|\ncheckbox_public|1\ncheckbox_silence|1");
                    return true;

                }
                else {
                    gt::send_log("`4 (X:" + to_string(x) + " Y:" + to_string(y) + ")`9 Dice is `4Not Public");
                    g_server->send(false, "action|dialog_return\ndialog_name|boombox_edit\ntilex|" + to_string(x) + "|\ntiley|" + to_string(y) +
                        "|\ncheckbox_public|0\ncheckbox_silence|1");
                    return true;
                }
                return true;
            }
        }
        if (content.find("set_default_color|`o") != -1) {
            if (content.find("add_label_with_icon|big|`wAre you Human?``|left|206|") != -1) {



            }
        }
        if (content.find("add_label_with_icon|big|`wThe Growtopia Gazette``|left|5016|") != -1) {
            varlist[1] = paket;
            g_server->send(true, varlist);
            return true;
        }
        if (taxsystem == true) {
            if (content.find("Drop") != -1) {
                if (content.find("embed_data|itemID|") != -1) {
                    std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
                    gt::send_log(itemid);
                    if (itemid.find("242") != -1) {
                        std::string count = content.substr(content.find("count||") + 7, content.length() - content.find("count||") - 1);
                        int bruh = stoi(count) % 10;
                        int bruh2 = stoi(count);
                        if (bruh == 1) {
                            bruh2 = bruh2 - 1;
                        }
                        if (bruh == 2) {
                            bruh2 = bruh2 - 2;
                        }
                        if (bruh == 3) {
                            bruh2 = bruh2 - 3;
                        }
                        if (bruh == 4) {
                            bruh2 = bruh2 - 4;
                        }
                        if (bruh == 5) {
                            bruh2 = bruh2 + 5;
                        }
                        if (bruh == 6) {
                            bruh2 = bruh2 + 4;
                        }
                        if (bruh == 7) {
                            bruh2 = bruh2 + 3;
                        }
                        if (bruh == 8) {
                            bruh2 = bruh2 + 2;
                        }
                        if (bruh == 9) {
                            bruh2 = bruh2 + 1;
                        }
                        int bruh3 = bruh2 * yuzde / 100;
                        int bruh4 = stoi(count) - bruh3;
                        if (fastmmode == true) {
                            g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|" + itemid + "|\ncount|" + std::to_string(bruh4));
                            return true;

                        }
                        else {
                            varlist[1] =
                                "set_default_color|`o\nadd_label_with_icon|big|`wDrop World Lock``|left|242|\nadd_textbox|How many to "
                                "drop?|left|\nadd_text_input|count||" +
                                to_string(bruh4) + "|5|\nembed_data|itemID|242\nend_dialog|drop_item|Cancel|OK|";
                            g_server->send(true, varlist);
                            return true;
                        }
                    }
                }
            }
        }
        if (fastdrop == true) {
            std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
            std::string count = content.substr(content.find("count||") + 7, content.length() - content.find("count||") - 1);
            if (content.find("embed_data|itemID|") != -1) {
                if (content.find("Drop") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|" + itemid + "|\ncount|" + count);
                    return true;
                }
            }
        }
        if (taxsystem == true) {
            if (fastmmode == true) {
                std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
                std::string count = content.substr(content.find("count||") + 7, content.length() - content.find("count||") - 1);
                if (content.find("embed_data|itemID|") != -1) {
                    if (content.find("Drop") != -1) {
                        if (itemid.find("242") != -1) {
                            return true;
                        }
                    }
                }
            }
        }
        if (fasttrash == true) {
            std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
            std::string count = content.substr(content.find("you have ") + 9, content.length() - content.find("you have ") - 1);
            std::string s = count;
            std::string delimiter = ")";
            std::string token = s.substr(0, s.find(delimiter));
            if (content.find("embed_data|itemID|") != -1) {
                if (content.find("Trash") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|trash_item\nitemID|" + itemid + "|\ncount|" + token);
                    return true;
                }
            }
        }
        if (wrenchpull == true) {
            std::string netidyarragi = content.substr(content.find("embed_data|netID") + 17, content.length() - content.find("embed_data|netID") - 1);
            if (content.find("set_default_color|`o") != -1) { // trash bug fix
                if (content.find("embed_data|netID") != -1) {
                    return true;
                }
            }

        }
        else if (content.find("add_button|my_worlds|`$My Worlds``|noflags|0|0|") != -1) {
            content = content.insert(content.find("add_button|my_worlds|`$My Worlds``|noflags|0|0|"),
                "\nadd_button|whitelisted_players|`2Whitelisted Players``|noflags|0|0|\n\nadd_button|blacklisted_players|`4Blacklisted "
                "Players``|noflags|0|0|\nadd_button|optionzzz|`5Options``|noflags|0|0|\n");

            varlist[1] = content;
            g_server->send(true, varlist, -1, -1);
            return true;
        }
    } break;
    case fnv32("OnRemove"): {
        auto text = varlist.get(1).get_string();
        if (text.find("netID|") == 0) {
            auto netid = atoi(text.substr(6).c_str());

            if (netid == g_server->m_world.local.netid)
                g_server->m_world.local = {};

            auto& players = g_server->m_world.players;
            for (size_t i = 0; i < players.size(); i++) {
                auto& player = players[i];
                if (player.netid == netid) {
                    players.erase(std::remove(players.begin(), players.end(), player), players.end());
                    break;
                }
            }
        }
    } break;
    case fnv32("OnSpawn"): {
        std::string meme = varlist.get(1).get_string();
        rtvar var = rtvar::parse(meme);
        auto name = var.find("name");

        auto netid = var.find("netID");
        auto onlineid = var.find("onlineID");
        auto userid = var.find("userID");
        if (name->m_value.find("`^") != -1) {
            std::string unaccess = userid->m_value;
            if (unaccesspaketi.find("\ncheckbox_" + userid->m_value + "|0") != -1) {
            }
            else {
                unaccesspaketi += "\ncheckbox_" + userid->m_value + "|0";
            }
        }
        if (blind == true) {
            std::vector<uint32_t> Skinlist;
            Skinlist = { 1348237567, 1685231359, 2022356223, 2190853119, 2527912447, 2864971775, 3033464831, 3370516479 };
            while (blind == true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                string color = "action|setSkin\ncolor|" + std::to_string(Skinlist.at(utils::random(0, 7)));
                g_server->send(false, color);
                return true;
            }
        }
        if (name && netid && onlineid) {
            player ply{};
            if (var.find("invis")->m_value != "1") {
                ply.name = name->m_value;
                ply.country = var.get("country");
                if (autohosts == true) {
                    name->m_values[0] += " `4[" + netid->m_value + "]``" + " `#[" + userid->m_value + "]``";
                }
                auto pos = var.find("posXY");
                if (pos && pos->m_values.size() >= 2) {
                    auto x = atoi(pos->m_values[0].c_str());
                    auto y = atoi(pos->m_values[1].c_str());
                    ply.pos = vector2_t{ float(x), float(y) };
                }
            }
            else {
                gt::send_log("Moderator entered the world. 1/2");
                ply.mod = true;
                ply.invis = true;
            }
            if (var.get("mstate") == "1" || var.get("smstate") == "1")
                ply.mod = true;
            ply.userid = var.get_int("userID");
            ply.netid = var.get_int("netID");
            if (meme.find("type|local") != -1) {
                var.find("mstate")->m_values[0] = "1";
                g_server->m_world.local = ply;
            }
            g_server->m_world.players.push_back(ply);
            auto str = var.serialize();
            utils::replace(str, "onlineID", "onlineID|");
            varlist[1] = str;
            std::PRINTC("new: %s\n", varlist.print().c_str());
            g_server->send(true, varlist, -1, -1);
            if (automsg == true) {
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    g_server->send(false, "action|input\n|text|/msg " + ply.name + currentmsg);
                }
                catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
            }
            if (pulas == true) {
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    for (auto& player : g_server->m_world.players) {
                        auto name_2 = player.name.substr(2); //remove color
                        string outputas = player.name.substr(2).substr(0, player.name.length() - 4);
                        g_server->send(false, "action|input\n|text|/pull " + outputas);
                    }
                }
                catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
            }
            if (banas == true) {
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    for (auto& player : g_server->m_world.players) {
                        auto name_2 = player.name.substr(2); //remove color
                        string outputas = player.name.substr(2).substr(0, player.name.length() - 4);
                        g_server->send(false, "action|input\n|text|/ban " + outputas);
                    }
                }
                catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
            }
            if (pullbynames == true) {
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    g_server->send(false, "action|input\n|text|/pull " + pulln);
                }
                catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
            }
            if (banbynames == true) {
                try {
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                    g_server->send(false, "action|input\n|text|/ban " + bann);
                }
                catch (std::exception) { gt::send_log("Critical Error : Invalid String Position"); }
            }
            return true;
        }
    } break;
    }
    return false;
}
bool events::in::generictext(std::string packet) {
    std::PRINTC("Generic text: %s\n", packet.c_str());

    return false;
}

bool events::in::gamemessage(std::string packet) {
    std::PRINTC("Game message: %s\n", packet.c_str());
    if (gt::resolving_uid2) {
        if (packet.find("PERSON IGNORED") != -1) {
            g_server->send(false, "action|dialog_return\ndialog_name|friends_guilds\nbuttonClicked|showfriend");
            g_server->send(false, "action|dialog_return\ndialog_name|friends\nbuttonClicked|friend_all");
        }
        else if (packet.find("Nobody is currently online with the name") != -1) {
            gt::resolving_uid2 = false;
            gt::send_log("Target is offline, cant find uid.");
        }
        else if (packet.find("Clever perhaps") != -1) {
            gt::resolving_uid2 = false;
            gt::send_log("Target is a moderator, can't ignore them.");
        }
    }
    return false;
}

bool events::in::sendmapdata(gameupdatepacket_t* packet) {
    g_server->m_world = {};
    auto extended = utils::get_extended(packet);
    extended += 4;
    auto data = extended + 6;
    auto name_length = *(short*)data;

    char* name = new char[name_length + 1];
    memcpy(name, data + sizeof(short), name_length);
    char none = '\0';
    memcpy(name + name_length, &none, 1);

    g_server->m_world.name = std::string(name);
    g_server->m_world.connected = true;
    delete[] name;
    std::PRINTC("world name is %s\n", g_server->m_world.name.c_str());

    return false;
}

bool events::in::state(gameupdatepacket_t* packet) {
    if (!g_server->m_world.connected)
        return false;
    if (packet->m_player_flags == -1)
        return false;

    auto& players = g_server->m_world.players;
    for (auto& player : players) {
        if (player.netid == packet->m_player_flags) {
            player.pos = vector2_t{ packet->m_vec_x, packet->m_vec_y };
            std::PRINTC("player %s position is %.0f %.0f\n", player.name.c_str(), player.pos.m_x, player.pos.m_y);
            break;
        }
    }
    return false;
}

bool events::in::tracking(std::string packet) {
    std::PRINTC("Tracking packet: %s\n", packet.c_str());
    if (packet.find("eventName|102_PLAYER.AUTHENTICATION") != -1) {
        string wlbalance = packet.substr(packet.find("Worldlock_balance|") + 18, packet.length() - packet.find("Worldlock_balance|") - 1);

        if (wlbalance.find("PLAYER.") != -1) {
            gt::send_log("`4World Lock Balance: `20");
        }
        else {
            gt::send_log("`4World Lock Balance: `2" + wlbalance);
        }
    }
    if (packet.find("eventName|100_MOBILE.START") != -1) {
        rtvar var = rtvar::parse(packet);
        string gem = packet.substr(packet.find("Gems_balance|") + 13, packet.length() - packet.find("Gems_balance|") - 1);
        string level = packet.substr(packet.find("Level|") + 6, packet.length() - packet.find("Level|") - 1);
        string uidd = packet.substr(packet.find("GrowId|") + 7, packet.length() - packet.find("GrowId|") - 1);
        gt::send_log("`4Gems Balance: `2" + gem);
        gt::send_log("`4Account Level: `2" + level);
    }
    if (packet.find("eventName|300_WORLD_VISIT") != -1) {
        if (packet.find("Locked|0") != -1) {
            rtvar var = rtvar::parse(packet);
            gt::send_log("`4This world is not locked by a world lock.");
            g_server->send(false, "action|input\n|text|/stats");
        }
        else {
            gt::send_log("`2This world is locked by a world lock.");

            if (packet.find("World_owner|") != -1) {
                string uidd = packet.substr(packet.find("World_owner|") + 12, packet.length() - packet.find("World_owner|") - 1);
                gt::send_log("`4World Owner UID: `2" + uidd);
                owneruid = uidd;
                g_server->send(false, "action|input\n|text|/stats");
            }
        }
    }

    return true;
}
