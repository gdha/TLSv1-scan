/* TLSv1-scan.c
 * Written by Gratien D'haese
 * Version 1.1 (date 13/Apr/2026)
 *
 * Scan the local or a remote system for services using TLSv1.0 or TLSv1.1.
 * Requires: sslscan, ss (iproute), lsof, netstat
 * For remote host scanning also requires: nmap
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define PRGNAME "TLSv1-scan"
#define VERSION "1.1"
#define DATE    "13/Apr/2026"

static void handle_interrupt(int sig)
{
    (void)sig;
    fprintf(stderr, "\nCtrl+C was pressed. Exit now.\n");
    exit(1);
}

/* Returns 1 if s contains only alphanumeric characters, hyphens, or underscores */
static int is_safe_cmd(const char *s)
{
    for (; *s; s++) {
        if (!((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') ||
              (*s >= '0' && *s <= '9') || *s == '-' || *s == '_'))
            return 0;
    }
    return 1;
}

/* Returns 1 if s is a valid IPv4 address or wildcard (digits, dots, asterisk) */
static int is_safe_host(const char *s)
{
    for (; *s; s++) {
        if (!((*s >= '0' && *s <= '9') || *s == '.' || *s == '*'))
            return 0;
    }
    return 1;
}

/* Returns 1 if s is a valid hostname or IPv4/IPv6 address.
 * Allows only alphanumeric characters, dots, hyphens, and colons (for IPv6).
 * This strict whitelist ensures the value is safe to embed in shell commands. */
static int is_valid_target(const char *s)
{
    if (!*s)
        return 0;
    for (; *s; s++) {
        if (!((*s >= 'a' && *s <= 'z') || (*s >= 'A' && *s <= 'Z') ||
              (*s >= '0' && *s <= '9') || *s == '.' || *s == '-' || *s == ':'))
            return 0;
    }
    return 1;
}

/* Returns 1 if s contains only digits */
static int is_numeric(const char *s)
{
    if (!*s)
        return 0;
    for (; *s; s++) {
        if (*s < '0' || *s > '9')
            return 0;
    }
    return 1;
}

static int command_exists(const char *cmd)
{
    char buf[256];
    if (!is_safe_cmd(cmd))
        return 0;
    snprintf(buf, sizeof(buf), "which %s >/dev/null 2>&1", cmd);
    return system(buf) == 0;
}

int main(int argc, char *argv[])
{
    const char *target_host = "localhost";

    /* Parse command-line arguments */
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--host") == 0) && i + 1 < argc) {
            target_host = argv[++i];
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: %s [--host <hostname|IP>]\n\n", PRGNAME);
            printf("  --host <hostname|IP>  Host to scan for TLSv1.0/TLSv1.1 (default: localhost)\n");
            printf("  --help, -h            Show this help message\n");
            return 0;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            fprintf(stderr, "Usage: %s [--host <hostname|IP>]\n", PRGNAME);
            return 1;
        }
    }

    if (!is_valid_target(target_host)) {
        fprintf(stderr, "Invalid host name or address: %s\n", target_host);
        return 1;
    }

    printf("\n"
           "******************************************\n"
           "* %s by Gratien D'haese\n"
           "* Version %s\n"
           "* Date %s\n"
           "******************************************\n\n",
           PRGNAME, VERSION, DATE);

    int is_local = (strcmp(target_host, "localhost") == 0 ||
                    strcmp(target_host, "127.0.0.1") == 0 ||
                    strcmp(target_host, "::1") == 0);

    if (is_local && getuid() != 0) {
        fprintf(stderr, "Program %s needs to be run as root when scanning localhost.\n", PRGNAME);
        return 1;
    }

    signal(SIGINT, handle_interrupt);

    if (!command_exists("sslscan")) {
        fprintf(stderr, "Please install 'sslscan' first (part of EPEL)!\n");
        return 1;
    }

    if (is_local) {
        /* Local scan: discover listening ports via netstat */
        if (!command_exists("ss")) {
            fprintf(stderr, "Please install 'ss' first (part of iproute package)!\n");
            return 1;
        }

        if (!command_exists("lsof")) {
            fprintf(stderr, "Please install 'lsof' first (part of lsof package)!\n");
            return 1;
        }

        printf("Scanning localhost for services using TLSv1.0 or TLSv1.1...\n\n");

        FILE *fp = popen("netstat -an | grep LISTEN | grep -v LISTENING | grep -v tcp6 | awk '{print $4}'", "r");
        if (!fp) {
            perror("popen");
            return 1;
        }

        char host_port[256];
        while (fgets(host_port, sizeof(host_port), fp)) {
            host_port[strcspn(host_port, "\n")] = '\0';
            if (host_port[0] == '\0')
                continue;

            /* Split "host:port" on the last colon */
            char *last_colon = strrchr(host_port, ':');
            if (!last_colon)
                continue;

            char host[128];
            char port[32];
            size_t host_len = (size_t)(last_colon - host_port);
            if (host_len >= sizeof(host))
                host_len = sizeof(host) - 1;
            memcpy(host, host_port, host_len);
            host[host_len] = '\0';
            strncpy(port, last_colon + 1, sizeof(port) - 1);
            port[sizeof(port) - 1] = '\0';

            if (!is_safe_host(host) || !is_numeric(port)) {
                fprintf(stderr, "Skipping suspicious host:port entry: %s\n", host_port);
                continue;
            }

            printf("**** Scanning for TLS in IP:port %s ****\n", host_port);

            char sslscan_cmd[512];
            snprintf(sslscan_cmd, sizeof(sslscan_cmd),
                     "timeout 10 sslscan %s:%s | grep -E '(TLSv1.0|TLSv1.1)' | grep enable",
                     host, port);

            FILE *scan_fp = popen(sslscan_cmd, "r");
            if (!scan_fp)
                continue;

            char scan_line[512];
            int found = 0;
            while (fgets(scan_line, sizeof(scan_line), scan_fp)) {
                printf("%s", scan_line);
                found = 1;
            }
            pclose(scan_fp);

            if (found) {
                printf("What is behind port %s?\n", port);

                char ss_cmd[256];
                snprintf(ss_cmd, sizeof(ss_cmd), "ss -tnlp | grep ':%s '", port);
                FILE *ss_fp = popen(ss_cmd, "r");
                if (ss_fp) {
                    char ss_line[512];
                    char pid_str[64] = "";
                    while (fgets(ss_line, sizeof(ss_line), ss_fp)) {
                        printf("%s", ss_line);
                        /* Extract PID from "pid=NNN" in ss output */
                        char *pid_ptr = strstr(ss_line, "pid=");
                        if (pid_ptr && pid_str[0] == '\0') {
                            pid_ptr += 4;
                            size_t i = 0;
                            while (i < sizeof(pid_str) - 1 && pid_ptr[i] &&
                                   pid_ptr[i] != ',' && pid_ptr[i] != ')') {
                                pid_str[i] = pid_ptr[i];
                                i++;
                            }
                            pid_str[i] = '\0';
                        }
                    }
                    pclose(ss_fp);

                    if (pid_str[0] && is_numeric(pid_str)) {
                        printf("Which program is using PID %s?\n", pid_str);
                        char lsof_cmd[128];
                        snprintf(lsof_cmd, sizeof(lsof_cmd), "lsof -p %s | head -2", pid_str);
                        FILE *lsof_fp = popen(lsof_cmd, "r");
                        if (lsof_fp) {
                            char lsof_line[512];
                            while (fgets(lsof_line, sizeof(lsof_line), lsof_fp))
                                printf("%s", lsof_line);
                            pclose(lsof_fp);
                        }
                    }
                }
                printf("------------------------------------------------------------------------------------\n");
            }
        }

        pclose(fp);
    } else {
        /* Remote scan: discover open ports via nmap, then sslscan each */
        if (!command_exists("nmap")) {
            fprintf(stderr, "Please install 'nmap' first (required for remote host scanning)!\n");
            return 1;
        }

        printf("Scanning remote host %s for services using TLSv1.0 or TLSv1.1...\n\n", target_host);

        char nmap_cmd[512];
        snprintf(nmap_cmd, sizeof(nmap_cmd),
                 "nmap --open -p 1-65535 -T4 -- '%s' 2>/dev/null | grep '^[0-9]' | awk -F'/' '{print $1}'",
                 target_host);

        FILE *fp = popen(nmap_cmd, "r");
        if (!fp) {
            perror("popen");
            return 1;
        }

        char port[32];
        while (fgets(port, sizeof(port), fp)) {
            port[strcspn(port, "\n")] = '\0';
            if (port[0] == '\0')
                continue;

            if (!is_numeric(port)) {
                fprintf(stderr, "Skipping unexpected nmap output: %s\n", port);
                continue;
            }

            printf("**** Scanning for TLS in %s:%s ****\n", target_host, port);

            char sslscan_cmd[512];
            snprintf(sslscan_cmd, sizeof(sslscan_cmd),
                     "timeout 10 sslscan '%s':%s | grep -E '(TLSv1.0|TLSv1.1)' | grep enable",
                     target_host, port);

            FILE *scan_fp = popen(sslscan_cmd, "r");
            if (!scan_fp)
                continue;

            char scan_line[512];
            int found = 0;
            while (fgets(scan_line, sizeof(scan_line), scan_fp)) {
                printf("%s", scan_line);
                found = 1;
            }
            pclose(scan_fp);

            if (found)
                printf("------------------------------------------------------------------------------------\n");
        }

        pclose(fp);
    }

    return 0;
}
