#include "loggermanager.h"
#include <locale>
#include <codecvt>
#include <fstream>
#include <ostream>
#include <exception>
#include <ctime>
#include <shared_mutex>

using namespace josh::util::logger;


/*************************************************************/
// LoggerManager::Builder::Impl
/*************************************************************/
struct LoggerManager::Builder::Impl {
    ;
};


/*************************************************************/
// LoggerManager
/*************************************************************/
LoggerManager::Builder::~Builder() noexcept = default;


LoggerManager::Builder::Builder(): impl(new Impl()) {
    ;
}


std::shared_ptr<Logger> LoggerManager::Builder::create() {
    return nullptr;
}


LoggerManager::Builder& LoggerManager::Builder::setName(std::string const& name) {
    return *this;
}


LoggerManager::Builder& LoggerManager::Builder::appendDestination(Destination&& dest, Level severity) {
    return *this;
}


/*************************************************************/
// LoggerManager::Impl
/*************************************************************/
struct LoggerManager::Impl {
};


/*************************************************************/
// LoggerManager
/*************************************************************/
LoggerManager::LoggerManager(): impl(new Impl()) {
    ;
}


LoggerManager::~LoggerManager() noexcept = default;


LoggerManager& LoggerManager::getInstance() noexcept {
    static LoggerManager instance;
    return instance;
}


LoggerManager::Builder&& LoggerManager::newBuilder() {
    return std::move(Builder());
}


std::shared_ptr<Logger> LoggerManager::get(std::string const& name) {
    return nullptr;
}


void LoggerManager::enableLog() {
    ;
}


void LoggerManager::disableLog() {
    ;
}


bool LoggerManager::isEnabled() const {
    return false;
}


void LoggerManager::setGlobalSeverity(Level level) {
    ;
}


void LoggerManager::addLogger(Builder& builder) {
    ;
}

