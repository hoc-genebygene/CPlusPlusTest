#include <boost/log/sources/severity_channel_logger.hpp>

#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/global_logger_storage.hpp>

#include <unordered_map>
#include <string>

namespace logging {
using namespace ::boost::log;

enum class severity_level {
    no_severity_specified, // use "all" in properties file
    trace,
    debug,
    info,
    warning,
    error,
    fatal,
    off
};

typedef std::unordered_map<std::string, severity_level> ChannelSeverityLevels;

boost::shared_ptr< ChannelSeverityLevels > logfileChannels;
boost::shared_ptr< ChannelSeverityLevels > consoleChannels;



severity_level minimum_severity_level_for_channel(const std::string & channel){
    if(logfileChannels == nullptr || consoleChannels == nullptr) {
        return severity_level::no_severity_specified;
    }
    
    ChannelSeverityLevels::const_iterator logfile_sev = logfileChannels->find(channel);
    ChannelSeverityLevels::const_iterator console_sev = consoleChannels->find(channel);
    
    if(logfile_sev == logfileChannels->end() || console_sev == consoleChannels->end()) {
        return severity_level::no_severity_specified;
    }
    
    return std::min(logfile_sev->second, console_sev->second);
}
    
class logger {
protected:
    ::boost::log::sources::severity_channel_logger_mt<severity_level> logger_obj;
    ::logging::severity_level min_severity_level;
public:
    //normal constructor for all logging channels
    logger(const std::string & channel_name)
    : logger_obj(logging::keywords::channel = channel_name)
    , min_severity_level(::logging::minimum_severity_level_for_channel(channel_name))
    {}
    
    //default constructor for global logger
    logger()
    : min_severity_level(severity_level::no_severity_specified)
    {}
    
    logger(const logger & other) : logger_obj(other.logger_obj), min_severity_level(other.min_severity_level) {}
    logger & operator=(const logger & other) {
        logger_obj = other.logger_obj;
        min_severity_level = other.min_severity_level;
        return *this;
    }
    
    logger(logger && other) : logger_obj(other.logger_obj), min_severity_level(other.min_severity_level) {} // Do the same thing the copy constructor does...
    logger & operator=(logger && other) {
        *this = other; // Just use the copy assignment operator
        return *this;
    }
    
    // should we try to log under this severity level? this is basically an
    // early out optimization
    inline bool should_attempt_logging(::logging::severity_level log_entry_level) const {
        return log_entry_level >= min_severity_level;
    }
    
    inline ::boost::log::sources::severity_channel_logger_mt<severity_level> & getLogger() { return logger_obj; }
};
} // namespace logging

#define GET_LOGGER(channel_name) logging::logger(channel_name)

class A {
public:
    A() {
        logger_obj = GET_LOGGER("A");
    }
private:
    logging::logger logger_obj;
};


int main() {
    A a;
}
