/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_propertyvalue_H
#define x_propertyvalue_H

#include <string>

#include <x/property_properties.H>
#include <x/property_valueeventhandlerobj.H>
#include <x/property_valueimpl.H>
#include <x/value_string.H>
#include <x/tostring.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE {

//! \anchor explicit_property_namespace Application properties

//! Properties are arbitrary "name=value" pairs that are loaded from a %property
//! file when the application starts.
//!
//! \par The property file
//!
//! Properties are read from a file when the application starts up. The
//! property file is determined as follows:
//!
//! - The \c PROPERTIES environment variable, if it is set, and the
//! application's userid and groupid are the same as the effective userid and
//! groupid. A setuid or a setgid process will not check the \c PROPERTIES
//! file.
//!
//! - The "properties" extended attribute on the executable binary, if it
//! is set. The extended attribute on an executable can be set with the
//! properties(1) utility.
//!
//! - A file called "program.properties", where
//! <i>program</i> is the application's executable filename.
//!
//! An application may also load default %property values by invoking
//! \ref property::load_properties "INSERT_LIBX_NAMESPACE::property::load_properties"().
//!
//! \ref save_properties "INSERT_LIBX_NAMESPACE::property::save_properties"()
//! takes a properties object and saves it to an output iterator.
//!
//! \par Specifying properties
//!
//! In the properties file, the '#' character introduces a comment. The rest
//! of the line, after the '#' character, gets ignored. Blank lines are also
//! ignored. The remaining lines must be in the form of "name=value".
//!
//! %Property names are not arbitrary, they follow a specific format.
//! A %property name contains hierarchical components
//! with double-colons for the hierarchy separator. Example:
//!
//! \code
//!    window::width = 300
//! \endcode
//!
//! Periods may be used instead of the colons.
//!
//! \code
//!    window.width = 300
//! \endcode
//!
//! Using colons allows natural mapping between %property names and C++ classes,
//! while periods allow for a similar syntax as %property definitions for
//! Java applications. Either periods or colons may be used interchangably.
//!
//! The property name parser also recognizes &lt;, &gt;, a comma, and
//! whitespace, as a hierarchy separator. This is used to implement
//! \ref logger "logging facilities" in template classes, where the template
//! class name, including the template parameters, gets turned into a property
//! hierarchy.
//!
//! \par Getting properties
//!
//! \code
//! static INSERT_LIBX_NAMESPACE::property::value<int> window_width("main::window::width", 400);
//!
//! // ...
//!
//! int window_width_value=window_width.getValue();
//! \endcode
//!
//! \c property::value is a template with a getValue() method which
//! retrieves the current %value of the %property (the %property's name,
//! as a wide string, is given to the
//! constructor). The %value returned by getValue() is a natural type
//! that's given by the template's parameter.
//!
//! An optional second argument to \c property::value's constructor gives
//! the property's value if it's not specified in the property file.
//!
//! The property name, the first parameter to the constructor, may be an
//! empty string. This initializes an anonymous property object, which
//! behaves like a regular \c property::value object, except that it has
//! no name, and is not affected by contents of the property file.
//!
//! This is useful when the application takes an optional property name
//! that specifies a configuration parameter. If the property name is not
//! specified, the application runs with a built-in default value.
//!
//! \par Property specialization
//!
//! The \c property::value template is implemented for all natural integer
//! types, \c bool, and a \c std:string.
//! There are also several specializations:
//!
//! \code
//! // In the properties file
//!
//! cleanup::interval: 3 hours
//!
//! expiration::interval: 1 week
//!
//! // Code:
//!
//! static INSERT_LIBX_NAMESPACE::property::value<INSERT_LIBX_NAMESPACE::hms> cleanup("main::interval",
//!                                           INSERT_LIBX_NAMESPACE::hms("06:00:00"));
//!
//! static INSERT_LIBX_NAMESPACE::property::value<INSERT_LIBX_NAMESPACE::ymd::interval> expiration("expiration::interval",
//!                                                       INSERT_LIBX_NAMESPACE::ymd::interval("2 weeks"));
//!
//! \endcode
//!
//! Two specializations exists that allow convenient specification of elapsed
//! time intervals. The property::value<hms> specialization has a getValue()
//! method that returns an hms object, suitable for specifying elapsed intervals
//! of time that are less than a day. Any string permitted by hms's constructor
//! can initialize the property. The property::value<ymd::interval>
//! specialization's getValue() method returns an ymd::interval object,
//! suitable for specifying elapsed intervals as days.
//!
//! As with all other property::value implementations, the second parameter
//! to the constructor gives the default value when this property is not
//! specified. An optional third parameter specifies a locale for interpreting
//! the string value.
//!
//! \par Updating %properties
//!
//! In addition to the default application property file that's loaded at
//! startup, additional %property files may be loaded on demand.
//!
//! \code
//! std::string filename;
//!
//! // ...
//!
//! INSERT_LIBX_NAMESPACE::property::load_file(filename, true, true, INSERT_LIBX_NAMESPACE::property::errhandler::errlog(),
//!                        INSERT_LIBX_NAMESPACE::locale::base::environment());
//! \endcode
//!
//! The first parameter to load_file() specifies the name of the file.
//!
//! The second parameter specifies whether existing %properties should be
//! updated by the contents of the %property file: \c true - values of
//! %properties that already exists will be updated from the %property file,
//! \c false - existing %properties will not be updated.
//!
//! The third parameter specifies whether new %properties should be created:
//! \c true - properties that do not exist in the %property file will be
//!  created,
//! \c false - new %properties won't be created, the %properties specified
//! in the %property file only update properties that already exist.
//!
//! The fourth parameter specifies how errors get reported. This includes
//! syntax errors in the %property file, as well as failures that occur when
//! converting %property values to their appropriate
//! \ref value "property::value<>" type:
//! \ref errhandler::errthrow "property::errhandler::err" -
//! an exception gets thrown,
//! \ref errhandler::errstream "property::errhandler::errstream(stream)" -
//! write an error message to an output stream (\c std::cerr by default),
//! \ref errhandler::errlog "property::errhandler::errlog()" -
//! \ref logger "log" the error message (default for this optional parameter).
//!
//! The fifth parameter specifies the \ref locale "locale" for parsing
//! properties from the application file, and defaults to the current global
//! application locale.
//!
//! \par Alternatives to %property::load_file()
//!
//! %property::load_properties() is an alternative that loads properties
//! \ref load_properties(std::istream &,const std::string &,bool,bool,const errhandler &,const const_locale &) "from an arbitrary input stream" or
//! \ref load_properties(const std::string &,bool,bool,const errhandler &,const const_locale &) "from a string literal".
//! \ref load_property() "property::load_property()" explicitly sets a value
//! of a single property.
//!
//! \par Locale issues
//!
//! %Property names and values are wide character strings. The locale parameter
//! to %load_properties(), %load_property(), and %load_file() defaults to the
//! current global application locale.
//!
//! The default locale for the properties loaded from the initial global
//! property file, whose name is given by the \c PROPERTIES environment
//! variable or the &ldquo;properties&rdquo; extended property on the
//! executable, or the &ldquo;<i>program</i>.properties&rdquo;
//! filename, is the system environment locale.
//!
//! An application may get started from environments whose locale varies.
//! To fixate the locale for parsing the initial global property file,
//! put "#LOCALE [locale]" on the first
//! line of the property %file:
//!
//! \code
//! #LOCALE en_US.utf-8
//!
//! name=value
//!
//! ...
//! \endcode
//!
//! This example fixates \c UTF-8 as the character set for parsing the
//! property file, and the \c en_US locale for other locale-specific factors.
//!
//! \par Property change notifications
//!
//! \code
//!    INSERT_LIBX_NAMESPACE::property::value<int> intprop("propertyname", 0);
//!
//! class mynotifycb : public INSERT_LIBX_NAMESPACE::property::notifyObj {
//!
//! public:
//!
//! // ...
//!
//!
//!   void event();
//!
//! // ...
//! };
//!
//! INSERT_LIBX_NAMESPACE::ref<mynotifycb> cb(INSERT_LIBX_NAMESPACE::ref<mynotifycb>::create());
//!
//! // ...
//!
//! intprop.installNotify(cb);
//!
//! \endcode
//!
//! This is a notification mechanism for %property changes. Subclass
//! property::notifyObj, and implement event(). After invoking existing
//! %property
//! object's installNotify() method, the callback object's event() gets invoked
//! after any changes to the %property's value.
//!
//! event() is permitted to access the %property's getValue(), however it may
//! not invoke any methods that modify or change %properties, as a deadlock
//! may occur.
//!
//! \par Private %property lists
//!
//! \code
//! #include <INSERT_LIBX_NAMESPACE/property_list.H>
//!
//! INSERT_LIBX_NAMESPACE::property::list props=INSERT_LIBX_NAMESPACE::property::list::create();
//!
//! // ...
//!
//! INSERT_LIBX_NAMESPACE::property::value<int> intprop(props, "propertyname", 0);
//!
//! // ...
//!
//! props->load_file("filename.properties", true, true);
//!
//! // ...
//! \endcode
//!
//! property::list is a reference to a reference-counted private %property
//! pool. Instantiating it creates a list of %properties that's completely
//! separate from the global application %property list. Supply the property
//! list reference as the first parameter to a property::value's constructor to
//! create a %property in the private pool.
//!
//! Use the referenced object's
//! \ref listObj::load_file "load_file()" and
//! \ref listObj::load "load()" methods to initialize
//! %properties in the private pool.

	namespace property {
#if 0
	};
};
#endif

//! A %property %value

//! This object stores a %value defined in the %property file.
//! This is NOT a reference-counted object.
//! This class can be instantiated in the static scope (reference-counted
//! objects cannot). If you want a reference-counted object (and instantiate
//! it on the heap), inherit from this class and obj.
//!
//! Use getValue() to retrieve the %property %value.
//! If the %property %value gets changed by
//! property::update_properties(),
//! the %value returned by getValue() gets updated automatically.
//!
//! \note
//! This class does not implement the copy constructor and the assignment
//! operator.
//!
//! \note
//! If this class is subclassed, the subclass must invoke
//! uninstall() in its destructor.

template<typename argType, typename stringType>
class value : private propvaluesetbase, public valueimpl<argType>,
	      public notcopyableObj {

public:
	//! Initialize the %property %value

	value(//! The name of the %property
	      const char *propname,

	      //! The default %property value, if not specified
	      const argType &initValue=value_string<argType>::initialValue(),

	      //! Current locale
	      const const_locale &l=locale::base::environment())
 : valueimpl<argType>(initValue)
	{
		install(propname, stringType::tostr(this->getValue(), l), l);
	}

	//! Initialize the %property %value

	value(//! Explicit property
	      const ptr<listObj> &props,

	      //! The name of the %property
	      const char *propname,

	      //! The default %property value, if not specified
	      argType initValue=value_string<argType>::initialValue(),

	      //! Current locale
	      const const_locale &l=locale::base::environment())
 : valueimpl<argType>(initValue)
	{
		install(props, propname,
			stringType::tostr(this->getValue(), l), l);
	}

	//! Initialize the %property %value

	value(//! The name of the %property
	      const std::string &propname,

	      //! The default %property value, if not specified
	      const argType &initValue=value_string<argType>::initialValue(),

	      //! Current locale
	      const const_locale &l=locale::base::environment())
 : valueimpl<argType>(initValue)
	{
		install(propname, stringType::tostr(this->getValue(), l), l);
	}

	//! Initialize the %property %value

	value(//! Explicit property
	      const ptr<listObj> &props,

	      //! The name of the %property
	      const std::string &propname,

	      //! The default %property value, if not specified
	      argType initValue=value_string<argType>::initialValue(),

	      //! Current locale
	      const const_locale &l=locale::base::environment())
 : valueimpl<argType>(initValue)
	{
		install(props, propname,
			stringType::tostr(this->getValue(), l), l);
	}

protected:
	using propvaluesetbase::uninstall;

public:
	//! Default destructor

	~value() noexcept
	{
		uninstall();
	}

	using propvaluesetbase::installNotify;
private:
	//! Reset the property %value to its canonical form

	//! The destructor does it when set() terminates, either normally
	//! or by throwing an %exception.

	class sentry {

	public:
		//! Argument to set()

		const propvalueset_t &v;

		//! The %value object
		value<argType, stringType> *prop;

		//! Constructor
		sentry(const propvalueset_t &vArg,
		       value<argType, stringType> *propArg)
			: v(vArg), prop(propArg)
		{
		}

		//! Destructor
		~sentry() noexcept
		{
			*v.first=stringType::tostr(prop->getValue(), v.second);
		}
	};

	//! Update the %property %value

	void set(//! The new value
		 const propvalueset_t &value)
	{
		sentry reset(value, this);

		this->setValue(stringType::fromstr(*value.first, value.second));
	}
};

#if 0
{

	{
#endif
	}
}
#endif
