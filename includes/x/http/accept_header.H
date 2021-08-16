/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_accept_header_H
#define x_http_accept_header_H

#include <list>
#include <map>
#include <set>
#include <string>
#include <iomanip>
#include <algorithm>
#include <x/headersbasefwd.H>
#include <x/exception.H>
#include <x/chrcasecmp.H>
#include <x/http/messageimpl.H>
#include <x/mime/parameter.H>
#include <x/namespace.h>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif


//! Parsed contents of an HTTP Accept: header

//! See RFC 2616, section 4.1
//!

class accept_header {

	//! The "Accept" header name

	//! \internal
	//!
	static const char name[] LIBCXX_HIDDEN;
public:

	//! media parameters and accept parameters

	//! Keyed by case-insensitive attribute name,
	//! value is the attribute value

	typedef std::multiset<mime::parameter_t> parameters_t;

	//! A media candidate

	struct media_type_t {

	public:

		//! Media type
		std::string type;

		//! Media subtype
		std::string subtype;

		//! Media parameters
		parameters_t media_parameters;

		media_type_t() noexcept
		{
		}

		//! Convenient initializer

		media_type_t(//! "type/subtype; parameter=value"

			     const std::string &value);

		//! Convenient initializer

		media_type_t(//! "type/subtype; parameter=value"

			     const char *value);

		//! Convert media type to a string

		template<typename iter_type>
		iter_type to_string(//! Output iterator
				   iter_type iter) const
		{
			iter=std::copy(type.begin(), type.end(), iter);
			*iter++='/';
			iter=std::copy(subtype.begin(), subtype.end(), iter);

			for (parameters_t::const_iterator
				     b(media_parameters.begin()),
				     e(media_parameters.end()); b != e; ++b)
			{
				*iter++ = ';';
				*iter++ = ' ';
				iter=b->to_string(iter);
			}
			return iter;
		}

	private:
		//! Common initializer
		void init(const std::string &value);

	};

	//! A single parsed media entry in the Accept: header.

	struct accept_entry_t : public media_type_t {

		//! The normalized media quality value

		//! Implied decimal point, three digits of precision.
		//! "q=0.500" is 500, "q=0.3" is 300. "q=1" is 1000.

		unsigned qvalue;

		//! The accept parameters

		parameters_t accept_params;

		//! Default constructor

		accept_entry_t() noexcept : qvalue(1000) // Default qvalue of 1
		{
		}

		//! Verify whether the given media type candidate is acceptable

		//! \return \c true if every entry in media_parameters is
		//! present in the candidate (but the candidate may have others,
		//! which aare ignored).

		bool check_parameters(const media_type_t &candidate)
			const
		{
			for (parameters_t::const_iterator
				     b(media_parameters.begin()),
				     e(media_parameters.end()); b != e; ++b)
				if (candidate.media_parameters.find(*b)
				    == candidate.media_parameters.end())
					return false;
			return true;
		}

		//! Convert media type to a string

		template<typename iter_type>
		iter_type to_string(//! Output iterator
				   iter_type iter) const
		{
			iter=media_type_t::to_string(iter);

			std::ostringstream o;

			o << "; q=" << (qvalue / 1000) << '.'
			  << std::setw(3) << std::setfill('0')
			  << (qvalue % 1000);

			std::string s(o.str());

			iter=std::copy(s.begin(), s.end(), iter);

			for (parameters_t::const_iterator
				     b(accept_params.begin()),
				     e(accept_params.end()); b != e; ++b)
			{
				*iter++ = ';';
				*iter++ = ' ';
				iter=b->to_string(iter);
			}
			return iter;
		}

	};

private:

	//! Accept: header entries are keyed by case insensitive type and subtype

	class accept_comparator_t {

	public:
		//! Comparison function

		bool operator()(const accept_entry_t &a,
				const accept_entry_t &b) const noexcept
		{
			int rc=chrcasecmp::compare(a.type, b.type);

			if (rc == 0)
				rc=chrcasecmp::compare(a.subtype, b.subtype);
			return rc < 0;
		}
	};

public:
	//! Container for values parsed from the HTTP Accept: header

	typedef std::multiset<accept_entry_t, accept_comparator_t> list_t;

	//! Values parsed from the HTTP Accept: header

	list_t list;

	//! Default constructor
	accept_header() noexcept;

	//! Default destructor
	~accept_header();

	//! Construct from HTTP headers
	accept_header(const headersbase &);

	//! Construct from HTTP headers
	accept_header &operator=(const headersbase &);

	//! Construct from a string
	accept_header(const std::string &);

	//! Construct from a string
	accept_header(const char *);

	//! Construct from a string
	accept_header &operator=(const char *);

	//! Construct from a string
	accept_header &operator=(const std::string &);

	//! Convert the Accept: header value to a single string

	template<typename iter_type>
	iter_type to_string(//! Output iterator
			   iter_type iter) const
	{
		const char *sep_b="", *sep_e=sep_b;

		for (list_t::const_iterator b(list.begin()), e(list.end());
		     b != e; ++b)
		{
			iter=std::copy(sep_b, sep_e, iter);
			sep_b=", ";
			sep_e=sep_b+2;
			iter=b->to_string(iter);
		}
		return iter;
	}
private:

	//! Helper object for filtering media type candidates

	//! Processes the results of looking up a candidates type/subtype in
	//! the accept entry list. Collects a list of candidates that are
	//! found to be acceptable. If this candidate is acceptable but its
	//! matching entry has a
	//! higher qvalue, the list is clear and the candidate is placed in the
	//! candidate_list. If the qvalues are the same, the accept entry with
	//! the large number of media parameters is the tiebreaker.
	//!
	//! This template parameter is the candidate iterator type: a forward
	//! iterator that iterates over objects which are convertible to
	//! accept_entry_t.

	template<typename cand_iter_t>
	class finder {

	public:
		//! Current candidate list's qvalue
		unsigned candidatesq;

		//! Tiebreaker for equal values: number of media parameters
		size_t precision;

		//! List of all acceptable candidates with the highest qvalue and matching media parameters
		std::list<cand_iter_t> candidate_list;

		finder() noexcept : candidatesq(1), // Ignore 0 qvalue entries
			precision(0)
		{
		}

		//! Process a media type candidate

		//! \return \c true if the specified results of equal_range()
		//! search contain at list one container member (it may or may
		//! not've been selected because of its qvalue).

		bool validate_candidate(//! A forward iterator to the candidate
					const cand_iter_t &candidate,

					//! Results of equal_range() of candidate-referenced media_type_t on the accept list.

					const std::pair<list_t::const_iterator,
							list_t::const_iterator>
					&accept_iter)

		{
			list_t::const_iterator b(accept_iter.first),
				e(accept_iter.second);

			bool found=false;

			const media_type_t &candobj=*candidate;

			for ( ; b != e; ++b)
			{
				const accept_entry_t &en=*b;

				found=true;

				if (en.qvalue < candidatesq)
					continue;

				if (!b->check_parameters(candobj))
					continue;

				size_t match_precision=
					en.media_parameters.size();

				if (en.qvalue == candidatesq &&
				    match_precision < precision)
					continue;

				if (en.qvalue > candidatesq ||
				    (en.qvalue == candidatesq &&
				     match_precision > precision))
				{
					candidate_list.clear();
					candidatesq=en.qvalue;
					precision=match_precision;
				}
				candidate_list.push_back(candidate);
			}
			return found;
		}
	};

public:
	//! Calculate the best media type from those amongst the listed

	//! The first two parameters are forward iterators for objects that
	//! are convertible to media_type_t. The third parameter is an
	//! output iterator that iterates over the first two iterators' type.
	//!
	//! \return the output iterator after putting the subset of the
	//! range defined by the first two forward iterators for media types
	//! that are the best match for the list specified by the accept
	//! header.

	template<typename cand_iter_t,
		 typename output_iter_t>
	output_iter_t find(cand_iter_t beg_iter,
			   cand_iter_t end_iter,
			   output_iter_t output_iter)
	{
		if (list.empty())
		{
			while (beg_iter != end_iter)
			{
				*output_iter++=beg_iter;
				++beg_iter;
			}

			return output_iter;
		}

		finder<cand_iter_t> helper;

		accept_entry_t lookup;

		lookup.type=lookup.subtype="*";

		std::pair<list_t::const_iterator,
			  list_t::const_iterator>
			wildcard(list.equal_range(lookup));

		for (; beg_iter != end_iter; ++beg_iter)
		{
			media_type_t &obj=*beg_iter;

			lookup.type=obj.type;
			lookup.subtype=obj.subtype;

			if (helper.validate_candidate(beg_iter,
						      list.equal_range(lookup)))
				continue;

			lookup.subtype="*";

			if (helper.validate_candidate(beg_iter,
						      list.equal_range(lookup)))
				continue;

			helper.validate_candidate(beg_iter, wildcard);
		}

		return std::copy(helper.candidate_list.begin(),
				 helper.candidate_list.end(),
				 output_iter);
	}

private:

	//! Helper object for parsing the HTTP request header
	class parser;
};

#if 0
{
#endif
}
#endif
