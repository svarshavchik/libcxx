#ifndef xml_internal_h
#define xml_internal_h

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <sstream>
#include "x/xml/doc.H"
#include "x/xml/parser.H"
#include "x/xml/dtd.H"
#include "x/xml/newdtd.H"
#include "x/xml/readlock.H"
#include "x/xml/writelock.H"
#include "x/xml/xpath.H"
#include "x/logger.H"
#include "x/locale.H"
#include "x/sharedlock.H"

#include <string_view>
#include <courier-unicode.h>

namespace LIBCXX_NAMESPACE {
	namespace xml {
#if 0
	};
};
#endif

// Sigh... libxml2's global error handler.

class LIBCXX_HIDDEN error_handler {

 public:

	error_handler() noexcept;

	// This gets instantiated on the stack before calling something from
	// libxml2. The thread-local thread_error gets set to this, and the
	// global error handler uses it to unload the error message into
	// message.

	class error {

	public:
		// Accumulating error
		std::ostringstream message;

		// An error was reported
		static __thread bool errorflag;

		static __thread error *thread_error LIBCXX_HIDDEN;

		// Constructed on the stack. Point the thread local ptr to this.
		error() noexcept;

		// The destructor clears thread_error, to make sure that
		// some stray error doesn't create chaos.

		~error() noexcept;

		// Just before going out of scope, check for any reported errors
		void check();
	};

	~error_handler() noexcept;
};

struct LIBCXX_HIDDEN get_localeObj;

// Retrieve the XML document's global locale

// Used by to_xml_char(_or_null)?, not_null(), null_ok(), and get_str()
// helper to convert between the application's character set and xmlChar.

struct get_localeObj {

	virtual const const_locale &get_global_locale() const=0;
};

// Implement xml::doc

class LIBCXX_HIDDEN impldocObj : public docObj, public get_localeObj {

 public:
	class readlockImplObj;
	class createnodeImplObj;
	class createchildObj;
	class createnextsiblingObj;
	class createprevioussiblingObj;
	class writelockImplObj;

	class xpathcontextObj;
	class xpathImplObj;

	class save_impl;

	const xmlDocPtr p; // The libXML document.

	const const_locale global_locale; // The current locale object.

	sharedlock lock; // Readers/writer lock object

	// Constructor
	impldocObj(xmlDocPtr p, const const_locale &global_locale);

	// If p is not nullptr, destroy it.
	~impldocObj() noexcept;

	const const_locale &get_global_locale() const final override
	{
		return global_locale;
	}

	ref<readlockObj> readlock() override;
	ref<writelockObj> writelock() override;
};

// Implement xml::parser

class LIBCXX_HIDDEN implparserObj : public parserObj {

	LOG_CLASS_SCOPE;

	// Label for the document, given to the constructor or setLabel().
	std::string uri;

	// Label for the document that's currently being parsed.
	std::string docuri;

	// Current parser
	xmlParserCtxtPtr p;

	// Options for the parser.
	int p_options;

	// Output buffer. Buffer up the XML document into chunks, before
	// handing them over to libxml.
	std::vector<char> buffer;

	// How full the buffer is.
	size_t buffer_size;

	// Flush anything in the buffer.
	void flush();

 public:

	implparserObj(const std::string_view &uriArg,
		      const std::string_view &options);
	~implparserObj() noexcept;
	doc done() override;
	void operator=(char c) override;
};

// Implement read-only DTD methods.
// This subclasses newdtdObj, which subclasses dtdObj, in order to implement
// readonly methods for both classes. Methods inherited from newdtdObj
// throw an exception.

class LIBCXX_HIDDEN dtdimplObj : public newdtdObj,
				 public get_localeObj {

 public:

	struct LIBCXX_HIDDEN subset_impl_t {

		// Whether all methods apply to the external or the internal subset
		xmlDtdPtr (dtdimplObj::*get_dtd)();

		// Either xmlAddDocEntity or xmlAddDtdEntity
		xmlEntityPtr (*add_entity)(xmlDocPtr, const xmlChar *,
					   int,
					   const xmlChar *,
					   const xmlChar *,
					   const xmlChar *);
	};

	const subset_impl_t &subset_impl;

	static const subset_impl_t impl_external, impl_internal;

	// The document
	const ref<impldocObj> impl;

	// Return our locale object.
	const const_locale &get_global_locale() const final override
	{
		return impl->global_locale;
	}

	// This DTD's methods apply to the external subset.
	xmlDtdPtr get_external_dtd();

	// This DTD's methods apply to the internal subset.
	xmlDtdPtr get_internal_dtd();

	// This DTD better exist.
	xmlDtdPtr get_dtd_not_null();

	const const_readlock lock;

	dtdimplObj(const subset_impl_t &subset_implArg,
		   const ref<impldocObj> &implArg,
		   const const_readlock lockArg);
	~dtdimplObj() noexcept;

	bool exists() override;

	std::string name() override;

	std::string external_id() override;

	std::string system_id() override;

	static void notwrite() __attribute__((noreturn));

	void create_entity(const std::string &name,
			   int type,
			   const std::string &external_id,
			   const std::string &system_id,
			   const std::string &content) override;

	void include_parameter_entity(const std::string &name) override;

};

class LIBCXX_HIDDEN newdtdimplObj : public dtdimplObj {
 public:

	writelock lock;

	newdtdimplObj(const subset_impl_t &subset_implArg,
		      const ref<impldocObj> &implArg,
		      const writelock &lockArg);
	~newdtdimplObj() noexcept;

	void create_entity(const std::string &name,
			   int type,
			   const std::string &external_id,
			   const std::string &system_id,
			   const std::string &content) override;

	void include_parameter_entity(const std::string &name) override;

};

void throw_last_error(const char *context)
	LIBCXX_HIDDEN __attribute__((noreturn));

std::string not_null(xmlChar *str, const char *context,
		     const get_localeObj &) LIBCXX_HIDDEN;

std::string null_ok(xmlChar *str,
		    const get_localeObj &) LIBCXX_HIDDEN;

std::string get_str(const xmlChar *str,
		    const get_localeObj &) LIBCXX_HIDDEN;


struct LIBCXX_HIDDEN to_xml_char;
struct LIBCXX_HIDDEN to_xml_char_or_null;

// Convert a string to xmlChar
//
// We pass a to_xml_char(_or_null)? object to libxml functions that take an
// xmlChar pointer. to_xml_char converts the string to utf8 and stores it, and
// provides an xmlChar * wrapper

struct to_xml_char {

	const std::string s;

	to_xml_char(const std::string_view &s, const get_localeObj &l)
		: s{l.get_global_locale()->toutf8(s)}
	{
	}

	struct utf8_convert : unicode::iconvert::fromu {

		std::string s;

		using unicode::iconvert::fromu::operator();

		int converted(const char *p, size_t n) override
		{
			s.insert(s.end(), p, p+n);
			return 0;
		}
	};

	to_xml_char(const std::u32string_view &s)
		: s{ ({
				       utf8_convert conv;

				       conv.begin(unicode::utf_8);

				       conv(s.data(), s.size());
				       conv.end();

				       conv.s;
			       })}
	{
	}

	operator const xmlChar *() const
	{
		return reinterpret_cast<const xmlChar *>(s.c_str());
	}

	size_t size() const
	{
		return s.size();
	}
};

// Return a null xmlChar pointer for an empty string.

struct to_xml_char_or_null : public to_xml_char {

	using to_xml_char::to_xml_char;

	operator const xmlChar *() const
	{
		if (size() == 0)
			return nullptr;

		return to_xml_char::operator const xmlChar *();
	}
};

#if 0
{
	{
#endif
	}
}
#endif
