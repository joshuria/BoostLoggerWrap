#include "loggermanager.h"
#include <shared_mutex>
#include <map>
#include <vector>
#include <boost/log/core/core.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include "destination.h"
#include "logger.h"

using namespace josh::util::logger;


/*************************************************************/
// LoggerManager::Builder::Impl
/*************************************************************/
struct LoggerManager::Builder::Impl {
    std::string name;
    std::vector<std::shared_ptr<IDestination>> destinationList;
};


/*************************************************************/
// LoggerManager
/*************************************************************/
LoggerManager::Builder::~Builder() noexcept = default;


LoggerManager::Builder::Builder(): impl(new Impl()) { }


std::shared_ptr<Logger> LoggerManager::Builder::create() {
    auto instance = LoggerManager::getInstance().addLogger(*this);
    delete this;
    return instance;
}


void LoggerManager::Builder::cancel() {
    delete this;
}


LoggerManager::Builder& LoggerManager::Builder::setName(std::string const& name) {
    impl->name = name;
    return *this;
}


LoggerManager::Builder& LoggerManager::Builder::appendDestination(std::shared_ptr<IDestination> dest) {
    impl->destinationList.emplace_back(dest);
    return *this;
}


LoggerManager::Builder& LoggerManager::Builder::appendDestination(IDestination* dest) {
    impl->destinationList.emplace_back(std::shared_ptr<IDestination>(dest));
    return *this;
}


/*************************************************************/
// LoggerManager::Impl
/*************************************************************/
struct LoggerManager::Impl {
    //! All log instance table
    std::map<std::string, std::shared_ptr<Logger>> logTable;
    //! Mutex for read-write lock used by logTable
    std::shared_mutex mutex;
};


/*************************************************************/
// LoggerManager
/*************************************************************/
LoggerManager::LoggerManager(): impl(new Impl()) {
    boost::log::add_common_attributes();
}


LoggerManager::~LoggerManager() noexcept = default;


LoggerManager& LoggerManager::getInstance() noexcept {
    static LoggerManager instance;
    return instance;
}


LoggerManager::Builder& LoggerManager::newBuilder() {
    return *(new Builder());
}


std::shared_ptr<Logger> LoggerManager::get(std::string const& name) {
    std::shared_lock<decltype(impl->mutex)> readLock(impl->mutex);
    auto iter = impl->logTable.find(name);
    return iter != impl->logTable.end() ? iter->second: nullptr;
}


void LoggerManager::enableLog() {
    boost::log::core::get()->set_logging_enabled(true);
}


void LoggerManager::disableLog() {
    boost::log::core::get()->set_logging_enabled(false);
}


bool LoggerManager::isEnabled() const {
    return boost::log::core::get()->get_logging_enabled();
}


std::shared_ptr<Logger> LoggerManager::addLogger(Builder& builder) {
    auto logger = get(builder.impl->name);
    if (logger)  return logger;

    logger = std::shared_ptr<Logger>(new Logger(
        std::move(builder.impl->name), std::move(builder.impl->destinationList)));

    std::lock_guard<decltype(impl->mutex)> writeLock(impl->mutex);
    impl->logTable.emplace(logger->getName(), logger);
    
    return logger;
}

