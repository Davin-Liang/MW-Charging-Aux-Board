#ifndef CONFIG_H
#define CONFIG_H

#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_SELECT_H 1
#define HAVE_SYS_SOCKET_H 0       // Windows 下没有 <sys/socket.h>
#define HAVE_NETINET_IN_H 0
#define HAVE_ARPA_INET_H 0
#define HAVE_UNISTD_H 1           // MinGW 提供部分 POSIX 函数
#define HAVE_PTHREAD_H 0
#define HAVE_STRINGS_H 0
#define HAVE_NETDB_H 0
#define HAVE_ERRNO_H 1
#define HAVE_FCNTL_H 1
#define HAVE_SYS_STAT_H 1

#endif /* CONFIG_H */
