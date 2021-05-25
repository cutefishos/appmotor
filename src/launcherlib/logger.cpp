/***************************************************************************
**
** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (c) 2013 - 2021 Jolla Ltd.
** Copyright (c) 2021 Open Mobile Platform LLC.
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of applauncherd
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "logger.h"
#include <cstdlib>
#include <syslog.h>
#include <cstdarg>
#include <cstdio>
#include <unistd.h>
#include <ctype.h>

#include "coverage.h"

bool Logger::m_isOpened  = false;
bool Logger::m_debugMode = false;

static char *strip(char *str)
{
    if (str) {
        char *dst = str;
        char *src = str;
        while (*src && isspace(*src))
            ++src;
        for (;;) {
            while (*src && !isspace(*src))
                *dst++ = *src++;
            while (*src && isspace(*src))
                ++src;
            if (!*src)
                break;
            *dst++ = ' ';
        }
        *dst = 0;
    }
    return str;
}

static bool useSyslog()
{
    static bool checked = false;
    static bool value = false;
    if (!checked) {
        checked = true;
        value = !isatty(STDIN_FILENO);
    }
    return value;
}

void Logger::openLog(const char * progName)
{
    if (!progName)
        progName = "mapplauncherd";

    if (useSyslog()) {
        if (Logger::m_isOpened)
            Logger::closeLog();
        openlog(progName, LOG_PID, LOG_DAEMON);
        Logger::m_isOpened = true;
    }
}

void Logger::closeLog()
{
    if (useSyslog()) {
        if (Logger::m_isOpened)
            closelog();
        Logger::m_isOpened = false;
    }
}

void Logger::writeLog(const int priority, const char * format, va_list ap) 
{
    if (useSyslog()) {
        if (!Logger::m_isOpened)
            Logger::openLog();
        vsyslog(priority, format, ap);
    } else {
        char *msg = 0;
        if (vasprintf(&msg, format, ap) < 0)
            msg = 0;
        else
            strip(msg);
        fprintf(stderr, "BOOSTER(%d): %s\n", (int)getpid(), msg ?: format);
        fflush(stderr);
        free(msg);
    }
}

void Logger::logDebug(const char * format, ...)
{
    if (m_debugMode)
    {
        va_list ap;
        va_start(ap, format);
        writeLog(LOG_DEBUG, format, ap);
        va_end(ap);
    }
}

void Logger::logInfo(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    writeLog(LOG_INFO, format, ap); 
    va_end(ap);
    // To avoid extra file descriptors in forked boosters closing connection to syslog
    Logger::closeLog();
}

void Logger::logWarning(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    writeLog(LOG_WARNING, format, ap);
    va_end(ap);
}

void Logger::logError(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    writeLog(LOG_ERR, format, ap);
    va_end(ap);
}

void Logger::setDebugMode(bool enable)
{
    Logger::m_debugMode = enable;
}

