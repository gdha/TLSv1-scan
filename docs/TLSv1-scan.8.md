---
title: TLSV1-SCAN
section: 8
date: March 2026
header: System Administration Commands
footer: TLSv1-scan 1.0
---

# NAME

TLSv1-scan - scan local system for services using deprecated TLSv1.0 or TLSv1.1

# SYNOPSIS

**TLSv1-scan**

# DESCRIPTION

**TLSv1-scan** enumerates all local TCP listening ports using **netstat**(8),
then probes each port with **sslscan**(1) to detect whether TLSv1.0 or
TLSv1.1 is enabled.  For every port where a deprecated protocol version is
found, it identifies the owning process with **ss**(8) and **lsof**(8) and
prints the result to standard output.

The program must be run as **root**.

# EXIT STATUS

**0**
: All ports were scanned successfully (some may have had deprecated TLS
  versions enabled).

**1**
: The program was not run as root, a required external tool (**sslscan**,
  **ss**, or **lsof**) was not found, or a fatal error occurred during
  execution.

# DEPENDENCIES

The following tools must be installed and available in **PATH**:

**sslscan**(1)
: Part of EPEL on RHEL-based systems; the **sslscan** Debian/Ubuntu package.

**ss**(8)
: Part of the **iproute** (RHEL) / **iproute2** (Debian/Ubuntu) package.

**lsof**(8)
: Provided by the **lsof** package.

**netstat**(8)
: Part of the **net-tools** package.

# EXAMPLES

Run the scanner as root:

    sudo TLSv1-scan

# FILES

**/usr/sbin/TLSv1-scan**
: The installed program binary.

# NOTES

TLSv1.0 and TLSv1.1 are deprecated by RFC 8996.  Their use exposes services
to known attacks such as BEAST and POODLE.  Hosts on which this scanner
reports enabled deprecated versions should be reconfigured to support
TLSv1.2 or TLSv1.3 only.

# BUGS

Only IPv4 TCP listening ports are examined.  IPv6 listeners and UDP services
are not scanned.

# SEE ALSO

**sslscan**(1), **ss**(8), **lsof**(8), **netstat**(8)

# AUTHORS

Gratien D'haese \<gratien.dhaese@gmail.com\>
