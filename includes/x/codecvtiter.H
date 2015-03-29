/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_codecvtiter_H
#define x_codecvtiter_H

#include <x/basiciofiltercodecvtin.H>
#include <x/basiciofiltercodecvtout.H>
#include <x/basiciofilteriter.H>

#include <iterator>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Default buffer size for ocodecvtiter iterators.

extern size_t iofilter_codecvtbufsize();

//! Conversion buffer for \ref ocodecvtiter "ocodecvtiter".

//! The first template parameter is an iofilteradapterObj. The second parameter
//! is a \ref basic_codecvtin "basic_codecvtin" or a
//! \ref basic_codecvtout "basic_codecvtout" class.
//!
//! This template defines a class that multiply-inherits from these two
//! classes, and marries them together, implementing an \c iofilter that
//! converts between narrow and wide characters.

template<typename adapterobj_t, typename codecvt_t>
class codecvtiter_bufObj : public adapterobj_t, public codecvt_t {

public:
	//! Constructor
	codecvtiter_bufObj(//! Locale for conversion
			   const const_locale &localeRef)
		: adapterobj_t(iofilter_codecvtbufsize()),
		  codecvt_t(localeRef) {}

	//! Destructor
	~codecvtiter_bufObj() noexcept {}

	//! iofilter's filter() method implementation

	//! This stub invokes codecvt_t's filter() method.
	//!
	void filter()
	{
		codecvt_t::filter();
	}
};

extern template class codecvtiter_bufObj<iofilteradapterObj<char, wchar_t>,
					 basic_codecvtin<std::codecvt<wchar_t,
								      char,
								      mbstate_t>
							 > >;

extern template class codecvtiter_bufObj<iofilteradapterObj<wchar_t, char>,
					 basic_codecvtout<std::codecvt<wchar_t,
								       char,
								       mbstate_t
								       > > >;
//! An output iterator that converts between narrow and wide characters

//! The first template parameter is any output iterator class that receives
//! either narrow or wide characters. The second template parameter is either
//! is a \ref basic_codecvtin "basic_codecvtin" or a
//! \ref basic_codecvtout "basic_codecvtout" class, depending upon whether
//! the first template parameter receives narrow
//! (\ref basic_codecvtout "basic_codecvtout") or wide
//! (\ref basic_codecvtin "basic_codecvtin") characters.
//!
//! This template defines \ref iter "iter",
//! an output iterator that takes either narrow or
//! wide characters, as appropriate, performs the specified conversion,
//! then writes the converted characters to the output iterator specified
//! by the first template parameter.
//!
//! This template defines create(), that takes an output iterator and
//! returns \ref iter "iter".
//! Internal
//! buffering is used. After writing the entire output to the output iterator
//! returned by create(),
//! invoke flush() to flush out the buffer and return the original output
//! iterator (after being modified by the converted output sequence).
//!
//! After flush() the iterator becomes unusable.

template<typename out_iter, typename codecvt_t>
class basicocodecvtiter {


public:

	//! Iterator implementation

	typedef obasiciofilteriter<out_iter,
				   typename codecvt_t::input_type,
				   typename codecvt_t::output_type> iter;

	//! Create the output iterator

	static iter create(//! Output iterator for the converted character sequence
			   const out_iter &iterArg,

			   //! Locale that defines the mapping between narrow and wide characters
			   const const_locale &localeArg=locale::base::global())

	{
		return iter(ref<codecvtiter_bufObj
				<typename iter::adapterobj_t, codecvt_t> >
				    ::create(localeArg), iterArg);
	}
};

//! No-op output iterator, used by codecvtiter_bytype specializations

template<typename out_iter, typename input_type_t>
class basicocodecvtiter_noop {


public:

	//! The implementation class, a no-op iterator

	class iter : public std::iterator<std::output_iterator_tag,
					  input_type_t,
					  void, void, void > {

		//! Saved output iterator
		out_iter iterRef;

	public:
		//! Constructor

		iter(//! Output iterator for the converted character sequence
		     const out_iter &iterRefArg)
 : iterRef(iterRefArg)
		{
		}

		//! Default constructor
		iter() {}

		//! Destructor
		~iter() noexcept {}

		//! Iterator operator
		iter &operator++(int) noexcept { return *this; }

		//! Iterator operator
		iter &operator++() noexcept { return *this; }

		//! Iterator operator
		iter &operator*() noexcept { return *this; }

		//! Iterator operator

		void operator=(input_type_t val)
		{
			*iterRef++=val;
		}

		//! No-op

		out_iter flush()
		{
			return iterRef;
		}
	};

	//! Instantiate a no-op operator

	static iter create(//! Output iterator for the converted character sequence
			   const out_iter &iterArg,

			   //! Ignored
			   const const_locale &localeArg=locale::base::global())

	{
		return iter(iterArg);
	}
};

//! Output iterators for converting between narrow and wide characters

//! The first template parameter is an output iterator. The class defines
//! \c ctow_iter_t and \c wtoc_iter_t. Both of these are output iterators
//! that convert between narrow and wide characters, and write results of the
//! conversion into the output iterator instance.

template<typename out_iter,
	 typename codecvt_t=std::codecvt<wchar_t, char, mbstate_t> >
class ocodecvtiter {

public:

	//! Define an output iterator for converting narrow to wide characters

	typedef basicocodecvtiter<out_iter,
				  basic_codecvtin<codecvt_t> > ctow_iter_t;

	//! Define an output iterator for converting wide to narrow characters

	typedef basicocodecvtiter<out_iter,
				  basic_codecvtout<codecvt_t> > wtoc_iter_t;
};

//! Output iterators for converting between narrow and wide characters

//! This is an alternative template to \ref ocodecvtiter that selects the
//! appropriate implementation class by the input character type and the
//! output character type.

template<typename out_iter,
	 typename input_type,
	 typename output_type,
	 typename codecvt_t=std::codecvt<wchar_t, char, mbstate_t> >
class ocodecvtiter_bytype {};

//! Specialization for narrow to wide character output iterator

template<typename out_iter, typename codecvt_t>
class ocodecvtiter_bytype<out_iter, char, wchar_t, codecvt_t> {

public:
	//! Narrow to wide character output iterator

	typedef typename ocodecvtiter<out_iter, codecvt_t>::ctow_iter_t iter_t;
};

//! Specialization for wide to narrow character output iterator

template<typename out_iter, typename codecvt_t>
class ocodecvtiter_bytype<out_iter, wchar_t, char, codecvt_t> {

public:
	//! Wide to narrow character output iterator

	typedef typename ocodecvtiter<out_iter, codecvt_t>::wtoc_iter_t iter_t;
};

//! No-op specialization

template<typename out_iter, typename char_type, typename codecvt_t>
class ocodecvtiter_bytype<out_iter, char_type, char_type, codecvt_t> {

public:
	//! No-op iterator

	typedef basicocodecvtiter_noop<out_iter, char_type> iter_t;
};

// ---------------------------------------------------------------------------

//! An input iterator that converts between narrow and wide characters

//! The first template parameter is any input iterator class that iterates over
//! either a narrow or wide character sequence.
//! The second template parameter is either
//! is a \ref basic_codecvtin "basic_codecvtin" or a
//! \ref basic_codecvtout "basic_codecvtout" class, depending upon whether
//! the first template parameter receives narrow
//! (\ref basic_codecvtin "basic_codecvtin") or wide
//! (\ref basic_codecvtout "basic_codecvtout") characters.
//!
//! This template defines \ref iter "iter",
//! an output iterator that takes either narrow or
//! wide characters, as appropriate, performs the specified conversion,
//! then writes the converted characters to the output iterator specified
//! by the first template parameter.
//!
//! This template defines create(), that takes a beginning and
//! an ending iterator and
//! returns \ref iter "iter" representing the beginning iterator of the
//! converted input sequence. Instantiate the \ref iter "iter" class using
//! the default constructor to obtain the ending iterator.

template<typename in_iter, typename codecvt_t>
class basicicodecvtiter {


public:
	//! Iterator implementation

	typedef ibasiciofilteriter<in_iter,
				   typename codecvt_t::input_type,
				   typename codecvt_t::output_type> iter;


	//! Create the input iterator

	static iter create(//! Beginning iterator for the input sequence
			   const in_iter &beg_iterArg,

			   //! Ending iterator for the input sequence
			   const in_iter &end_iterArg,

			   //! Locale that defines the mapping between narrow and wide characters
			   const const_locale &localeArg=locale::base::global())

	{
		return iter(ref<codecvtiter_bufObj
				<typename iter::adapterobj_t, codecvt_t> >
				    ::create(localeArg), beg_iterArg,
				    end_iterArg);
	}
};

//! No-op input iterator, used by icodecvtiter_bytype specializations

template<typename in_iter, typename input_type_t>
class basicicodecvtiter_noop {

public:
	//! The implementation class, a no-op iterator

	class iter : public std::iterator<std::input_iterator_tag,
					  input_type_t,
					  void, void, void > {
		//! Saved input iterator
		in_iter beg_iter;

		//! Saved ending iterator
		in_iter end_iter;

		//! A helper object for the post-increment operator

		class placeholder {

		public:
			//! Value being saved

			input_type_t val;

			//! Constructor
			placeholder(input_type_t valArg) noexcept
				: val(valArg) {}

			//! Destructor
			~placeholder() noexcept {}

			//! The post-increment * operator returns the value.

			input_type_t operator*() const noexcept { return val; }
		};
	public:

		//! Constructor
		iter(//! Beginning iterator for the input sequence
		     const in_iter &beg_iterArg,

		     //! Ending iterator for the input sequence
		     const in_iter &end_iterArg)
			: beg_iter(beg_iterArg), end_iter(end_iterArg)
		{
		}

		//! Default constructor
		iter() : beg_iter(in_iter()),
			end_iter(beg_iter) {}

		//! Destructor
		~iter() noexcept {}

		//! Iterator operator
		placeholder operator++(int) noexcept
		{
			return placeholder(*beg_iter++);
		}

		//! Iterator operator
		iter &operator++() noexcept
		{
			++beg_iter;
			return *this;
		}

		//! Iterator operator
		input_type_t operator*() noexcept
		{
			return *beg_iter;
		}


		//! Iterator operator
		bool operator==(const iter &o) const noexcept
		{
			return beg_iter == end_iter && o.beg_iter == o.end_iter;
		}

		//! Iterator operator
		bool operator!=(const iter &o) const noexcept
		{
			return !operator==(o);
		}
	};

	//! Instantiate a no-op operator

	static iter create(//! Beginning iterator for the input sequence
			   const in_iter &beg_iterArg,

			   //! Ending iterator for the input sequence
			   const in_iter &end_iterArg,

			   //! Ignored
			   const const_locale &localeArg=locale::base::global())

	{
		return iter(beg_iterArg, end_iterArg);
	}
};

//! Input iterators for converting between narrow and wide characters

//! The first template parameter is an input iterator. The class defines
//! \c ctow_iter_t and \c wtoc_iter_t. Both of these are input iterators
//! that convert between narrow and wide characters. The input iterators
//! convert an input sequence defined by the template parameter, and produce
//! an input iterator that iterates over the converted character sequence.

template<typename in_iter,
	 typename codecvt_t=std::codecvt<wchar_t, char, mbstate_t> >
class icodecvtiter {

public:

	//! Define an input iterator for converting narrow to wide characters

	typedef basicicodecvtiter<in_iter,
				  basic_codecvtin<codecvt_t> > ctow_iter_t;

	//! Define an input iterator for converting wide to narrow characters

	typedef basicicodecvtiter<in_iter,
				  basic_codecvtout<codecvt_t> > wtoc_iter_t;
};


//! Input iterators for converting between narrow and wide characters

//! This is an alternative template to \ref icodecvtiter that selects the
//! appropriate implementation class by the input character type and the
//! output character type.

template<typename in_iter,
	 typename input_type,
	 typename output_type,
	 typename codecvt_t=std::codecvt<wchar_t, char, mbstate_t> >
class icodecvtiter_bytype {};

//! Specialization for narrow to wide character output iterator

template<typename in_iter, typename codecvt_t>
class icodecvtiter_bytype<in_iter, char, wchar_t, codecvt_t> {

public:
	//! Narrow to wide character output iterator

	typedef typename icodecvtiter<in_iter, codecvt_t>::ctow_iter_t iter_t;
};

//! Specialization for wide to narrow character output iterator

template<typename in_iter, typename codecvt_t>
class icodecvtiter_bytype<in_iter, wchar_t, char, codecvt_t> {

public:
	//! Wide to narrow character output iterator

	typedef typename icodecvtiter<in_iter, codecvt_t>::wtoc_iter_t iter_t;
};

//! No-op specialization

template<typename in_iter, typename char_type, typename codecvt_t>
class icodecvtiter_bytype<in_iter, char_type, char_type, codecvt_t> {

public:
	//! No-op iterator

	typedef basicicodecvtiter_noop<in_iter, char_type> iter_t;
};

#if 0
{
#endif
}
#endif
