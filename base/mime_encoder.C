/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxx_config.h"
#include "x/mime/encoder.H"
#include "x/mime/structured_content_header.H"
#include "x/uuid.H"
#include "x/tostring.H"
#include "x/fd.H"
#include "x/singleton.H"
#include "x/strtok.H"
#include "x/exception.H"
#include "x/sysexception.H"
#include <magic.h>
#include <fstream>

namespace LIBCXX_NAMESPACE {
	namespace mime {
#if 0
	};
};
#endif

encoderObj::section::~section()
{
}

auto encoderObj::section::begin() const
	-> decltype( (*(encoderbase *)nullptr)->begin())
{
	return operator*().begin();
}

auto encoderObj::section::end() const
	-> decltype( (*(encoderbase *)nullptr)->end())
{
	return operator*().end();
}

inputrefiterator<char> encoderObj::ending_iterator_factory::ending_iterator()
{
	return inputrefiterator<char>::create();
}

//! The internal input iterator that's returned by this encoded object

//! \internal
//! Instantiate a join iterator to combine the contents of all
//! \ref section "sections".

class LIBCXX_HIDDEN encoderObj::combinedObj : public inputrefiteratorObj<char> {

	//! Reference to the encoded object this iterator is from.

	//! This makes sure that the encoded object doesn't go out of
	//! scope and get destroyed before this iterator does.

	const_encoder encoder_ref;

	//! Current join iterator
	mutable joiniterator_t cur;

	//! Ending iterator value
	joiniterator_t end;

	//! Fill the iterator.

	void fill() const override;

 public:
	//! Constructor
	combinedObj(const const_encoder &encoder_refArg,
		    const joiniterator_t &curArg,
		    const joiniterator_t &endArg);
	//! Destructor
	~combinedObj();
};

encoderObj::combinedObj::combinedObj(const const_encoder &encoder_refArg,
				     const joiniterator_t &curArg,
				     const joiniterator_t &endArg)
	: encoder_ref(encoder_refArg),
	  cur(curArg), end(endArg)
{
}

encoderObj::combinedObj::~combinedObj()
{
}

void encoderObj::combinedObj::fill() const
{
	auto n=fdbaseObj::get_buffer_size();

	for (size_t i=0; i<n; ++i)
	{
		if (cur == end)
			break;

		this->buffer.push_back(*cur);
		++cur;
	}
}

encoderObj::encoderObj() : sections(vector<section>::create())
{
}

encoderObj::~encoderObj()
{
}

encoderObj::joiniterator_t encoderObj::beg_iter() const
{
	return joiniterator_t(sections->begin(), sections->end());
}

inputrefiterator<char> encoderObj::begin() const
{
	auto b=beg_iter();

	return ref<combinedObj>::create(const_encoder(this), b, b.end());
}

inputrefiterator<char> encoderObj::end() const
{
	auto iter=beg_iter().end();

	return ref<combinedObj>::create(const_encoder(this), iter, iter);
}

template<typename header_type>
encoder encoderObj::assemble_section(const headersimpl<header_type> &header,
				     const encoderbase &section,
				     const char *extra_header_name,
				     const char *extra_header_value)
{
	headersimpl<header_type> content_transfer_encoding_header;

	if (extra_header_value)
	{
		if (!extra_header_name)
			extra_header_name=structured_content_header
				::content_transfer_encoding;

		content_transfer_encoding_header
			.append(extra_header_name,
				extra_header_value);
	}

	auto e=encoder::create();

	e->sections->reserve(2);
	e->add_headers(header, content_transfer_encoding_header);
	e->sections->push_back(section);

	return e;
}

template<typename header_type>
void encoderObj::add_headers(const headersimpl<header_type> &headers,
			     const headersimpl<header_type> &extra_header)
{
	std::ostringstream o;

	extra_header.toString(headers.toString(std::ostreambuf_iterator<char>
					       (o), true));

	sections->push_back(create_plain_encoder(o.str()));
}

template<typename header_type> void encoderObj::
assemble_multipart_content_type(headersimpl<header_type> &headers,
				const std::string &subtype,
				const std::string &boundary,
				size_t maxwidth)
{
	structured_content_header h;

	h.value="multipart/"+subtype;

	parameter_t param;

	param.name="boundary";
	param.value=boundary;

	h.parameters.insert(std::make_pair(param.name, param));

	h.append(headers,
		 structured_content_header::content_type,
		 maxwidth);
}

void encoderObj::add_boundary(boundary_type type,
			      const std::string &boundary,
			      const char *newline_seq)
{
	std::ostringstream o;

	if (type == boundary_type::initial)
		o << "This is a MIME multipart message";

	o << newline_seq << "--" << boundary;

	if (type == boundary_type::final)
		o << "--";
	o << newline_seq;

	sections->push_back(create_plain_encoder(o.str()));
}

std::string encoderObj::new_boundary()
{
	return tostring(uuid()) + ":=";
}

template
encoder encoderObj::assemble_section(const headersimpl<headersbase::crlf_endl>
				     &header,
				     const encoderbase &,
				     const char *, const char *);

template
encoder encoderObj::assemble_section(const headersimpl<headersbase::lf_endl>
				     &header,
				     const encoderbase &,
				     const char *, const char *);

template
void encoderObj::add_headers(const headersimpl<headersbase::crlf_endl> &headers,
			     const headersimpl<headersbase::crlf_endl> &extra);

template
void encoderObj::add_headers(const headersimpl<headersbase::lf_endl> &headers,
			     const headersimpl<headersbase::lf_endl> &extra);

template void encoderObj
::assemble_multipart_content_type<headersbase::crlf_endl
				  >(headersimpl<headersbase::crlf_endl>
				    &headers,
				    const std::string &subtype,
				    const std::string &boundary,
				    size_t maxwidth);

template void encoderObj
::assemble_multipart_content_type<headersbase::lf_endl
				  >(headersimpl<headersbase::lf_endl>
				    &headers,
				    const std::string &subtype,
				    const std::string &boundary,
				    size_t maxwidth);

//////////////////////////////////////////////////////////////////////////////

from_file_container::from_file_container(const std::string &filenameArg)
	: filename(filenameArg)
{
}

from_file_container::~from_file_container()
{
}

fdinputiter from_file_container::begin() const
{
	return fdinputiter(fd::base::open(filename, O_RDONLY));
}

fdinputiter from_file_container::end()
{
	return fdinputiter();
}

fd from_fd_container::check_filedesc(const fd &filedescArg)
{
	if (S_ISREG(filedescArg->stat().st_mode))
		return filedescArg;

	auto tmpfile=fd::base::tmpfile();

	std::copy(fdinputiter(filedescArg), fdinputiter(),
		  fdoutputiter(tmpfile)).flush();

	return tmpfile;
}

from_fd_container::from_fd_container(const fd &filedescArg)
	: filedesc(check_filedesc(filedescArg))
{
}

from_fd_container::~from_fd_container()
{
}

fdinputiter from_fd_container::begin() const
{
	auto newfd=fd::base::dup(filedesc);

	newfd->seek(0, SEEK_SET);
	return fdinputiter(newfd);
}

fdinputiter from_fd_container::end()
{
	return from_file_container::end();
}

//////////////////////////////////////////////////////////////////////////////

class magicObj : virtual public obj {

	magic_t handle;

public:
	magicObj() LIBCXX_HIDDEN;
	~magicObj() LIBCXX_HIDDEN;

	std::string lookup(const fd &filedesc, int type) LIBCXX_HIDDEN;
};

magicObj::magicObj() : handle(magic_open(MAGIC_NONE))
{
	if (!handle)
		throw SYSEXCEPTION("magic_open");
	magic_load(handle, NULL);
}

magicObj::~magicObj()
{
	if (handle)
		magic_close(handle);
}

std::string magicObj::lookup(const fd &filedesc, int type)
{
	std::lock_guard<std::mutex> lock(objmutex);

	if (magic_setflags(handle, type) < 0)
		throw SYSEXCEPTION("magic_setflags");

	const char *p=magic_descriptor(handle, filedesc->get_fd());

	if (!p)
	{
		p=magic_error(handle);

		if (p)
			throw EXCEPTION(p);

		errno=magic_errno(handle);
		throw SYSEXCEPTION("magic_descriptor");
	}

	std::string s=p;

	return s;
}

static singleton<magicObj> magic;

std::string filetype(const std::string_view &filename,
		     bool check_contents)
{
	auto p=filename.rfind('/');

	if (p == std::string::npos)
		p=0;

	p=filename.find('.', p);

	if (p != std::string::npos)
	{
		std::string ext{filename.substr(p+1)};

		std::ifstream i(MIMETYPES);
		std::string s;

		while (i.is_open())
		{
			s.clear();
			std::getline(i, s);
			p=s.find('#');

			if (p != std::string::npos)
				s=s.substr(0, p);

			std::list<std::string> words;

			strtok_str(s, " \t\r", words);

			if (!words.empty())
			{
				std::string mime_type=words.front();

				words.pop_front();

				if (std::find(words.begin(), words.end(), ext)
				    != words.end())
					return mime_type;
			}
			if (i.eof())
				break;
		}
	}

	if (!check_contents)
		return "application/octet-stream";

	return filetype(fd::base::open(filename, O_RDONLY));
}

std::string filetype(const fd &filedesc)
{
 	return magic.get()->lookup(filedesc, MAGIC_MIME_TYPE);
}

std::string filecharset(const std::string &filename)
{
	return filecharset(fd::base::open(filename, O_RDONLY));
}

std::string filecharset(const fd &filedesc)
{
	return magic.get()->lookup(filedesc, MAGIC_MIME_ENCODING);
}

#if 0
{
	{
#endif
	}
}
