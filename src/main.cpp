#include "SecMng.hpp"

using namespace secmng;

int main(void) {
    SecMng *sec = new SecMng("8000", "../web_root");
    sec->SetInstance(sec);
    sec->Init();
    sec->Run(1000);
}
