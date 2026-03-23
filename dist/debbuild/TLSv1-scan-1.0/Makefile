CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGET  = TLSv1-scan
VERSION = 1.0
SPECFILE  = packaging/rpm/$(TARGET).spec
DEBDIR    = packaging/debian
DISTDIR   = dist
PKGSRCS   = $(TARGET).c $(TARGET).sh Makefile LICENSE README.md

all: $(TARGET)

$(TARGET): TLSv1-scan.c
	$(CC) $(CFLAGS) -o $(TARGET) TLSv1-scan.c

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/sbin/$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(DISTDIR)/rpmbuild
	rm -rf $(DISTDIR)/debbuild

rpm: $(TARGET)
	mkdir -p $(DISTDIR)/rpmbuild/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
	tar --transform 's,^,$(TARGET)-$(VERSION)/,' \
	    -czf $(DISTDIR)/rpmbuild/SOURCES/$(TARGET)-$(VERSION).tar.gz \
	    $(PKGSRCS)
	cp $(SPECFILE) $(DISTDIR)/rpmbuild/SPECS/
	rpmbuild -ba \
	    --define "_topdir $(CURDIR)/$(DISTDIR)/rpmbuild" \
	    $(DISTDIR)/rpmbuild/SPECS/$(TARGET).spec
	find $(DISTDIR)/rpmbuild/RPMS $(DISTDIR)/rpmbuild/SRPMS -name '*.rpm' \
	    -exec cp {} $(DISTDIR)/ \;

deb:
	mkdir -p $(DISTDIR)/debbuild/$(TARGET)-$(VERSION)
	cp $(PKGSRCS) \
	    $(DISTDIR)/debbuild/$(TARGET)-$(VERSION)/
	cp -r $(DEBDIR) $(DISTDIR)/debbuild/$(TARGET)-$(VERSION)/debian
	cd $(DISTDIR)/debbuild/$(TARGET)-$(VERSION) && \
	    dpkg-buildpackage -us -uc -b
	find $(DISTDIR)/debbuild -maxdepth 1 -name '*.deb' \
	    -exec cp {} $(DISTDIR)/ \;

.PHONY: all install clean rpm deb
