# TLSv1-scan
Program to scan system on TLSv1.0 and TLSv1.1 usage

## Prerequisites

The following tools must be installed and available in `PATH`:

- `sslscan` (part of EPEL on RHEL-based systems)
- `ss` (part of the `iproute` package)
- `lsof` (part of the `lsof` package)
- `netstat` (part of the `net-tools` package)

## Build

```bash
make
```

## Install

```bash
sudo make install
```

This installs the binary to `/usr/sbin/TLSv1-scan`.

## Usage

The program must be run as root:

```bash
sudo TLSv1-scan
```

It scans all local TCP listening ports, checks each one for TLSv1.0 or TLSv1.1
support using `sslscan`, and reports the owning process for any service where
those deprecated protocol versions are enabled.
