#ifndef xml_internal_h
#define xml_internal_h

#include <libxml/parser.h>
#include "x/xml/doc.H"
#include "x/xml/parser.H"
#include "x/xml/dtd.H"
#include "x/xml/newdtd.H"
#include "x/logger.H"
#include "x/rwlock.H"

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
		std::string message;

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

// Implement xml::doc

class LIBCXX_HIDDEN impldocObj : public docObj {

 public:
	class readlockImplObj;
	class createnodeImplObj;
	class createchildObj;
	class createnextsiblingObj;
	class createprevioussiblingObj;
	class writelockImplObj;

	class save_impl;

	xmlDocPtr p; // The libXML document.

	rwlock lock; // Readers/writer lock object

	// Constructor
	impldocObj(xmlDocPtr pArg);

	// If p is not nullptr, destroy it.
	~impldocObj() noexcept;

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

	implparserObj(const std::string &uriArg, const std::string &options);
	~implparserObj() noexcept;
	doc done() override;
	void operator=(char c) override;
};

// Implement read-only DTD methods.
// This subclasses newdtdObj, which subclasses dtdObj, in order to implement
// readonly methods for both classes. Methods inherited from newdtdObj
// throw an exception.

class LIBCXX_HIDDEN dtdimplObj : public newdtdObj {

 public:

	struct LIBCXX_HIDDEN subset_impl_t {

		// Whether all methods apply to the external or the internal subset
		xmlDtdPtr (dtdimplObj::*get_dtd)();
	};

	const subset_impl_t &subset_impl;

	static const subset_impl_t impl_external, impl_internal;

	// The document
	ref<impldocObj> impl;

	// This DTD's methods apply to the external subset.
	xmlDtdPtr get_external_dtd();

	// This DTD's methods apply to the internal subset.
	xmlDtdPtr get_internal_dtd();

	// This DTD better exist.
	xmlDtdPtr get_dtd_not_null();

	doc::base::readlock lock;

	dtdimplObj(const subset_impl_t &subset_implArg,
		   const ref<impldocObj> &implArg,
		   const doc::base::readlock lockArg);
	~dtdimplObj() noexcept;

	bool exists() override;

	std::string name() override;

	std::string external_id() override;

	std::string system_id() override;
};

class LIBCXX_HIDDEN newdtdimplObj : public dtdimplObj {
 public:

	doc::base::writelock lock;

	newdtdimplObj(const subset_impl_t &subset_implArg,
		      const ref<impldocObj> &implArg,
		      const doc::base::writelock &lockArg);
	~newdtdimplObj() noexcept;
};

void throw_last_error(const char *context)
	LIBCXX_HIDDEN __attribute__((noreturn));

std::string not_null(xmlChar *str, const char *context) LIBCXX_HIDDEN;

std::string null_ok(xmlChar *str) LIBCXX_HIDDEN;

std::string get_str(const xmlChar *str) LIBCXX_HIDDEN;

#if 0
{
	{
#endif
	}
}
#endif
