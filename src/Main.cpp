#include <Zap.h>
#include <red/registry/Registrar.h>
#include <red/util/Log.h>

red::Registrar* zap::getRegistrar() {
    static red::Registrar sRegistrar("zap");
    red::Registrar* p = &sRegistrar;
    return p;
}

void main() {
    red::print("Welcome to ZAP\n");
}
