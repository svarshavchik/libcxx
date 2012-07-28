/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_getlinecrlf_H
#define x_getlinecrlf_H

#include <string>
#include <istream>
#include <iterator>
#include <x/namespace.h>
#include <x/iter_value.H>
#include <x/exception.H>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

//! Read a LF-terminated line from an input iterator

//! This is like std::getline(), except that the line is read from an
//! input iterator.

template<typename iter_type, typename container_type>
std::pair<iter_type, bool> getlinelf(//! Beginning input iterator
				     iter_type beg_iter,

				     //! Ending input iterator
				     iter_type end_iter,

				     //! The container to receive the line.

				     //! This is typically a \c std::string.
				     //!
	    
				     container_type &cont)

{
	typedef typename iter_value_type<iter_type>::value_t char_type;

	const char_type lf('\n');

	cont.erase();

	bool ok=false;

	while (beg_iter != end_iter)
	{
		ok=true;

		char_type c= *beg_iter++;

		if (c == lf)
			break;

		cont.push_back(c);
	}
	return std::make_pair(beg_iter, ok);
}

//! Read a LF-terminated line from an input stream

//! This is not much different from std::getline(). It's here for completeness
//! because of getlinecrlf().
//!
//! Some semantics regarding stream state is probably different than what
//! std::getline does.

template<typename char_type, typename traits_type, typename container_type>
std::basic_istream<char_type, traits_type> &
getlinelf(//! Input stream
            std::basic_istream<char_type, traits_type> &i,

            //! The container to receive the line.

            //! This is typically a \c std::string.
            //!
            
            container_type &cont)
{
	typedef std::istreambuf_iterator<char_type, traits_type> iter_type;

	if (!getlinelf(iter_type(i), iter_type(), cont).second)
		i.setstate(std::ios_base::failbit);
	return i;
}


//! Read a CRLF-terminated line from an input iterator

//! This is like std::getline(), except that the line terminator is the
//! \c CRLF sequence. Lone newlines are placed in the string.
//! The \c CRLF sequence is not placed into the string.
//!
//! Many Internet protocols use CRLF-terminated lines. This function implements
//! a compliant parser.

template<typename iter_type, typename container_type>
std::pair<iter_type, bool> getlinecrlf(//! Beginning input iterator
				       iter_type beg_iter,

				       //! Ending input iterator
				       iter_type end_iter,

				       //! The container to receive the line.

				       //! This is typically a \c std::string.
				       //!
	    
				       container_type &cont)

{
	typedef typename iter_value_type<iter_type>::value_t char_type;

	const char_type cr('\r');
	const char_type lf('\n');

	cont.erase();

	bool ok=false;

	while (beg_iter != end_iter)
	{
		ok=true;

		char_type c= *beg_iter++;

		if (c == cr)
		{
			if (beg_iter != end_iter && *beg_iter == lf)
			{
				++beg_iter;
				break;
			}
		}

		cont.push_back(c);
	}
	return std::make_pair(beg_iter, ok);
}

//! Read a CRLF-terminated line from an input stream

//! This is like std::getline(), except that the line terminator is the
//! \c CRLF sequence. Lone newlines are placed in the string.
//! The \c CRLF sequence is not placed into the string.
//!
//! Many Internet protocols use CRLF-terminated lines. This function implements
//! a compliant parser.

template<typename char_type, typename traits_type, typename container_type>
std::basic_istream<char_type, traits_type> &
getlinecrlf(//! Input stream
            std::basic_istream<char_type, traits_type> &i,

            //! The container to receive the line.

            //! This is typically a \c std::string.
            //!
            
            container_type &cont)
{
	typedef std::istreambuf_iterator<char_type, traits_type> iter_type;

	if (!getlinecrlf(iter_type(i), iter_type(), cont).second)
		i.setstate(std::ios_base::failbit);
	return i;
}

#ifndef DOXYGEN
extern template std::istream &getlinecrlf(std::istream &, std::string &)
;

extern template std::pair<std::istreambuf_iterator<char>, bool>
getlinecrlf(std::istreambuf_iterator<char>,
	    std::istreambuf_iterator<char>,
	    std::string &);

extern template std::istream &getlinelf(std::istream &, std::string &)
;

extern template std::pair<std::istreambuf_iterator<char>, bool>
getlinelf(std::istreambuf_iterator<char>,
	    std::istreambuf_iterator<char>,
	    std::string &);

#endif

#if 0
{
#endif
}
#endif
