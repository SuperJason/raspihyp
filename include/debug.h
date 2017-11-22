/*
 * Copyright (c) 2017, HongluoStudio.com. All rights reserved.
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define LOG_LEVEL_NONE			0
#define LOG_LEVEL_ERROR			10
#define LOG_LEVEL_NOTICE		20
#define LOG_LEVEL_WARNING		30
#define LOG_LEVEL_INFO			40
#define LOG_LEVEL_DEBUG			50
#define LOG_LEVEL_VERBOSE		60


#if LOG_LEVEL >= LOG_LEVEL_NOTICE
#define pr_notice(...)	hyp_printf("NOTICE:  " __VA_ARGS__)
#else
#define pr_notice(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define pr_err(...)	hyp_printf("ERROR:   " __VA_ARGS__)
#else
#define pr_err(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARNING
# define pr_warn(...)	hyp_printf("WARNING: " __VA_ARGS__)
#else
# define pr_warn(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
# define pr_info(...)	hyp_printf("INFO:    " __VA_ARGS__)
#else
# define pr_info(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
# define pr_debug(...)	hyp_printf("DEBUG:   " __VA_ARGS__)
#else
# define pr_debug(...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
# define pr_verbose(...)	hyp_printf("VERBOSE: " __VA_ARGS__)
#else
# define pr_verbose(...)
#endif

void hyp_printf(const char *fmt, ...);

#endif /* __DEBUG_H__ */
