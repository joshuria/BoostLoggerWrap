# BoostLoggerWrap
A wrap for `boost::log`, which provides:
  - A manager for querying created instances.
  - Writing log with specified level.
  - Runtime enable/disable logging.


> *Note:* Current implementation does NOT support initialization from `ptree` or configuration file.


# Require to Build
  - C++ compiler support at least C++14.
  - cmake 3.0+.
  - [The boost library](https://www.boost.org/).


# Require to Run
Depend on build configuration, runtime link requires:
  - boost\_datetime.
  - boost\_filesystem.
  - boost\_locale.
  - boost\_log.
  - boost\_system.
  - boost\_thread.


# Reference
 - [Boost.Log v2](https://www.boost.org/doc/libs/1_73_0/libs/log/doc/html/index.html);

