#include "red/registry/Registrar.h"
#include <cafe.h>
#include <Zap.h>

struct Test { Test() {OSReport("AAMAIN\n");} };

Test a;

red::Registrar* zap::getRegistrar() {
    OSReport("One registrar coming right up!\n");
    static red::Registrar sRegistrar("zap");
    red::Registrar* p = &sRegistrar;
    OSReport("Returning 0x%x\n", p);
    return p;
}

void main() {
    OSReport("Welcome to ZAP\n");
    zap::getRegistrar();
    OSReport("We survived!\n");
}
