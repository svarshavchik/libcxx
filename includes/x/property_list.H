/*
** Copyright 2012-2015 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_property_list_H
#define x_property_list_H

#include <x/namespace.h>
#include <x/property_properties.H>
#include <x/weaklist.H>
#include <x/weakmap.H>
#include <x/weakmultimap.H>
#include <x/eventhandlerobj.H>

namespace LIBCXX_NAMESPACE {
	namespace property {
#if 0
	};
};
#endif

class nodehierObj;
class listObj;

//! A reference to a %property %list object

typedef ref<listObj> list;

//! A nullable pointer to a %property %list object

typedef ptr<listObj> listptr;


//! A set property

class nodeObj : virtual public obj {

	std::string val; //!< Current value of the property

	ref<nodehierObj> hierp; //!< Its node in the hierarchical property tree

public:

	friend class listObj;

	//! A container for handlers that get notified when the value changes

	typedef weaklist< eventhandlerObj<propvalueset_t> > callbacks_t;

	//! Handlers that get notified when the value changes

	callbacks_t callbacks;

	//! Constructor

	nodeObj(//! Initial value
		const std::string &valArg,

		//! The entry in the hierarchical property tree
		const ref<nodehierObj> hierpArg);

	//! Destructor
	~nodeObj() noexcept;
};

//! A hierarchical tree of properties

class nodehierObj : virtual public obj {

	ptr<nodehierObj> parent; //!< The parent node, null for the root node.

public:

	weakptr<ptr<nodeObj> > value; //!< This property's value, if defined

	//! A container of all children of this node, in the hierarchical tree

	//! It should be a weakmap, except for the occasion where a destroyed
	//! map entry isn't purged yet.
	typedef weakmultimap<std::string, nodehierObj> children_t;

	//! All children of this node, in the hierarchical tree
	children_t children;

	//! Constructor
	nodehierObj(const ptr<nodehierObj> &parentArg);

	//! Destructor
	~nodehierObj() noexcept;
};

//! A list of defined properties

class listObj : virtual public obj {

	//! Root node of the hierarchical property tree

	ref<nodehierObj> rootnode;

	//! A container for a map of all nodes in the hierarchical property tree
	typedef weakmap<std::string, nodehierObj> prophier_t;

	//! A map of all nodes in the hierarchical property tree
	prophier_t prophier;

	//! Find a node

	ref<nodehierObj> findhier_locked(const std::list<std::string> &hier,
					 //!< Hierarchical name,

					 std::string &n //!< Reconstituted name
					 );

	//! A container for all loaded properties
	typedef std::map<std::string,ref<nodeObj> > loaded_t;

	//! All loaded properties
	loaded_t loaded;

	//! A wrapper for a property name input iterator

	//! When parsing a property name, make an iterator that compares
	//! equal to an ending iterator when the underlying sequence contains
	//! a newline, #, or = character.

	template<typename iter_type>
	class parsepropname_iter : std::iterator<std::input_iterator_tag,
						 char,
						 void, void, void> {

		//! Current iterator value
		iter_type cur_iter;

		//! Ending iterator value
		iter_type end_iter;

		//! Whether this iterator is an ending iterator
		bool ending() const
		{
			if (cur_iter != end_iter)
				switch (*cur_iter) {
				case '\n':
				case '#':
				case '=':
					break;
				default:
					return false;
				}
			return true;
		}

		//! Helper class for postfix ++ operator
		class dummy {

		public:
			//! Return value

			typename iter_type::value_type c;

			//! Iterator operator

			typename iter_type::value_type
			operator*() const noexcept { return c; }

			//! Constructor
			dummy(typename iter_type::value_type cArg)
				noexcept : c(cArg) {}

			//! Destructor
			~dummy() noexcept {}
		};

	public:
		//! Constructor
		parsepropname_iter(//! Iterator
				   iter_type cur_iterArg,

				   //! Ending iterator value
				   iter_type end_iterArg)
			: cur_iter(cur_iterArg),
			  end_iter(end_iterArg)
		{
		}

		//! Destructor
		~parsepropname_iter() noexcept
		{
		}

		//! Iterator operator
		bool operator==(const parsepropname_iter<iter_type> &o)
			const
		{
			return ending() && o.ending();
		}

		//! Iterator operator
		bool operator!=(const parsepropname_iter<iter_type> &o)
			const
		{
			return !operator==(o);
		}

		//! Iterator operator
		typename iter_type::value_type operator*()
			const
		{
			return *cur_iter;
		}

		//! Iterator operator
		parsepropname_iter<iter_type> &operator++()
		{
			++cur_iter;
			return *this;
		}

		//! Iterator operator
		dummy operator++(int)
		{
			dummy val= operator*();

			++*this;
			return val;
		}

		//! Retrieve the current underlying iterator

		iter_type iter() const
		{
			return cur_iter;
		}
	};

public:
	//! Parse a sequence that defines properties

	template<typename iter_type> iter_type
	parse(iter_type beg_iter, //!< Beginning iterator
	      iter_type end_iter, //!< Ending iterator

	      const std::string &location,
	      //!< Iterator sequence label, like a filename

	      bool do_update,
	      //!< Whether existing properties should be overriden

	      bool do_create,
	      //!< Whether nonexisting properties should be created

	      //! Error handler
	      const errhandler &errh=errhandler::errlog(),

	      //! Locale for interpreting properties
	      const const_locale &l=locale::base::global())
	{
		loaded_t parsed;

		beg_iter=lock_parse(beg_iter, end_iter, location,
				    do_update, do_create, parsed,
				    errh, l);
		update(parsed, errh, l);

		return beg_iter;
	}

	//! Load properties from a string
	void load(const std::string &properties, //!< Property string to parse

		  bool update, //!< Whether existing properties should be overriden

		  bool create,	//!< Whether nonexisting properties should be created

		  //! Error handler
		  const errhandler &errh=errhandler::errlog(),

		  //! Locale for interpreting properties
		  const const_locale &l=locale::base::global()
		  );

	//! Load properties from a stream
	void load(std::istream &i,
		  //!< Stream to parse

		  const std::string &location, //!< Name of stream, the filename

		  bool update, //!< Whether existing properties should be overriden

		  bool create,	//!< Whether nonexisting properties should be created

		  //! Error handler
		  const errhandler &errh=errhandler::errlog(),

		  //! Locale for interpreting properties
		  const const_locale &l=locale::base::global()
		  );

	//! Load properties from a file
	void load_file(const std::string &filename, //!< The filename

		       bool update, //!< Whether existing properties should be overriden

		       bool create, //!< Whether nonexisting properties should be created

		       //! Error handler
		       const errhandler &errh=errhandler::errlog(),

		       //! Default locale for interpreting the property file, may be overridden by "#LOCALE="

		       const const_locale &l=locale::base::global()
		       );
protected:

	//! Load properties from a file

	//! \internal
	//!
	void load_file(std::ifstream &i,
		       const std::string &filename,
		       bool update,
		       bool create,
		       const errhandler &errh,
		       const const_locale &l);

public:
	//! Set a single, explicit property
	void load(const std::string &n, //!< Property name
		  const std::string &v, //!< Property value
		  bool update, //!< Whether existing properties should be overriden

		  bool create,	//!< Whether nonexisting properties should be created

		  //! Error handler
		  const errhandler &errh=errhandler::errlog(),

		  //! Locale for interpreting properties
		  const const_locale &l=locale::base::global()
		  );

	friend class lockObj;

	//! Enumerate existing properties
	void enumerate(std::map<std::string, std::string> &properties)
;

	class iterator;

private:
	//! An iterator over the property tree

	class iteratorObj : virtual public obj {

	public:
		//! The property list
		list l;

		//! Name of this node
		std::string nodename;

		//! Node pointer
		ptr<nodehierObj> p;

		//! Constructor
		iteratorObj(const list &lArg,
			    const std::string &nodenameArg,
			    const ptr<nodehierObj> &pArg) LIBCXX_INTERNAL;

		~iteratorObj() noexcept LIBCXX_INTERNAL;
	};

public:
	//! A reference-counted iterator over the property tree

	class iterator {

		//! The real iterator data
		ref<iteratorObj> iter;

		//! Constructor
		iterator(const ref<iteratorObj> &iterArg) LIBCXX_INTERNAL;

	public:
		friend class listObj;

		//! Return this node's name

		std::string propname() const;

		//! Return this node's value

		//! first - the value
		//! second - a bool, \c true if the property is not set (and
		//! first is an empty string).

		std::pair<std::string, bool> value() const;

		//! The container of this property's children properties
		typedef std::map<std::string, iterator> children_t;

		//! Return this property's children properties
		void children(children_t &childrenArg) const;

		//! Convenience function

		//! If the requested child does not exist, the returned
		//! iterator's propname() is an empty string.
		template<typename arg_type>
		iterator child(const arg_type &arg,
			       const const_locale &localeArg=locale::base::global())
			const
		{
			return get(stringize<arg_type>
				   ::tostr(arg, localeArg));
		}

	private:
		//! Retrieve the given children
		iterator get(const std::string &childname)
			const;
	};

	//! Return the root node of the property tree

	iterator root();

	//! Return an iterator for a specific property

	//! The returned iterator's propname() is empty if the property is not
	//! set.
	template<typename arg_type>
	iterator find(//! Property name
		      const arg_type &propname,

		      //! Locale for parsing the property name
		      const const_locale &l=locale::base::global())

	{
		return find_internal(stringize<arg_type>
				     ::tostr(propname, l), l);
	}

private:
	//! Return an iterator for a specific property

	//! The returned iterator's propname() is empty if the property is not
	//! set.
	iterator find_internal(//! Property name
			       const std::string &propname,

			       //! Locale for parsing the property value
			       const const_locale &l);

	//! Enumerate existing properties
	void enumerate(std::map<std::string, std::string> &properties,
		       const iterator &iter);

	//! Parse a sequence that defines properties while this object is locked

	template<typename iter_type> iter_type
	lock_parse(iter_type beg_iter,
		   iter_type end_iter,
		   const std::string &location,
		   bool update,
		   bool create,
		   loaded_t &parsed,
		   const errhandler &errh,
		   const const_locale &l)
	{
		std::lock_guard<std::mutex> lock(objmutex);

		size_t linenum=0;

		parsed.clear();

		while (beg_iter != end_iter)
		{
			++linenum;

			std::list<std::string> hier;
			ptr<nodehierObj> p;
			std::string n;
			std::string v;
			size_t nonspcpos=0;

			// Parse name+value, first

			beg_iter=parsepropname(parsepropname_iter<iter_type>
					       (beg_iter, end_iter),
					       parsepropname_iter<iter_type>
					       (end_iter, end_iter),
					       hier, l).iter();

			if (hier.empty())
			{
				if (beg_iter != end_iter && *beg_iter == '=')
					errh(location, linenum,
					     "Syntax error");
				goto eol;
			}


			if (beg_iter == end_iter || *beg_iter != '=')
			{
				errh(location, linenum, "Syntax error");
				goto eol;
			}

			do
			{
				++beg_iter;
			} while (beg_iter != end_iter &&
				 (*beg_iter == ' ' || *beg_iter == '\t'
				  || *beg_iter == '\r'));

			while (beg_iter != end_iter &&
			       *beg_iter != '\n' &&
			       *beg_iter != '#')
			{
				auto c=*beg_iter++;

				if (c == '\\' && beg_iter != end_iter)
				{
					auto c= *beg_iter++;

					if (c == 'n')
					{
						v.push_back('\n');
					}
					else if (c == 'r')
					{
						v.push_back('\r');
					}
					else if (c == 't')
					{
						v.push_back('\t');
					}
					else if (c >= '0' && c <= '7')
					{
						c -= '0';

						if (beg_iter != end_iter &&
						    *beg_iter >= '0' &&
						    *beg_iter <= '7')
						{
							c=c*8 + *beg_iter++-'0';
						}
						v.push_back(c);
					}
					else
					{
						v.push_back(c);
					}
				}
				else
				{
					v.push_back(c);

					if (c == '\t' || c == ' ' ||
					    c == '\n' || c == '\r')
						continue;
				}
				nonspcpos=v.size();
			}

			update_locked(hier, v.substr(0, nonspcpos),
				      parsed, update, create);

		eol:
			while (beg_iter != end_iter)
				if (*beg_iter++ == '\n')
					break;
		}

		return beg_iter;
	}

	//! Main meat of parsed()

	void update_locked(const std::list<std::string> &hier,
			   //!< Hierarchical name

			   const std::string &v, //!< Parsed value

			   loaded_t &parsed, //!< Parsed value accumulator

			   bool update, //!< Whether existing properties should be overriden

			   bool create	//!< Whether nonexisting properties should be created
			   );

	//! Process the results of parse()

	void update(const loaded_t &loaded,
		    const errhandler &errh,
		    const const_locale &l);
public:
	//! Constructor
	listObj();

	//! Destructor
	~listObj() noexcept;

	//! Install a callback for a property

	//! Each time the property value changes, the callback gets invoked.
	//!
	//! \return a reference to the property value node object.

	ref<nodeObj>
	install(const ref<eventhandlerObj<propvalueset_t> > &callback,
		//!< Callback to install

		const std::string &propname, //!< Property name

		const std::string &initialvalue,
		//!< Default property value. If the property exists, the
		//!< callback gets invoked immediately. If the property
		//!< does not exist, it gets created with the given value
		//!< and the callback does NOT get invoked

		//! Locale for interpreting properties
		const const_locale &l=locale::base::global()
		);

	//! Return the global property list

	//! May return an empty list during application shutdown.
	//!

	static listptr global();
};

#ifndef DOXYGEN
#define LIBCXX_TEMPLATE_DECL extern
#include <x/property_list_t.H>
#undef LIBCXX_TEMPLATE_DECL
#endif

#if 0
{
	{
#endif
	}
}
#endif
