#include "SecMng.hpp"
#include "Database.hpp"

using namespace secmng;
using namespace database;

int main(void) {
    SecMng *sec = new SecMng("8000", "../web_root");
    sec->SetInstance(sec);
    sec->Init();
    sec->Run(1000);

}
