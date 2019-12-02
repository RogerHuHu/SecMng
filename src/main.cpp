#include "SecMng.hpp"
#include "Database.hpp"
#include <iostream>

using namespace secmng;
using namespace database;

int main(void) {
    SecMng *sec = new SecMng("9001", "../web_root");
    sec->SetInstance(sec);
    sec->Init();
    sec->Run(1000);
    /*
    Database *db = new Database("../db/secmng.db");
    db->SqliteOpen();
    //db->CreateTableAccounts();
    struct UserInfo acnt;
    acnt.username = "admin";
    acnt.password = "12345";
    acnt.priority = 1;
    acnt.flag = 1;
    db->InsertUserInfo(acnt);
    db->CreateTableAccouts();
    db->SqliteClose();
    */
}
