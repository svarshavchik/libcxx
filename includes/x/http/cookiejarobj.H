#ifndef x_http_allcookies_H
#define x_http_allcookies_H

#include <x/http/cookiemrulist.H>
#include <x/hier.H>
#include <x/logger.H>

namespace LIBCXX_NAMESPACE {

	class uriimpl;

	namespace http {
		class cookie;
#if 0
	};
};
#endif

class domaincookiesObj;
class responseimpl;
class storedcookieObj;

//! A cookie jar object.

class cookiejarObj : virtual public obj {

	//! Logging
	LOG_CLASS_SCOPE;

	//! A hierarchical container, by domain component

	typedef hier<std::string, ref<domaincookiesObj> > cookiedomainhier_t;

	//! The cookies, by domain hierarchy

	cookiedomainhier_t cookiedomainhier;

	//! All cookies in all domains

	cookiemrulist_t allcookies;

 public:

	//! Constructor
	cookiejarObj();

	//! Destructor

	~cookiejarObj();

	//! Store cookies from a response

	void store(//! The original request
		   const uriimpl &request,

		   //! The response &response,
		   const responseimpl &resp);

	//! Find all matching cookies

	void find(//! We want cookies for this URI
		  const uriimpl &uri,

		  //! Put my cookies here, a list of name=value pairs.

		  std::list<std::pair<std::string, std::string> > &cookies,

		  //! Whether we want to exclude cookies that are HttpOnly

		  bool for_http=true);

	//! Save the cookie jar into a file

	//! This method comes from libcxxyaml.

	void save(const std::string &filename);

	//! Load saved cookies from a file

	//! This method comes from libcxxyaml.

	void load(const std::string &filename);

 private:
	//! Store a cookie

	//! \internal
	void store(//! The cookie to store

		   //! The cookie's domain and path must be validated values.

		   const ref<storedcookieObj> &cookie) LIBCXX_HIDDEN;

	//! Used by load()

	void store(const cookie &c);

	//! Remove a cookie

	//! \internal

	void remove(cookiemrulist_t::lock &allcookies_lock,
		    const cookiedomainhier_t::base::writelock &writer_lock,
		    const ref<storedcookieObj> &cookie) LIBCXX_HIDDEN;

	//! Split domain by hostnames, in reverse order

	static void domain_key(const std::string &domain,
			       std::list<std::string> &key) LIBCXX_HIDDEN;

public:

	class iteratorimplObj;

	//! An iterator over the contents of a cookiejar

	class iterator : public std::iterator<std::input_iterator_tag, cookie> {

		//! Internal implementation

		ptr<iteratorimplObj> value;

		//! Advance to next non-empty position in the hier objects

		void nextnonempty() LIBCXX_HIDDEN;

	public:
		friend class cookiejarObj;

		//! Internal constructor

		iterator(const ptr<iteratorimplObj> &valueArg);

		//! Ending iterator constructor
		iterator();

		//! Copy constructor
		iterator(const iterator &);

		//! Destructor
		~iterator();

		//! Assignment operator
		iterator &operator=(const iterator &);

		//! Iterator operator
		iterator &operator++();

		//! Iterator operator
		const cookie *operator++(int);

		//! Iterator operator
		cookie operator*() const;

		//! Iterator operator
		bool operator==(const iterator &o) const;

		//! Iterator operator

		bool operator!=(const iterator &o) const
		{
			return !operator==(o);
		}
	};

	//! Return a beginning iterator over cookies in the cookie jar
	iterator begin() const;

	//! Return an ending iterator over cookies in the cookie jar
	iterator end() const;
};

#if 0
{
	{
#endif
	}
}
#endif
