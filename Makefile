CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGET  = TLSv1-scan
VERSION = 1.0
SPECFILE = packaging/rpm/$(TARGET).spec
DISTDIR  = dist

all: $(TARGET)

$(TARGET): TLSv1-scan.c
	$(CC) $(CFLAGS) -o $(TARGET) TLSv1-scan.c

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/sbin/$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(DISTDIR)/rpmbuild

rpm: $(TARGET)
	mkdir -p $(DISTDIR)/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
	tar --transform 's,^,$(TARGET)-$(VERSION)/,' \
	    -czf $(DISTDIR)/rpmbuild/SOURCES/$(TARGET)-$(VERSION).tar.gz \
	    $(TARGET).c $(TARGET).sh Makefile LICENSE README.md
	cp $(SPECFILE) $(DISTDIR)/rpmbuild/SPECS/
	rpmbuild -ba \
	    --define "_topdir $(CURDIR)/$(DISTDIR)/rpmbuild" \
	    $(DISTDIR)/rpmbuild/SPECS/$(TARGET).spec
	find $(DISTDIR)/rpmbuild/RPMS $(DISTDIR)/rpmbuild/SRPMS -name '*.rpm' \
	    -exec cp {} $(DISTDIR)/ \;

.PHONY: all install clean rpm
