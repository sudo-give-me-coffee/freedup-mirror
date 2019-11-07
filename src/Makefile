#!/usr/bin/make -f
#
# This file is part of FreeDup.
#
#    FreeDup -- search for duplicate files in one or more directory hierarchies
#    Copyright (C) 2007,2008 Andreas Neuper
#
#    FreeDup is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Foobar is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with FreeDup.  If not, see <http://www.gnu.org/licenses/>.
#
SUFFIX=.gz .bz2 .tar.bz2 .tgz .tar .lint
.PHONY: distclean clean rpm hashtest tarball test

#
# OS specific definitions
#
#the following two lines will most likely fail for non-GNU make
#in this case please replace the lines by the contents of the included file
include Makefile.$(shell uname -s)
HTML2TEXT = /usr/bin/html2text
CFLAGS=-g3 -Wall -pedantic -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 $(DEFS) -std=gnu99 -O0
CFLAGS=-O2 -Wall -pedantic -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 $(DEFS) -std=gnu99

#
# PREFIX indicates directory tree where program is to be installed.
# Suggested values are "/usr/local" or "/usr".
# The variable used for execution ist DESTDIR only (needed by debian)
#
INSTALL = install
INSTALLDIR = /
PREFIX = /usr
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib
MANDIR = $(PREFIX)/share/man
INSTBINMODE = 755
INSTDOCMODE = 755
INSTALLOWN = root
INSTALLGRP = root
DESTDIR=$(INSTALLDIR)


#
# VERSION determines the program's version number.
#
VERSION = "$$(/bin/cat version)"
MAKFILES= Makefile Makefile.tests Makefile.Linux Makefile.Darwin \
	  Makefile.CYGWIN_NT-5.1 Makefile.AIX Makefile.BSD
SRCFILES= freedup.c freedup.h sha1.c sha1.h config.c config.h \
	  my.c mp3.c mp4.c mpc.c jpg.c ogg.c auto.c \
	  my.h mp3.h mp4.h mpc.h jpg.h ogg.h auto.h \
	  web.c web.h socket.c socket.h helper.h nethelp.h \
	  symharden.c
TARFILES= $(MAKFILES) demo $(SRCFILES) README.SHA README.html COPYING.SHA \
	  freedup freedup.1 freedup.spec freedup.xinetd symharden symharden.1 \
	  ChangeLog TODO README COPYING rules copyright verify pad.xml debian
HTMLFILES=intro.html syntax.html download.html process.html \
	  examples.html hint.html questions.html offer.html
DOCFILES= freedup.css index.html foot.html head.html $(HTMLFILES)
OBJFILES= freedup.o config.o sha1.o mp3.o mp4.o mpc.o jpg.o my.o ogg.o auto.o web.o socket.o 

#
# ALL that needs/should be done
#
all:	freedup test

tags:	$(SRCFILES)
	ctags $(SRCFILES)
#
# ALL that needs/should be done
#
test:	Makefile.tests stest
	$(MAKE) -f $<

stest: hashtest	shatest

verify:	Makefile Makefile.$(shell uname -s)
	@echo "#!$$(which make) -f " >verify
	@sed "s:./freedup:$(DESTDIR)/freedup:" Makefile.$(shell uname -s) >>verify
	@sed -e '/^include/d' -e 's/:[ 	]*freedup/:/' Makefile.tests >>verify
	@chmod a+x verify

#
# INSTALLATION without using RPM
#
install: freedup freedup.1.gz symharden symharden.1.gz
	[ "./freedup" = $(FREEDUPEXE) ] || cp $(FREEDUPEXE) freedup
	$(INSTALL) -d $(DESTDIR)
	$(INSTALL) -d $(DESTDIR)/$(BINDIR)
	$(INSTALL) -p -s -m $(INSTBINMODE) freedup   $(DESTDIR)/$(BINDIR)/freedup
	$(INSTALL) -p -s -m $(INSTBINMODE) symharden $(DESTDIR)/$(BINDIR)/symharden
	$(INSTALL) -d $(DESTDIR)/$(MANDIR)/man1
	$(INSTALL) -p -m $(INSTDOCMODE) freedup.1.gz   $(DESTDIR)/$(MANDIR)/man1/
	$(INSTALL) -p -m $(INSTDOCMODE) symharden.1.gz $(DESTDIR)/$(MANDIR)/man1/

#
# FREEDUP direct dependencies
#
freedup.o: freedup.c freedup.h version.h mp3.h jpg.h my.h ogg.h
freedup: $(OBJFILES)


#
# VERSION is now only specified in freedup.spec
# this action generates a reliable "version" file
# that will be used troughout this package
#
version: freedup.spec
	awk '{if($$1=="%define"){if($$2=="version")v=$$3;if($$2=="release")r=$$3;}};END{printf"%s-%s",v,r;}' \
	freedup.spec >version

version.h: version
	$(ECHOE) "#define VERSION\t\"$$(/bin/cat version)\"" >version.h

#
# An attempt to produce a single file documentation
#
README.html: head.html $(DOCFILES)
	-@rm -f $@
	sed -n '1,/<.BODY>/p' head.html >$@
	for n in $(HTMLFILES); do \
	sed -n '/<BODY/,/<.BODY>/p' $$n | grep -v 'BODY' >>$@ ; done
	$(ECHOE) "</BODY>\n</HTML>">>$@

README:	README.html
	@echo "Converting $< into a plain text file called $@."
	@if [ -x $(HTML2TEXT) ] ; then \
	$(ECHOE) " FFFFF RRRRR   EEEEE  EEEEE  DDDD   UU UU  PPPPP" >$@ ;\
	$(ECHOE) " FF    RR  RR  EE     EE     DD DD  UU UU  PP  PP" >>$@ ;\
	$(ECHOE) " FFFF  RRRR    EEEEE  EEEEE  DD DD  UU UU  PPPPP" >>$@ ;\
	$(ECHOE) " FF    RR RR   EE     EE     DD DD  UU UU  PP" >>$@ ;\
	$(ECHOE) " FF    RR  RR  EEEEE  EEEEE  DDDD    UUU   PP" >>$@ ;\
	$(HTML2TEXT) -nobs $< |\
	sed -e '1,3d' -e '5s:FREEDUP *::' -e '6s: :- :g' \
	    -e 's:\*\{5\}\(.*\)\*\{5\}:###############    \1##########:' \
	    -e 's:\*\{4\}\(.*\)\*\{4\}:\*\*\*\*\*\*\*\*\*\* \1\*\*\*\*\*:' |\
	tr '\015' '\012' >> $@ ;\
	else echo "see README.html">$@ ;fi

changelog: freedup.spec version
	(echo "freedup ($$(/bin/cat version)) unstable; urgency=low"; echo) >changelog
	@sed -n '/^%changelog/,$$p' freedup.spec | grep -v '^%changelog' |\
	awk '{if($$1=="*")author="\n -- "$$6" "$$7" "$$8"  "$$2", "$$4" "$$3" "$$5" 06:00:00 +0100\n"; if($$1=="-" || $$1=="+") print " "$$0"."; if(NF==0){print author}}' |\
	sed -e 's: - :  * :g'>>changelog

ChangeLog: freedup.spec
	sed -n '/^%changelog/,$$p' freedup.spec | grep -v '^%changelog' >ChangeLog


pad.xml: pad.xms
	@sed -e s:VERSION:$$(/bin/cat version): -e s=VERYEAR=$$(date +%Y)= -e s=VERMONTH=$$(date +%m)= -e s=VERDAY=$$(date +%d)= -e s=VERSIZE=$$(ls -l attic/freedup-$$(/bin/cat version)-src.tar.bz2 | awk '{print $$5}')= -e s=VERKSIZE=$$(ls -l attic/freedup-$$(/bin/cat version)-src.tar.bz2 | awk '{printf"%d",$$5/1024}')= -e s=VERMSIZE=$$(ls -l attic/freedup-$$(/bin/cat version)-src.tar.bz2 | awk '{printf"%3.3f",$$5/1024/1024}')= $< >$@

#
# automatically generate all published file formats
#
%.tar.bz2:	%.tgz
	gzip -cd $< | bzip2 -c >$@

%.tgz:	%.tar.bz2
	bzip2 -cd $< | gzip -c >$@

%.html:	html/%.html
	ln $< $@

%.css:	html/%.css
	ln $< $@

%.gz:	%
	gzip -c $< >$@

%.html: %
	@echo "Generating $@ from plain text file $<."
	@($(ECHOE) "<?XML version=\"1.0\" encoding=\"utf-8\"?>\n<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n<HTML lang=\"en\" xml:lang=\"en\" xmlns=\"http://www.w3.org/1999/xhtml\">\n<HEAD>\n<TITLE>Freedup $< </TITLE>\n<META HTTP-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n<META NAME=\"KEYWORDS\" content=\"Freedup, systems management, tool, link, $<\" />\n<META HTTP-EQUIV=\"expires\" content=\"Fri, 31 Dec 2010 19:00:00 CET\" />\n<LINK REL=\"stylesheet\" type=\"text/css\" href=\"freedup.css\" />\n</HEAD>\n<BODY BGCOLOR=\"#DDDDFF\">\n<PRE>" ; sed -e 's:^$$:</PRE><P><PRE>:' $< ; $(ECHOE) "</PRE>\n</BODY>\n</HTML>") >$@

#
# generate the setup.hint file for CYGWIN automatically
#
SDESC=reduces used disk space by linking duplicate files while being able to ignore tags.
LDESC=Freedup finds and eliminates duplicate files by linking them (aka deduplication), and thus reduces the amount of used disk space within one or more file systems. By default, hardlinks are used on a single device, symbolic links when the devices differ. A set of options allows you to modify the methods of file comparison, the hash functions, the linking behavior, and the reporting style. You may use batch or interactive mode. Freedup usually only considers identical files, but when comparing audio or graphics files, you may elect to ignore the tags.

setup.hint: $(MAKFILES) version freedup.spec
	@echo "# This file is generated by make" >setup.hint
	@echo "category: System Utils" >>setup.hint
	@echo "requires: cygwin" >>setup.hint
	@echo "sdesc: Freedup $(SDESC)" >>setup.hint
	@echo "ldesc: $(LDESC)" >>setup.hint
	@echo "curr: $$(/bin/cat version)" >>setup.hint

DEBIAN=	control compat changelog
#debian: $(DEBIAN) rules copyright
#	@mkdir -p $@
#	mv $(DEBIAN) $@
#	cp rules copyright $@
#	dpkg-buildpackage -d
#
#control:
#	@echo "Source: freedup" >control
#	@echo "Section: utils" >>control
#	@echo "Priority: optional" >>control
#	@echo "Maintainer: AN <AN@freedup.org>" >>control
#	@echo "Build-Depends: debhelper (>= 3.0.0)" >>control
#	@echo "Standards-Version: 3.5.2" >>control
#	@echo "" >>control
#	@echo "Package: freedup" >>control
#	@echo 'Depends: $${shlibs:Depends}, $${misc:Depends}' >>control
#	@echo "Architecture: $$(uname -i)" >>control
#	@echo "Description: $(SDESC)" >>control
#	@echo " $(LDESC)" >>control
#
#compat:
#	echo "4" >compat

#
# generate the distribution tarballs
# (avoid calling this - use make rpm instead)
#
tarball:	freedup version README ChangeLog $(TARFILES)
	[ "./freedup" = $(FREEDUPEXE) ] || cp $(FREEDUPEXE) freedup
	strip freedup
	if [ -d freedup-$$(/bin/cat version|tr - -) ] ; then exit -1 ;fi
	mkdir freedup-$$(/bin/cat version|tr - -)
	cp -r $(TARFILES) freedup-$$(/bin/cat version|tr - -)
	mkdir freedup-$$(/bin/cat version|tr - -)/html
	cp $(DOCFILES) freedup-$$(/bin/cat version|tr - -)/html
	tar czf freedup-$$(/bin/cat version)-orig.tar.gz \
	freedup-$$(/bin/cat version|tr - -)
	tar cjf freedup-$$(/bin/cat version)-src.tar.bz2 \
	freedup-$$(/bin/cat version|tr - -)
	ln freedup-$$(/bin/cat version)-orig.tar.gz \
	freedup_$$(/bin/cat version|cut -d- -f1).orig.tar.gz
	@echo "\n---Debian systems may fail during the next steps---\n\n"
	-[ -d $(RPMBUILD)/SOURCES ] &&	\
	tar cjf $(RPMBUILD)/SOURCES/freedup-$$(/bin/cat version)-src.tar.bz2 \
	freedup-$$(/bin/cat version|tr - -)
	-[ -d $(RPMBUILD)/SOURCES ] &&	\
	tar czf $(RPMBUILD)/SOURCES/freedup-$$(/bin/cat version)-src.tgz \
	freedup-$$(/bin/cat version|tr - -)

#
# generate the own distribution RPM file
#
rpm:	tarball verify
	rpmbuild -ba --sign freedup.spec
	cp $(RPMBUILD)/RPMS/i586/freedup-$$(/bin/cat version)* .
	cp $(RPMBUILD)/SRPMS/freedup-$$(/bin/cat version)* .

#
# switching gui mode on and off
#
webon:	
	-@[ -f /etc/services ] && $$(grep -q freedup /etc/services) || echo -e "freedup\t\t31166/tcp" >>//etc/services
	-@[ -f /etc/inetd.conf ] && $$(grep -q freedup /etc/inetd.conf) || echo "freedup stream  tcp nowait root /usr/bin/freedup freedup -W" >>/etc/inetd.conf
	-@[ -f /etc/inetd.conf ] && $$(grep -q freedup /etc/inetd.conf) && cp /etc/inetd.conf /tmp && sed 's:^.*freedup.*stream:freedup stream:' /tmp/inetd.conf >/etc/inetd.conf && rm /tmp/inetd.conf
	-@[ -d /etc/xinetd.d -a ! -f /etc/xinetd.d/freedup ] && echo "service freedup\n{\n\tport\t= 31166\n\tsocket_type\t= stream\n\tprotocol\t= tcp\n\twait\t= no\n\tuser\t= root\n\tdisable\t= no\n\tserver\t= /home/aneuper/src/freedup/freedup\n\tserver_args\t= -W\n}" >>/etc/xinetd.d/freedup && chmod 644 /etc/xinetd.d/freedup || echo  -e '\c'
	-@[ -d /etc/xinetd.d -a -f /etc/xinetd.d/freedup ] && mv /etc/xinetd.d/freedup /tmp/freedup.off && sed 's:disable.*:disable\t= no:' /tmp/freedup.off >/etc/xinetd.d/freedup && chmod 644 /etc/xinetd.d/freedup

weboff:
	-@[ -f /etc/services ] && $$(grep -q freedup /etc/services) || echo -e "freedup\t\t31166/tcp" >>//etc/services
	-@[ -d /etc/xinetd.d -a -f /etc/xinetd.d/freedup ] && mv  /etc/xinetd.d/freedup /tmp/freedup.on && sed 's:disable.*:disable\t= yes:' /tmp/freedup.on >/etc/xinetd.d/freedup && chmod 644 /etc/xinetd.d/freedup && rm /tmp/freedup.on
	-@[ -f /etc/inetd.conf ] && $$(grep -q freedup /etc/inetd.conf) && cp /etc/inetd.conf /tmp && sed 's:^.*freedup.*stream:# freedup stream:' /tmp/inetd.conf >/etc/inetd.conf && rm /tmp/inetd.conf

state:
	-@[ -f /etc/services ] && $$(grep -q freedup /etc/services >/dev/null) && echo -e "freedup service is defined to port \c" && awk '{if($$1=="freedup"&&x==0){split($$2,a,"/");print a[1];x=1;}}' /etc/services || echo "freedup service is not defined"
	-@[ -d /etc/xinetd.d -a -f /etc/xinetd.d/freedup ] && sed -n -e 's/.*disable.*=.*[yY][eE][sS].*/freedup support for xinetd is disabled./p' -e 's/.*disable.*=.*[nN][oO].*/freedup support for xinetd is enabled./p' /etc/xinetd.d/freedup
	-@[ -f /etc/inetd.conf ] && $$(grep -q freedup /etc/inetd.conf) && sed -n -e 's:^[^#]*freedup.*:freedup support for inetd is enabled:p' -e 's:^ *#.*freedup.*:freedup support for inetd is disabled:p' /etc/inetd.conf || echo "freedup support for inetd is not configured."


#
# some sanity run definitions
#
clean:	
	rm -f $(OBJFILES) freedup freedup.1.gz symharden symharden.1.gz \
		ChangeLog ChangeLog.html changelog TODO.html \
		setup.hint version.h control core compat

distclean: clean
	rm -f freedup-$$(/bin/cat version)-src.tar.bz2 freedup-$$(/bin/cat version)-src.tgz
	rm -fr freedup-$$(/bin/cat version|cut -d- -f1) version

#
# development tests
#
oggtest: ogg.c
	@gcc -Wall -g3 -DOGG_TEST -o oggtest ogg.c

jpgtest: jpg.c
	@gcc -Wall -g3 -DJPG_TEST -o jpgtest jpg.c

mp3test: mp3.c
	@gcc -Wall -g3 -DMP3_TEST -o mp3test mp3.c

mp4test: mp4.c
	@gcc -Wall -g3 -DMP4_TEST -o mp4test mp4.c

mpctest: mpc.c
	@gcc -Wall -g3 -DMPC_TEST -o mpctest mpc.c

autotest.o: auto.c
	gcc -Wall -g3 -DAUTO_TEST -o autotest.o -c auto.c

autotest: autotest.o mp3.o mp4.o mpc.o jpg.o ogg.o my.o 
	gcc -Wall -g3 -o autotest  autotest.o mp3.o mp4.o mpc.o jpg.o ogg.o my.o

sha1test: sha1.c
	@gcc -Wall -g3 -DSHA1_TEST -o sha1test sha1.c

shatest: sha1test
	@mkdir -p test11
	@$(ECHOE) "Test prepared:\t\tHash Method - internal: SHA1 by A.Saddi\t\c"
	@$(SHATESTEXE) >test11/output
	@$(ECHOE) "a9993e36 4706816a ba3e2571 7850c26c 9cd0d89d" \
		  "\n84983e44 1c3bd26e baae4aa1 f95129e5 e54670f1" \
		  "\n34aa973c d4c4daa4 f61eeb2b dbad2731 6534016f ">test11/expected
	@diff test11/output test11/expected >/dev/null && \
	 echo "[32msuccessful[0m" || echo "[31mfailed[0m"
	@rm -r test11 $(SHATESTEXE)

%.lint: %.c
	@splint -exportlocal -warnposix $< 2>&1 | tee $*.lint |\
	sed -e 's:\(no warnings\):[32m\1[0m:' -e 's:\(...code warning\):[31m\1[0m:'

hashtest:
	@sum="$$(grep '^#.*define *SHA512SUM' freedup.h|cut -d\" -f2)" ; if [ -n "$$sum" ] ; then\
	 if [ -x $$sum ] ; then $(ECHOE) "External program\t\"$$sum\" [32m\t is present and executable[0m" ; else\
	 $(ECHOE) "External program\t\"$$sum\" [31m\t is missing/not executable[0m " ; fi  ;fi
	@sum="$$(grep '^#.*define *SHA384SUM' freedup.h|cut -d\" -f2)" ; if [ -n "$$sum" ] ; then\
	 if [ -x $$sum ] ; then $(ECHOE) "External program\t\"$$sum\" [32m\t is present and executable[0m" ; else\
	 $(ECHOE) "External program\t\"$$sum\" [31m\t is missing/not executable[0m " ; fi ; fi
	@sum="$$(grep '^#.*define *SHA256SUM' freedup.h|cut -d\" -f2)" ; if [ -n "$$sum" ] ; then\
	 if [ -x $$sum ] ; then $(ECHOE) "External program\t\"$$sum\" [32m\t is present and executable[0m" ; else\
	 $(ECHOE) "External program\t\"$$sum\" [31m\t is missing/not executable[0m " ; fi ; fi
	@sum="$$(grep '^#.*define *SHA224SUM' freedup.h|cut -d\" -f2)" ; if [ -n "$$sum" ] ; then\
	 if [ -x $$sum ] ; then $(ECHOE) "External program\t\"$$sum\" [32m\t is present and executable[0m" ; else\
	 $(ECHOE) "External program\t\"$$sum\" [31m\t is missing/not executable[0m " ; fi ; fi
	@sum="$$(grep '^#.*define *SHA1SUM' freedup.h|cut -d\" -f2)" ; if [ -n "$$sum" ] ; then\
	 if [ -x $$sum ] ; then $(ECHOE) "External program\t\"$$sum\" [32m\t is present and executable[0m" ; else\
	 $(ECHOE) "External program\t\"$$sum\" [31m\t is missing/not executable[0m " ; fi ; fi
	@sum="$$(grep '^#.*define *MD5SUM' freedup.h|cut -d\" -f2)" ; if [ -n "$$sum" ] ; then\
	 if [ -x $$sum ] ; then $(ECHOE) "External program\t\"$$sum\" [32m\t is present and executable[0m" ; else\
	 $(ECHOE) "External program\t\"$$sum\" [31m\t is missing/not executable[0m " ; fi ; fi
	@sum="$$(grep '^#.*define *SUM' freedup.h|cut -d\" -f2)" ; if [ -n "$$sum" ] ; then\
	 if [ -x $$sum ] ; then $(ECHOE) "External program\t\"$$sum\" [32m\t\t is present and executable[0m" ; else\
	 $(ECHOE) "External program\t\"$$sum\" [31m\t\t is missing/not executable[0m " ; fi ; fi

