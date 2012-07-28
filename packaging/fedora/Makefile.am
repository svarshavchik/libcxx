#
# Copyright 2012 Double Precision, Inc.
# See COPYING for distribution information.
#

noinst_DATA=systemd
EXTRA_DIST=libcxx.spec systemd.in
CLEANFILES=systemd

systemd: systemd.in Makefile
	sed 's~@''datadir''@~${datadir}~g' <$< >$@.tmp
	mv $@.tmp $@

