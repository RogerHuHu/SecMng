#include "SecMng.hpp"
#include "Database.hpp"

using namespace secmng;
using namespace database;

int main(void) {
    SecMng *sec = new SecMng("8000", "../web_root");
    sec->SetInstance(sec);
    sec->Init();
    sec->Run(1000);
    /*
    Database *db = new Database("../db/secmng.db");
    db->SqliteOpen();
    //db->CreateTableAccounts();
    struct UserInfo acnt;
    acnt.username = "Hqqqqq";
    acnt.password = "huqin52457";
    acnt.priority = 2;
    acnt.flag = 2;
    db->InsertUserInfo(acnt);
    db->SqliteClose();
    */
}
