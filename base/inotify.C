/*
** Copyright 2015-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/inotify.H"
#include "x/sysexception.H"
#include "x/weakmultimap.H"
#include "x/logger.H"
#include "x/functionalrefptr.H"

#include <sys/inotify.h>
#include <limits.h>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

const uint32_t inotify_access=IN_ACCESS;
const uint32_t inotify_attrib=IN_ATTRIB;
const uint32_t inotify_close_write=IN_CLOSE_WRITE;
const uint32_t inotify_close_nowrite=IN_CLOSE_NOWRITE;
const uint32_t inotify_create=IN_CREATE;
const uint32_t inotify_delete=IN_DELETE;
const uint32_t inotify_delete_self=IN_DELETE_SELF;
const uint32_t inotify_modify=IN_MODIFY;
const uint32_t inotify_move_self=IN_MOVE_SELF;
const uint32_t inotify_moved_from=IN_MOVED_FROM;
const uint32_t inotify_moved_to=IN_MOVED_TO;
const uint32_t inotify_open=IN_OPEN;
const uint32_t inotify_move=IN_MOVE;
const uint32_t inotify_close=IN_CLOSE;

#ifndef IN_DONT_FOLLOW
#define IN_DONT_FOLLOW 0
#endif
#ifndef IN_EXCL_UNLINK
#define IN_EXCL_UNLINK 0
#endif
#ifndef IN_ONLYDIR
#define IN_ONLYDIR 0
#endif
#ifndef IN_IGNORED
#define IN_IGNORED 0
#endif
#ifndef IN_ISDIR
#define IN_ISDIR 0
#endif
#ifndef IN_UNMOUNT
#define IN_UNMOUNT 0
#endif

const uint32_t inotify_dont_follow=IN_DONT_FOLLOW;
const uint32_t inotify_excl_unlink=IN_EXCL_UNLINK;
const uint32_t inotify_onlydir=IN_ONLYDIR;
const uint32_t inotify_ignored=IN_IGNORED;
const uint32_t inotify_isdir=IN_ISDIR;
const uint32_t inotify_unmount=IN_UNMOUNT;

static int create_inotify()
{
#ifdef IN_NONBLOCK
	int fd=inotify_init1(IN_NONBLOCK);
#else
	int fd=inotify_init();
#endif

	if (fd < 0)
		throw SYSEXCEPTION("inotify_init");

	return fd;
}

#pragma GCC visibility push(hidden)
class inotifyObj::watcherObj : virtual public obj {

 public:
	const inotify i;
	const int w;
	const watch_callback_t cb;

	watcherObj(inotify && iArg,
		   const std::string &pathname,
		   uint32_t mask,
		   const watch_callback_t &cbArg)
		: i(std::move(iArg)),
		  w(inotify_add_watch(i->get_fd(), pathname.c_str(),
				      mask)),
		  cb(cbArg)
	{
		if (w < 0)
			throw SYSEXCEPTION("inotify_add_watch: "
					   + pathname);
	}

	~watcherObj()
	{
		inotify_rm_watch(i->get_fd(), w);
	}
};
#pragma GCC visibility pop

inotifyObj::inotifyObj()
	: fdObj(create_inotify()), watchers(watchers_t::create())
{
}

inotifyObj::~inotifyObj()
{
}

ref<obj> inotifyObj::create(const std::string &pathname,
			       uint32_t mask,
			       const watch_callback_t &functor)
{
	auto w=ref<watcherObj>::create(inotify(this), pathname, mask,
				       functor);

	watchers->insert(w->w, w);

	return w;
}

LOG_FUNC_SCOPE_DECL(LIBCXX_NAMESPACE "inotifyObj::read", readLogger);

void inotifyObj::read()
{
	LOG_FUNC_SCOPE(readLogger);

	const size_t buffer_size=sizeof(struct inotify_event) + NAME_MAX + 1;
	char buffer[buffer_size];

	auto s=fdObj::read(buffer, buffer_size);
	const char *p=buffer;

	while (s >= sizeof(struct inotify_event)) // GIGO
	{
		const struct inotify_event *e=
			reinterpret_cast<const struct inotify_event
					 *>(p);

		size_t n=sizeof(struct inotify_event)+e->len;

		if (n > s)
			n=s; // GIGO

		p += n;
		s -= n;

		ptr<watcherObj> w;

		{
			auto range=watchers->equal_range(e->wd);

			while (range.first != range.second)
			{
				auto p=range.first->second.getptr();

				if (!p.null())
					w=p;
				break;
			}
		}

		if (!w.null())
			try {
				w->cb(e->mask, e->cookie, e->name);
			} catch (const exception &e)
			{
				LOG_ERROR(e);
				LOG_TRACE(e->backtrace);
			} catch (...)
			{
				LOG_ERROR("Unknown exception");
			}
	}
}

#if 0
{
#endif
}
