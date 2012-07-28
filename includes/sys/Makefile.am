#
# Copyright 2012 Double Precision, Inc.
# See COPYING for distribution information.
#

headerdir=$(includedir)/x/sys

header_HEADERS = $(shell cd $(srcdir) && ls *.h 2>/dev/null)

EXTRA_HEADERS=

dist-hook:
	rm -f $(distdir)/*.h
