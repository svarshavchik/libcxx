/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_form_H
#define x_http_form_H

#include <x/obj.H>
#include <x/ptr.H>
#include <x/fditer.H>
#include <x/refiterator.H>
#include <x/http/formfwd.H>
#include <x/http/requestimpl.H>
#include <x/http/responseimpl.H>
#include <x/http/receiverimpl.H>
#include <x/mime/tokens.H>
#include <x/mime/structured_content_header.H>
#include <x/namespace.h>

#include <iterator>

namespace LIBCXX_NAMESPACE {

	namespace http {
		namespace form {
#if 0
		};
	};
};
#endif

template<typename iter_type> class limited_iter;

//! Return the contents of the \c INSERT_LIBX_NAMESPACE::http::form::maxsize property

size_t getformmaxsize();

//! Internal hexadecimal string.

extern const char hex[];

//! Encountered a %-encoded nybble parsing an input sequence

//! The next two characters are expected to be a hexadecimal nybble.

template<typename iter_type>
std::pair<iter_type, unsigned char> decode_nybble(//! Beginning iterator value
						  iter_type b,

						  //! Ending iterator value
						  iter_type e)
{
	unsigned char c=0;

	if (b != e)
	{
		const char *p=strchr(hex, *b++);

		if (p)
		{
			if (p >= hex+16)
				p -= 6;

			c=(p-hex) << 4;

			if (b != e)
			{
				p=strchr(hex, *b++);

				if (p)
				{
					if (p >= hex+16)
						p -= 6;

					c += (p-hex);
				}
			}
		}
	}

	return std::make_pair(b, c);
}

//! Parameters in an HTTP form.

class parametersObj : public map_t, virtual public obj {

	//! When constructing from an HTTP request, whether POST was consumed

	bool consumedFlag;

public:
	//! Default constructor

	parametersObj();

	//! Construct from a list of strings

	template<typename... Args_t>
	parametersObj(const std::string &paramname,
		      const std::string &paramvalue,
		      Args_t &&...args) : consumedFlag(false)
	{
		init_from_string_list(paramname, paramvalue,
				      std::forward<Args_t>(args)...);
	}

	//! Construct from a www-encoded string

	parametersObj(const std::string &query_string);

	//! Construct from a container

	template<typename container_type>
	parametersObj(const container_type &container) : consumedFlag(false)
	{
		decode_params(container.begin(), container.end());
	}
	//! Destructor
	~parametersObj() noexcept;

private:

	//! Empty string list

	void init_from_string_list() noexcept {}

	//! Initialize from a string list of name/value pairs.

	template<typename... Args_t>
	void init_from_string_list(const std::string &paramname,
				   const std::string &paramvalue,
				   Args_t && ...args)

	{
		insert(std::make_pair(paramname, paramvalue));
		init_from_string_list(std::forward<Args_t>(args)...);
	}

	//! Base class for the interface to the file receiver lambda/functor

	class filereceiverObj : virtual public obj {

	public:

		//! Constructor
		filereceiverObj();

		//! Destructor
		~filereceiverObj() noexcept;

		//! Write the next chunk

		virtual void write(const std::vector<char> &chunk)=0;

		//! Finished
		virtual void close()=0;
	};

	//! Functor adapter for filereceiverObj

	template<typename write_functor_type, typename close_functor_type>
	class filereceiverImplObj : public filereceiverObj {

		//! The write functor
		write_functor_type write_functor;

		//! The close functor
		close_functor_type close_functor;

	public:

		//! Constructor
		template<typename writeArgType, typename closeArgType>
		filereceiverImplObj(writeArgType &&write,
				    closeArgType &&close)
			: write_functor(std::forward<writeArgType>(write)),
			  close_functor(std::forward<closeArgType>(close))
		{
		}

		//! Destructor

		filereceiverImplObj();

		void write(const std::vector<char> &chunk) override
		{
			write_functor(chunk);
		}

		void close() override
		{
			close_functor();
		}
	};

public:

	//! Next file receiver gets stored here.

	class filereceiver {

	public:
		//! No copy constructor
		filereceiver(const filereceiver &)=delete;

		//! Constructor
		filereceiver();

		//! Destructor
		~filereceiver() noexcept;

		//! Constructed receiver object.

		ptr<filereceiverObj> receiverptr;

		//! Get the next file receiver

		template<typename write_functor_type,
			 typename close_functor_type>
		void receive(write_functor_type &&write_functor,
			     close_functor_type &&close_functor)
		{
			receiverptr=ref<filereceiverImplObj
					<typename
					 std::decay<write_functor_type>::type,
					 typename
					 std::decay<close_functor_type>::type>>
				::create(std::forward<write_functor_type>
					 (write_functor),
					 std::forward<close_functor_type>
					 (close_functor));
		}
	};

private:

	//! A factory for file receiver.

	struct filereceiverfactorybase {

		//! Get the next receiver

		//! The default implementation returns a null pointer, which
		//! results in a thrown 404 exception.

		virtual ptr<filereceiverObj>
		next(//! Full headers
		     const headersbase &headers,

		     //! Field name, converted to UTF-8
		     const std::string &name_utf8,

		     //! Filename, converted to UTF-8
		     const std::string &filename_utf8) const
		{
			return ptr<filereceiverObj>();
		}
	};

	//! Functor adapter for the file receiver factory

	template<typename functor_type>
	class filereceiverfactory : public filereceiverfactorybase {

		//! The functor that constructs a new receiver.
		functor_type functor;

	public:

		//! Constructor
		template<typename functorArgType>
		filereceiverfactory(functorArgType &&functorArg)
			: functor(std::forward<functorArgType>(functorArg)) {}

		//! Invoke the functor, return its receiver object.
		ptr<filereceiverObj> next(const headersbase &headers,
					  const std::string &name_utf8,
					  const std::string &filename_utf8)
			const override
		{
			filereceiver receiver;

			functor(headers, name_utf8, filename_utf8, receiver);

			return receiver.receiverptr;
		}
	};

public:
	//! Decode CGI form parameters

	//! \internal
	//!
	template<typename iter_type>
	static iter_type decode_octet(iter_type b, iter_type e,
				      std::string &s)
	{
		char c=*b;

		if (c == '+')
		{
			s.push_back(' ');
			return ++b;
		}

		if (c != '%')
		{
			s.push_back(c);
			return ++b;
		}

		auto ret=decode_nybble(++b, e);

		s.push_back(ret.second);
		return ret.first;
	}

	//! Used by useragent to decode authority part of the URL

	template<typename iter_type>
	static std::string decode_octets(iter_type b, iter_type e)
	{
		std::string s;

		while (b != e)
		{
			b=decode_octet(b, e, s);
		}
		return s;
	}

	//! Add additional form parameters from x-www-form-urlencoded parameters

	template<typename iter_type>
	void decode_params(iter_type b, iter_type e)
	{
		while (b != e)
		{
			std::pair<std::string, std::string> param;
		
			while (b != e && *b != '=' && *b != '&')
				b=decode_octet(b, e, param.first);
			if (b != e && *b == '=')
			{
				++b;
				while (b != e && *b != '&')
					b=decode_octet(b, e, param.second);
			}
			if (param.first.size() && param.second.size())
				this->insert(param);
			if (b != e)
				++b;
		}
	}

	//! Construct from form parameters from a received HTTP request.

	//! If the request is a form upload, a 404 exception gets thrown.
	//!

	template<typename input_iter>
	parametersObj(//! HTTP message receiver
		      receiverimpl<requestimpl, input_iter> &receiver,

		      //! HTTP request received
		      const requestimpl &req,

		      //! Whether there's content in the request
		      bool hasbody) : consumedFlag(false)
	{
		consumedFlag=
			process_received_request(receiver,
						 req, hasbody,
						 filereceiverfactorybase(),
						 "UTF-8");
	}

	//! Construct from form parameters from a received HTTP request

	//! The additional parameter specifies a factory for receiving uploaded
	//! files.

	template<typename input_iter, typename factory_type>
	parametersObj(//! HTTP message receiver
		      receiverimpl<requestimpl, input_iter> &receiver,

		      //! HTTP request received
		      const requestimpl &req,

		      //! Whether there's content in the request
		      bool hasbody,

		      //! The factory for upload receivers
		      factory_type &&factory,

		      //! Upload form's encoding
		      const std::string &form_encoding
		      ) : consumedFlag(false)
	{
		typedef typename std::decay<factory_type>::type functor_t;


		consumedFlag=process_received_request
			(receiver, req, hasbody,
			 filereceiverfactory<functor_t>(factory),
			 form_encoding);
	}

	//! Get the consumed flags

	bool consumed() const { return consumedFlag; }
private:

	class rfc2388Obj;

	//! Create an iterator for parsing multipart/form-data content.

	//! There's a subclass with all the gory details. We, thankfully,
	//! hide it from public view.
	//!
	//! \internal

	static outputrefiterator<int>
	create_multipart_formdata_iterator(const std::string &boundary,
					   const filereceiverfactorybase &fac,
					   parametersObj &me,
					   const std::string &form_encoding);

	//! Retrieve form parameters from a received HTTP message.

	template<typename input_iter>
	bool process_received_request(//! HTTP message receiver
				      receiverimpl<requestimpl, input_iter> &receiver,

				      //! HTTP request received
				      const requestimpl &req,

				      //! Whether there's content in the request
				      bool hasbody,

				      //! The file receiver factory
				      const filereceiverfactorybase &factory,

				      //! Form's original encoding
				      const std::string &form_encoding)
	{

		const std::string &s=req.getURI().getQuery();

		decode_params(s.begin(), s.end()); // GET

		if (!hasbody)
			return false;

		mime::structured_content_header
			ct(req, mime::structured_content_header::content_type);

		typedef typename receiverimpl<requestimpl, input_iter>
			::iterator decode_iter_t;

		if (ct == mime::structured_content_header
		    ::application_x_www_form_urlencoded)
		{
			decode_params(limited_iter<decode_iter_t>
				      (receiver.begin(), getformmaxsize()),
				      limited_iter<decode_iter_t>
				      (receiver.end(), 0));
			return true;
		}

		if (ct == mime::structured_content_header::multipart_form_data)
		{
			std::string boundary=ct.boundary();

			if (!boundary.empty())
			{
				auto iter=create_multipart_formdata_iterator
					(boundary, factory, *this,
					 form_encoding);

				for (unsigned char c:receiver)
				{
					*iter=c;
				}

				*iter=mime::eof;
				return true;
			}
		}
		return false;
	}

	//! %-escape a word, in UTF-8 (if the flag is set)

	//! This is an iterator-like object.
	//! \internal

	class word_iter {

		//! Current ptr
		const char *ptr;

		//! Enumeration that defines the current escape state
		typedef enum {
			no_escape,
			do_escape,
			escape_next1,
			escape_next2 } escape_t;

		//! Current escape state

		escape_t escape;

	public:
		//! Constructor - beginning iterator
		word_iter(//! String to escape (or not)
			  const char *ptrArg,

			  //! Whether to %-escape the string
			  bool escapeArg)
			: ptr(ptrArg),
			  escape(escapeArg ? do_escape:no_escape)
		{
		}

		//! Constructor - ending iterator

		word_iter() : ptr("") {}

		//! Destructor
		~word_iter() noexcept {}

		//! Return pointer to current character
		const char *curchar() noexcept LIBCXX_INTERNAL;

		//! Advance this iterator
		void advance() LIBCXX_INTERNAL;

	};

public:

	class encode_iter;

	//! Return a beginning iterator for encoding form parameters
	encode_iter encode_begin() const;

	//! Return the ending iterator value for encoding form parameters
	encode_iter encode_end() const;
};

//! Refer to this class as \c INSERT_LIBX_NAMESPACE::form::parameters::base or \c INSERT_LIBX_NAMESPACE::form::parametersptr::base

class parametersBase : public ptrrefBase {

public:

	//! Convenient typedef to encoding iterator

	typedef parametersObj::encode_iter encode_iter;

	//! Convenience typedef

	//! This is the parameter that's passed by reference to the lambda
	//! or the functor when receiving uploaded files from a form.
	//! The lambda/functor gets a two parameter, a const headersbase,
	//! with the headers of the uploaded file, and a native reference to
	//! the filereceiver object.
	//!
	//! The lambda/functor invokes filereceiver.receive(write, close),
	//! with write and close being two functor. write() receives a
	//! const std::vector<char> &, the next chunk of the received file
	//! (it gets called multiple times until the entire file is received),
	//! followed by close().

	typedef parametersObj::filereceiver filereceiver;

};

//! Specialization:: Use \c std::string::const_iterator template instance for \c std::string::iterator

template<> void
parametersObj::decode_params(std::string::iterator b, std::string::iterator e);


//! Current iterator over the encoded parameter values

class parametersObj::encode_iter :
	public std::iterator<std::forward_iterator_tag, char> {

	//! Current parameter being iterated over
	mutable map_t::const_iterator begin_iter;

	//! Ending iterator value for iteration
	mutable map_t::const_iterator end_iter;

	//! Which parameters being iterated over
	const_parameters params;

	//! An enumeration that defines the current iteration state
	typedef enum {
		next_do_equals,
		next_do_value,
		next_do_advance,
		next_do_param } next_word_t;

	//! Current iteration state
	mutable next_word_t next_word;

	//! Current iterator over the word being encoded
	mutable word_iter cur_word;

	//! Constructor
	encode_iter(const const_parameters &parametersArg,
		    bool endingIterator)
	LIBCXX_INTERNAL;

	//! Return true if the end of the iteration has been reached
	bool eof() const;

	//! Return the current character to emit
	const char *curchar() noexcept;

	//! Advance to the next character to emit
	void advance();

public:
	friend class parametersObj;

	//! Destructor
	~encode_iter() noexcept;

	//! Iterator operator
	bool operator==(const encode_iter &o) const
	{
		return eof() && o.eof();
	}

	//! Iterator operator
	bool operator!=(const encode_iter &o) const
	{
		return !operator==(o);
	}

	//! Iterator operator
	char operator*()
	{
		return *curchar();
	}

	//! Iterator operator
	encode_iter &operator++()
	{
		advance();
		return *this;
	}

	//! Iterator operator
	const char *operator++(int)
	{
		const char *p=curchar();
		operator++();
		return p;
	}
};

//! Derive an iterator that throws an %exception upon iterating over more than N characters

template<typename iter_type> class limited_iter
	: public std::iterator<std::input_iterator_tag,
			       typename iter_type::value_type,
			       void,
			       typename iter_type::value_type *,
			       typename iter_type::value_type &> {

	//! The underlying iterator

	iter_type iter;

	//! How many characters are left to process
	size_t left;

	//! Helper object for the post-increment operator.

	class helper {
	public:
		//! The saved iterator's value
		typename iter_type::value_type value;

		//! Constructor
		helper(const typename iter_type::value_type &valueArg)
			: value(valueArg) {}

		//! Destructor
		~helper() noexcept {}

		//! Iterator operator
		typename iter_type::value_type operator*() { return value; }
	};

public:
	//! Constructor, presumably the ending iterator

	limited_iter() : left(0) {}

	//! Destructor
	~limited_iter() noexcept {}

	//! Constructor, presumably the beginning iterator

	limited_iter(//! Underlying iterator value
		     const iter_type &iterArg,

		     //! How many characters to process before throwing an %exception
		     size_t leftArg)
		: iter(iterArg), left(leftArg)
	{
	}

	//! Assignment operator

	limited_iter<iter_type> &operator=(const iter_type &iterArg)
	{
		iter=iterArg;
		return *this;
	}

	//! Iterator operator
	bool operator==(const limited_iter<iter_type> &o) const noexcept
	{
		return iter == o.iter;
	}

	//! Iterator operator
	bool operator!=(const limited_iter<iter_type> &o) const noexcept
	{
		return iter != o.iter;
	}

	//! Iterator operator
	typename iter_type::value_type operator*()
	{
		return *iter;
	}

	//! Iterator operator
	limited_iter<iter_type> &operator++()
	{
		if (left == 0)
			responseimpl::throw_request_entity_too_large();
		--left;
		++iter;
		return *this;
	}

	//! Iterator operator
	helper operator++(int)
	{
		helper ret(operator*());

		++*this;

		return ret;
	}

	//! Retrieve the underlying iterator

	operator iter_type() noexcept
	{
		return iter;
	}
};

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/http/form_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
	{
		{
#endif
		}
	}
}

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/http/useragentobj_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif


#endif
