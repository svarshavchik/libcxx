/*
** Copyright 2012-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/dirwalktreeobj.H"
#include "x/sysexception.H"
#include <dirent.h>
#include <cerrno>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

dirwalktreeObj::dirwalktreeObj(const std::string &dirnameArg,
			       bool ispostorderArg)
	: ispostorder(ispostorderArg)
{
	dir d=dir::create(dirnameArg);

	entry e;

	try {
		e.b=d->begin();
		e.e=d->end();
	} catch (const sysexception &ex)
	{
		if (ex.getErrorCode() != ENOENT)
			throw;
		e.b=e.e=d->end();
	}
	dirtree.push_back(e);
}

dirwalktreeObj::~dirwalktreeObj()
{
}

bool dirwalktreeObj::initial()
{
	if (dirtree.back().b == dirtree.back().e)
		return true;

	if (!ispostorder)
		return false;

	postorder_opendirs();
	return false;
}

void dirwalktreeObj::postorder_opendirs()
{
	while (dirtree.back().b->filetype() == DT_DIR)
	{
		dir d=dir::create(dirtree.back().b->fullpath());

		entry e;

		try {
			e.b=d->begin();
			e.e=d->end();
		} catch (const sysexception &ex)
		{
			if (ex.getErrorCode() != ENOENT)
				throw;
			e.b=e.e=d->end();
		}

		if (e.b == e.e)
			return;

		dirtree.push_back(e);
	}
}

bool dirwalktreeObj::increment()
{
	if (ispostorder)
	{
		if (++dirtree.back().b == dirtree.back().e)
		{
			dirtree.pop_back();
			if (dirtree.empty())
				return true;
			return false;
		}
		postorder_opendirs();
		return false;
	}

	if (dirtree.back().b->filetype() == DT_DIR)
	{
		dir d=dir::create(dirtree.back().b->fullpath());

		entry e;

		try {
			e.b=d->begin();
			e.e=d->end();
		} catch (const sysexception &ex)
		{
			if (ex.getErrorCode() != ENOENT)
				throw;
			e.b=e.e=d->end();
		}

		if (e.b != e.e)
		{
			dirtree.push_back(e);
			return false;
		}
	}

	while (++dirtree.back().b == dirtree.back().e)
	{
		dirtree.pop_back();
		if (dirtree.empty())
			return true;
	}

	return false;
}

#if 0
{
#endif
}
