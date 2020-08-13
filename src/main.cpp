#include <iostream>
#include "util/logger/loggermanager.h"
#include "util/logger/logger.h"
#include "util/logger/destination.h"

using namespace josh::util::logger;


int main(int const argc, char const* argv[]) noexcept {
    std::cout << "app started" << std::endl;

    auto l1 = LoggerManager::getInstance().newBuilder()
        .setName("L1")
        .appendDestination(new FileDestination("L1.txt"))
        .appendDestination(new DebugOutputDestination())
        .create();
    auto l2 = LoggerManager::getInstance().newBuilder()
        .setName("L2")
        .appendDestination(new FileDestination("L2.txt"))
        .appendDestination(new ConsoleDestination())
        .create();

    //Logger l1("L1", "l1.log", true, true, true, true, Level::Warn);
    //Logger l2("L2", "l2.log", true, true, true, true, Level::Info);

    l1->trace("This is l1 trace.");
    l1->debug("This is l1 debug.");
    l1->info("This is l1 info.");
    l1->warn("This is l1 warn.");
    l1->error("This is l1 error.");
    l1->fatal("This is l1 fatal.");

    l2->trace("This is l2 trace.");
    l2->debug("This is l2 debug.");
    l2->info("This is l2 info.");
    l2->warn("This is l2 warn.");
    l2->error("This is l2 error.");
    l2->fatal("This is l2 fatal.");

    l2->setSeverity(Level::Info);
    l2->trace("This is l2 trace.");
    l2->debug("This is l2 debug.");
    l2->info("This is l2 info.");
    l2->warn("This is l2 warn.");
    l2->error("This is l2 error.");
    l2->fatal("This is l2 fatal.");

    //std::cin.get();
    return 0;
}

