/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <unistd.h>
#include <ctype.h>

#include "report.h"

static enum report_output output = report_guess;

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

void report_set_output(enum report_output new_output)
{
    if (output == new_output)
        return;

    if (output == report_syslog)
        closelog();

    if (new_output == report_syslog)
        openlog(PROG_NAME_INVOKER, LOG_PID, LOG_DAEMON);

    output = new_output;
}

static void vreport(enum report_type type, const char *msg, va_list arg)
{
    char str[400];
    char *str_type = "";
    int log_type;

    switch (type)
    {
    case report_debug:
        log_type = LOG_DEBUG;
        break;
    default:
    case report_info:
        log_type = LOG_INFO;
        break;
    case report_warning:
        str_type = "warning: ";
        log_type = LOG_WARNING;
        break;
    case report_error:
        str_type = "error: ";
        log_type = LOG_ERR;
        break;
    case report_fatal:
        str_type = "died: ";
        log_type = LOG_ERR;
        break;
    }

    vsnprintf(str, sizeof(str), msg, arg);

    if (output == report_guess) {
        if (isatty(STDIN_FILENO))
            output = report_console;
        else
            output = report_syslog;
    }

    if (output == report_console) {
        fprintf(stderr, "%s: %s%s\n", PROG_NAME_INVOKER, str_type, strip(str));
        fflush(stderr);
    } else if (output == report_syslog) {
        syslog(log_type, "%s%s", str_type, str);
    }
}

void report(enum report_type type, const char *msg, ...)
{
    va_list arg;

    va_start(arg, msg);
    vreport(type, msg, arg);
    va_end(arg);
}

void die(int status, const char *msg, ...)
{
    va_list arg;

    va_start(arg, msg);
    vreport(report_fatal, msg, arg);
    va_end(arg);

    exit(status);
}

