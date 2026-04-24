#include <zap/Zap.h>
#include <red/registry/Registrar.h>
#include <telkin/Print.h>

// boilerplate
red::Registrar* zap::getRegistrar() {
    static red::Registrar sRegistrar("zap");
    return &sRegistrar;
}

void main() {
    tk::println("Welcome to ZAP");
}
