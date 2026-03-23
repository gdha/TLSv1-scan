CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGET  = TLSv1-scan

all: $(TARGET)

$(TARGET): TLSv1-scan.c
	$(CC) $(CFLAGS) -o $(TARGET) TLSv1-scan.c

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/sbin/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install clean
