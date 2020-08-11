#include <iostream>
#include "logger.h"

using namespace josh::util;


int main(int const argc, char const* argv[]) noexcept {
    std::cout << "app started" << std::endl;
    Logger& l0 = Logger::create("L0", true, false, "l0.log");
    Logger& l1 = Logger::create("L1", true, true, "l1.log");

    l0.trace("L0 trace");
    l0.debug("L0 debug");
    l0.info("L0 info");
    l0.warn("L0 warn");
    l0.error("L0 error");
    l0.fatal("L0 fatal");

    l1.trace("L1 trace");
    l1.debug("L1 debug");
    l1.info("L1 info");
    l1.warn("L1 warn");
    l1.error("L1 error");
    l1.fatal("L1 fatal");
    return 0;
}

