#pragma once
#include <string>
#include "packet.h"

//return value: true - dont send original packet, false - send original packet
namespace events {

    namespace out {
        bool worldoptions(std::string option);
        bool variantlist(gameupdatepacket_t* packet);
        bool pingreply(gameupdatepacket_t* packet);
        bool generictext(std::string packet);
        bool gamemessage(std::string packet);
        bool state(gameupdatepacket_t* packet);
        std::string speedas = "";
        std::string amx = "0";
        std::string swx = "0";
        std::string lidb = "0";
        std::string pullfal = "0";
        std::string allfal = "0";
        std::string pallfal = "0";
        std::string banfal = "0";
        std::string currentmsg = "";
        std::string pulln = "";
        std::string bann = "";
        std::string cdropcount;
        std::string saveworld;
        std::string packets = "`c[Altanb#3535] `o";//`c[Altanb#3535] `o";
        std::string dc = "Altanb#3535";
        std::string bnn = "isodogusprivkeyb";
        std::string kck = "isodogusprivkeyk";
        std::string pll = "isodogusprivkeyp";
        std::string ip;
        std::string type;
        std::string xyposition;
        std::string backas = "";
        std::string backas1 = "";
        std::string name = "";
        std::string nid = "1";
        std::string player1x = "";
        std::string player1y = "";
        std::string player2x = "";
        std::string player2y = "";
        std::string player1boxy = "";
        std::string player1boxx = "";
        std::string player2boxx = "";
        std::string player2boxy = "";
        std::string currentuid;
        std::string unaccesspaketi;
        std::string country = "";
        std::string deneme;
        std::string autopullsayi = "0";
        std::string mode = "`5Pull";
        std::string owneruid;
        std::string spam1;
        bool spaminterval = true;
        bool dropdl = false;
        bool back = false;
        bool cordinates = false;
        bool take = false;
        bool spam = false;
        bool dropbgl = false;
        bool dropwl = false;
        bool daw = false;
        bool dawas = false;
        bool pullbynames = false;
        bool xpos = false;
        bool pulas = false;
        bool banas = false;
        bool banbynames = false;
        bool automsg = false;
        bool autoban = false;
        bool autoacc = false;
        bool blind = false;
        bool taxsystem = false;
        bool daws = false;
        bool fastmmode = false;
        bool dicemod = false;
        bool coloredtext = false;
        bool worldbanjoinmod = false;
        bool autopull = false;
        bool fastdrop = false;
        bool fasttrash = false;
        bool visualspin = false;
        bool wltroll = false;
        bool wrenchpull = false;
        bool autoaccs = false;
        bool autounacc = false;
        bool blinkas = false;
        int yuzde = 10;
        int test;
        int aa = 0;
        int wlx = 0;
        int wly = 0;
        int uidz;
        int setx = 0;
        int sety = 0;
        int id = 12;
        int doorid = 123;
        int posx = 0;
        int posy = 0;
        int ruletsayi = 0;
        int iditemm = 0;
        int loggedin = 1;
        int type2;
        bool autohosts = false;
        bool dicespeed = false;
        bool visdice = false;
        extern int sayi = 1;
    }; // namespace out
    namespace in {
        bool variantlist(gameupdatepacket_t* packet);
        bool generictext(std::string packet);
        bool gamemessage(std::string packet);
        bool sendmapdata(gameupdatepacket_t* packet);
        bool state(gameupdatepacket_t* packet);
        bool tracking(std::string packet);
    }; // namespace in
};     // namespace events
