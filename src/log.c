/**
 * @file log.c
 * @author lancer (lancerstadium@163.com)
 * @brief 日志库功能实现
 * @version 0.1
 * @date 2023-12-25
 * 
 * @copyright Copyright (c) 2023
 * @note 参考项目：[Github | log.c](https://github.com/rxi/log.c)
 * 
 */
#include "log.h"
#include "color.h"

/**< 最大回调函数数量 */
#define MAX_CALLBACKS 32
/**< 开启日志颜色 */
#define LOG_USE_COLOR


/**
 * @brief 日志回调结构体
 * 
 */
typedef struct
{
    log_LogFn fn;                                   /**< 回调函数 */
    void *udata;                                    /**< 用户数据 */
    int level;                                      /**< 日志级别 */
} Callback;


/**
 * @brief 日志库全局变量结构体
 * 
 * @param L 日志库全局变量
 */
static struct
{
    void *udata;                                    /**< 用户数据 */
    log_LockFn lock;                                /**< 锁函数 */
    int level;                                      /**< 日志级别 */
    bool quiet;                                     /**< 是否静默 */
    Callback callbacks[MAX_CALLBACKS];              /**< 回调函数 */
} L;


/**
 * @brief 日志级别枚举字符串
 * @note 对应日志枚举类型 `log_Level`
 */
static const char *level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

#ifdef LOG_USE_COLOR
static const char *level_colors[] = {
    ANSI_BRIGHT_BLUE, ANSI_CYAN, ANSI_GREEN, ANSI_YELLOW, ANSI_RED, ANSI_MAGENTA};
#endif

static void stdout_callback(log_Event *ev)
{
    char buf[16];
    buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
#ifdef LOG_USE_COLOR
    fprintf(
        ev->udata, "%s %s%5s" ANSI_FMT(" %s:%d: ", ANSI_BRIGHT_BLACK),
        buf, level_colors[ev->level], level_strings[ev->level],
        ev->file, ev->line);
#else
    fprintf(
        ev->udata, "%s %-5s %s:%d: ",
        buf, level_strings[ev->level], ev->file, ev->line);
#endif
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}

static void file_callback(log_Event *ev)
{
    char buf[64];
    buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
    fprintf(
        ev->udata, "%s %-5s %s:%d: ",
        buf, level_strings[ev->level], ev->file, ev->line);
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}

static void lock(void)
{
    if (L.lock)
    {
        L.lock(true, L.udata);
    }
}

static void unlock(void)
{
    if (L.lock)
    {
        L.lock(false, L.udata);
    }
}

const char *log_level_string(int level)
{
    return level_strings[level];
}

void log_set_lock(log_LockFn fn, void *udata)
{
    L.lock = fn;
    L.udata = udata;
}

void log_set_level(int level)
{
    L.level = level;
}

void log_set_quiet(bool enable)
{
    L.quiet = enable;
}

int log_add_callback(log_LogFn fn, void *udata, int level)
{
    for (int i = 0; i < MAX_CALLBACKS; i++)
    {
        if (!L.callbacks[i].fn)
        {
            L.callbacks[i] = (Callback){fn, udata, level};
            return 0;
        }
    }
    return -1;
}

int log_add_fp(FILE *fp, int level)
{
    return log_add_callback(file_callback, fp, level);
}

static void init_event(log_Event *ev, void *udata)
{
    if (!ev->time)
    {
        time_t t = time(NULL);
        ev->time = localtime(&t);
    }
    ev->udata = udata;
}

void log_log(int level, const char *file, int line, const char *fmt, ...)
{
    log_Event ev = {
        .fmt = fmt,
        .file = file,
        .line = line,
        .level = level,
    };

    lock();

    if (!L.quiet && level >= L.level)
    {
        init_event(&ev, stderr);
        va_start(ev.ap, fmt);
        stdout_callback(&ev);
        va_end(ev.ap);
    }

    for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++)
    {
        Callback *cb = &L.callbacks[i];
        if (level >= cb->level)
        {
            init_event(&ev, cb->udata);
            va_start(ev.ap, fmt);
            cb->fn(&ev);
            va_end(ev.ap);
        }
    }

    unlock();
}
