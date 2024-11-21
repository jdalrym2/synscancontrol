/*
 * Project Name: synscancontrol
 * File: Logger.hpp
 *
 * Copyright (C) 2024 Jon Dalrymple
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jon Dalrymple
 * Created: 13 November 2024
 * Description: Debug logging handlers
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <stdint.h>
#include <sstream>

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
    LoggerHandler() {};
    virtual void log(const char *msg) = 0;
};

class HardwareSerialLoggerHandler : public LoggerHandler
{
public:
    HardwareSerialLoggerHandler(HardwareSerial *s) : _s(s) {};
    void log(const char *msg) override
    {
        _s->println(msg);
    }

private:
    HardwareSerial *_s = nullptr;
};

class Logger
{
public:
    Logger() {};

    void addHandler(LoggerHandler *handler)
    {
        _handlerList.push_back(handler);
    }
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
    char buffer[2048]; // TODO: enable larger buffer?
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