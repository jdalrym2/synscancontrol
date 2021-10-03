#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <stdint.h>
#include <sstream>

#include <AsyncUDP.h>

enum class LoggingLevel
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL
};

class LoggerHandler
{
public:
    LoggerHandler(){};
    virtual void log(const char *msg);
};

class HardwareSerialLoggerHandler : public LoggerHandler
{
public:
    HardwareSerialLoggerHandler(HardwareSerial *s) : _s(s){};
    void log(const char *msg)
    {
        _s->println(msg);
    }

private:
    HardwareSerial *_s = nullptr;
};

class UDPLoggerHandler : public LoggerHandler
{
public:
    UDPLoggerHandler(AsyncUDP *udp, uint16_t udpPort)
    {
        _udpPort = udpPort;
        _udp = udp;
        if (udp->connect(IPAddress(255, 255, 255, 255), _udpPort))
        {
            Serial.println("UDP connected.");
        }
    }
    void log(const char *msg)
    {
        _udp->broadcastTo(msg, _udpPort);
    }

private:
    uint16_t _udpPort = 0;
    AsyncUDP *_udp;
};

class Logger
{
public:
    Logger(){};
    void addHardwareSerialHandler(HardwareSerial *s)
    {
        _addHandler(new HardwareSerialLoggerHandler(s));
    };
    void addUDPHandler(AsyncUDP *udp, uint16_t udpPort)
    {
        _addHandler(new UDPLoggerHandler(udp, udpPort));
    };
    void debug(const char *msg)
    {
        return _log(LoggingLevel::LOG_DEBUG, msg);
    }
    void debug(std::ostringstream *msg)
    {
        return debug(msg->str().c_str());
    }
    void info(const char *msg)
    {
        return _log(LoggingLevel::LOG_INFO, msg);
    }
    void info(std::ostringstream *msg)
    {
        return info(msg->str().c_str());
    }
    void warning(const char *msg)
    {
        return _log(LoggingLevel::LOG_WARNING, msg);
    }
    void warning(std::ostringstream *msg)
    {
        return warning(msg->str().c_str());
    }
    void error(const char *msg)
    {
        return _log(LoggingLevel::LOG_ERROR, msg);
    }
    void error(std::ostringstream *msg)
    {
        return error(msg->str().c_str());
    }
    void critical(const char *msg)
    {
        return _log(LoggingLevel::LOG_CRITICAL, msg);
    }
    void critical(std::ostringstream *msg)
    {
        return critical(msg->str().c_str());
    }

private:
    std::vector<LoggerHandler *> _handlerList;
    char buffer[2048];
    void _addHandler(LoggerHandler *handler)
    {
        _handlerList.push_back(handler);
    }
    void _log(LoggingLevel level, const char *msg)
    {
        switch (level)
        {
        case (LoggingLevel::LOG_DEBUG):
            sprintf(buffer, "[DEBUG] %s", msg);
            break;
        case (LoggingLevel::LOG_INFO):
            sprintf(buffer, "[INFO] %s", msg);
            break;
        case (LoggingLevel::LOG_WARNING):
            sprintf(buffer, "[WARNING] %s", msg);
            break;
        case (LoggingLevel::LOG_ERROR):
            sprintf(buffer, "[ERROR] %s", msg);
            break;
        case (LoggingLevel::LOG_CRITICAL):
            sprintf(buffer, "[CRITICAL] %s", msg);
            break;
        default:
            sprintf(buffer, "[INFO] %s", msg);
            break;
        }
        for (auto it = begin(_handlerList); it != end(_handlerList); ++it)
        {
            (*it)->log(buffer);
        }
    }
};

#endif /* LOGGER_H */