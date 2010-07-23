/* Copyright (c) 2010 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file
 * Header file for debug logs.
 */

#ifndef RAMCLOUD_LOGGING_H
#define RAMCLOUD_LOGGING_H

#include <Common.h>

namespace RAMCloud {

/**
 * The levels of verbosity for messages logged with #LOG.
 */
enum LogLevel {
    // Keep this in sync with logLevelNames defined inside _LOG.
    SILENT_LOG_LEVEL = 0,
    ERROR,
    WARNING,
    NOTICE,
    DEBUG,
    NUM_LOG_LEVELS // must be the last element in the enum
};

enum LogModule {
    DEFAULT_LOG_MODULE = 0,
    NUM_LOG_MODULES // must be the last element in the enum
};

class Logger {
  public:
    explicit Logger(LogLevel level);

    void setLogLevel(LogModule, LogLevel level);
    void setLogLevel(LogModule, int level);
    void changeLogLevel(LogModule, int delta);

    void setLogLevels(LogLevel level);
    void setLogLevels(int level);
    void changeLogLevels(int delta);

    void logMessage(LogModule module, LogLevel level, const char* format, ...)
        __attribute__((format(printf, 4, 5)));

    /**
     * Return whether the current logging configuration includes messages of
     * the given level. This is separate from #LOG in case there's some
     * non-trivial work that goes into calculating a log message, and it's not
     * possible or convenient to include that work as an expression in the
     * argument list to #LOG.
     */
    bool isLogging(LogModule module, LogLevel level) {
        return (level <= logLevels[module]);
    }

  private:
    /**
     * The stream on which to log messages.
     */
    FILE* stream;

    /**
     * An array indexed by LogModule where each entry means that, for that
     * module, messages at least as important as the entry's value will be
     * logged.
     */
    LogLevel logLevels[NUM_LOG_MODULES];

    friend class LoggerTest;
    DISALLOW_COPY_AND_ASSIGN(Logger);
};

extern Logger logger;

} // end RAMCloud

#define CURRENT_LOG_MODULE DEFAULT_LOG_MODULE

/**
 * Log a message for the system administrator.
 * The #CURRENT_LOG_MODULE macro should be set to the LogModule to which the
 * message pertains.
 * \param[in] level
 *      The level of importance of the message (LogLevel).
 * \param[in] format
 *      A printf-style format string for the message. It should not have a line
 *      break at the end, as LOG will add one.
 * \param[in] ...
 *      The arguments to the format string.
 */
#define LOG(level, format, ...) do { \
    if (RAMCloud::logger.isLogging(CURRENT_LOG_MODULE, level)) \
        RAMCloud::logger.logMessage(CURRENT_LOG_MODULE, level, \
                                    format "\n", ##__VA_ARGS__); \
} while (0)

#endif  // RAMCLOUD_LOGGING_H