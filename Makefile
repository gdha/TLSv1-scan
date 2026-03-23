CC      = gcc
CFLAGS  = -Wall -Wextra -O2
TARGET  = TLSv1-scan
VERSION = 1.0
SPECFILE  = packaging/rpm/$(TARGET).spec
DEBDIR    = packaging/debian
DISTDIR   = dist
MANPAGE   = $(TARGET).8
PKGSRCS   = $(TARGET).c Makefile LICENSE README.md $(TARGET).8.ronn

all: $(TARGET)

$(TARGET): TLSv1-scan.c
	$(CC) $(CFLAGS) -o $(TARGET) TLSv1-scan.c

man: $(MANPAGE)

$(MANPAGE): $(TARGET).8.ronn
	ronn --roff $<

install: $(TARGET) man
	install -m 755 $(TARGET) /usr/sbin/$(TARGET)
	install -D -m 644 $(MANPAGE) /usr/share/man/man8/$(MANPAGE)

clean:
	rm -f $(TARGET)
	rm -f $(MANPAGE)
	rm -rf $(DISTDIR)/rpmbuild
	rm -rf $(DISTDIR)/debbuild
	rm -f dist/$(TARGET)-$(VERSION).tar.gz

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

.PHONY: all man install clean rpm deb
