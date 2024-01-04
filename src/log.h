/**
 * @file log.h
 * @author lancer (lancerstadium@163.com)
 * @brief 日志库头文件
 * @version 0.1.0
 * @date 2023-12-25
 * 
 * @copyright Copyright (c) 2023
 * @note 参考项目：[Github | log.c](https://github.com/rxi/log.c)
 * 
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

/** 日志库版本 */
#define LOG_VERSION "0.1.0"


/**
 * @brief 日志事件结构体
 * 
 */
typedef struct
{
    va_list ap;                                     /**< 参数列表 */
    const char *fmt;                                /**< 格式化字符串 */
    const char *file;                               /**< 文件名 */
    struct tm *time;                                /**< 时间 */
    void *udata;                                    /**< 用户数据 */
    int line;                                       /**< 行号 */
    int level;                                      /**< 日志级别 */
} log_Event;

/**
 * @brief 日志回调函数
 * 
 */
typedef void (*log_LogFn)(log_Event *ev);


/**
 * @brief 日志锁回调函数
 * 
 */
typedef void (*log_LockFn)(bool lock, void *udata);


/**
 * @brief 日志级别枚举类
 * 
 */
enum log_Level
{
    LOG_TRACE,                                      /**< 跟踪级别 */
    LOG_DEBUG,                                      /**< 调试级别 */
    LOG_INFO,                                       /**< 信息级别 */
    LOG_WARN,                                       /**< 警告级别 */
    LOG_ERROR,                                      /**< 错误级别 */
    LOG_FATAL                                       /**< 致命级别 */
};

/**
 * @brief 日志跟踪级别输出
 * 
 */
#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief 日志调试级别输出
 * 
 */
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief 日志信息级别输出
 * 
 */
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief 日志警告级别输出
 * 
 */
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief 日志错误级别输出
 * 
 */
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief 日志致命级别输出
 * 
 */
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)


/**
 * @brief 获取日志级别的字符串
 * 
 * @param level 日志级别枚举类型，参考 `log_Level`
 * @return const char* 日志级别对应的字符串指针
 */
const char *log_level_string(int level);

/**
 * @brief 设置日志锁
 * 
 * @param fn 日志锁回调函数，参考 `log_LockFn`
 * @param udata 用户数据
 */
void log_set_lock(log_LockFn fn, void *udata);


/**
 * @brief 设置日志级别
 * 
 * @param level 日志级别枚举类型，参考 `log_Level`
 */
void log_set_level(int level);


/**
 * @brief 设置日志是否静默
 * 
 * @param enable 布尔值：`true` 为静默，`false` 为不静默
 * @note 将日志库全局变量 `L` 中的 `quiet` 字段设置为 `enable`
 * 1. 对于同一段日志输出函数 `log_info` ：
 * ```c
 * // 不设置静默模式
 * log_set_quiet(false);
 * log_info("hello");
 * ```
 * 控制台输出：
 * ```
 * 12:48:17 INFO  src/main.c:26: Hello world!
 * ```
 * 
 * 2. 若设置静默模式，日志不会输出到控制台：
 * ```c
 * // 设置静默模式
 * log_set_quiet(true);
 * log_info("hello");
 * ```
 */
void log_set_quiet(bool enable);

/**
 * @brief 添加日志回调
 * 
 * @param fn 日志锁回调函数，参考 `log_LockFn`
 * @param udata 用户数据
 * @param level 日志级别枚举类型，参考 `log_Level`
 * @return int 是否成功添加日志文件
 * @retval `0` 成功
 *         `-1` 失败
 */
int log_add_callback(log_LogFn fn, void *udata, int level);

/**
 * @brief 添加日志文件和级别
 * 
 * @param fp 文件指针
 * @param level 日志级别枚举类型，参考 `log_Level`
 * @return int 是否成功添加日志文件
 * @retval `0` 成功
 *         `-1` 失败
 * @note 内部实现参考 `log_add_callback`
 */
int log_add_fp(FILE *fp, int level);

/**
 * @brief 日志输出
 * 
 * @param level 日志级别枚举类型，参考 `log_Level`
 * @param file 文件名
 * @param line 文件行号
 * @param fmt 格式化字符串
 * @param ... 其他参数
 */
void log_log(int level, const char *file, int line, const char *fmt, ...);

#endif  /* LOG_H */
