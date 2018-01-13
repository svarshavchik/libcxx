/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_propertyvalueeventhandlerobj_H
#define x_propertyvalueeventhandlerobj_H

#include <string>
#include <map>

#include <x/property_valuefwd.H>
#include <x/eventhandlerobj.H>
#include <x/locale.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {
	namespace property {
#if 0
	};
};
#endif

class listObj;
class nodeObj;

//! Property %value update notification

//! When a property %value is to be set, this is what gets passed to the
//! property %value container: a pointer to the requested property %value,
//! and a locale for interpreting the %value.

typedef std::pair<std::string *, const_locale> propvalueset_t;

//! Base class for %property change notification

//! Subclass notifyObj, and define either event() callback.
//! Call \ref propvaluesetbase::installNotify "property::value<>::installNotify()"
//! to have event() invoked whenever the %property's %value changes.
//!
//! The callback is allowed to invoke get(), on its %property but may not
//! modify any %properties, otherwise a deadlock may occur.

class notifyObj : public eventhandlerObj<propvalueset_t> {

public:
	//! Constructor
	notifyObj();

	//! Destructor
	~notifyObj();

	//! The real callback

	//! The default implementation invokes event(void)
	//!
	void event(const propvalueset_t &newvalue) override;

	//! The fallback callback

	//! The default implementation does nothing.
	virtual void event(void);
};

//! A reference to a notification handler

typedef ref<notifyObj> notify;

//! A nullable pointer to a notification handler

typedef ptr<notifyObj> notifyptr;

class propvaluebaseObj;

//! Base class for property %value containers

//! All property %value containers, of all types, derive from valuebase.
//! The handler for property %value change events invokes set(), which
//! must be implemented by the subclass.

class propvaluesetbase {

protected:

	//! Property notification handler
	ptr<propvaluebaseObj> handler;

protected:

	//! Install the property callback

	void install(const char *namestr, //!< Property name
		     const std::string &initvalue, //!< Initial property value
		     const const_locale &locale //!< Locale
		     );

	//! Install the property callback

	void install(const ptr<listObj> &props, //!< Property list
		     const char *namestr, //!< Property name
		     const std::string &initvalue, //!< Initial property value
		     const const_locale &locale //!< Locale
		     );
	//! Install the property callback

	void install(const std::string &namestr, //!< Property name
		     const std::string &initvalue, //!< Initial property value
		     const const_locale &locale //!< Locale
		     );
	//! Install the property callback

	void install(const ref<listObj> &props, //!< Property list
		     const std::string &namestr, //!< Property name
		     const std::string &initvalue, //!< Initial property value
		     const const_locale &locale //!< Locale
		     );
	//! Break the property callback

	//! This function disables automatic forwarding of updated property
	//! value to this object. This function gets invoked from
	//! property::value<>'s destructor, to make sure that the link is
	//! broken before the destruction commences.

	void uninstall();

public:
	//! Default constructor
	propvaluesetbase();

	//! Default destructor
	virtual ~propvaluesetbase();

	//! Update property %value

	//! This is implemented by the actual subclass.
	//!
	virtual void update(//! Updated property %value
			    const propvalueset_t &newvalue)=0;

	//! Install a notification callback when the %property %value changes

	void installNotify(const notify &notifyRef);
};

#if 0
{

	{
#endif
	}
}
#endif
