/*
** Copyright 2012 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_basiciofilteriter_H
#define x_basiciofilteriter_H

#include <x/basiciofilter.H>

#include <iterator>

namespace LIBCXX_NAMESPACE {

#if 0
};
#endif

//! Base class for adapting \ref iofilteradapterObj "IO filters" as output iterators

//! This base class provides
//! support for a subclass that implements an output iterator
//! which collects the output sequence
//! into an IO filter's input buffer, uses the IO filter to filter the sequence
//! then have the subclass collect the filtered output sequence.
//!
//! The template parametersa are the IO filter input sequence value type and
//! the output sequence value type. The constructor receives a reference to
//! the corresponding IO filter adapter object.
//!
//! The subclass invokes collect() to receive each value in the output sequence.
//! collect() places the values into the output buffer, invokes the IO filter,
//! when needed, then invokes
//! flush(output_type_t *out_seq, size_t cnt_seq), implemented in the subclass
//! that consumes the filtered output sequence.
//!
//! This base class also implements flush(), that filters the collected,
//! but unfiltered output sequence, and sends the left over filtered sequence
//! to flush(), for the subclass to consume.

template<typename input_type, typename output_type>
class obasiciofilteriterbase {

	//! This class
	typedef obasiciofilteriterbase<input_type, output_type> iter_t;

protected:
	//! Input type, narrow or wide characters
	typedef input_type input_type_t;

	//! Output type, narrow or wide characters
	typedef output_type output_type_t;

public:
	//! \ref iofilteradapterObj "iofilteradapterObj" type
	typedef iofilteradapterObj<input_type, output_type> adapterobj_t;

private:
	//! IO filter instance
	ptr<adapterobj_t> buffer;

public:
	//! Constructor
	obasiciofilteriterbase(//! The IO filter to use
			       const ref<adapterobj_t> &bufferArg)

		: buffer(bufferArg)
	{
		buffer->reset();
	}

	//! Default constructor
	obasiciofilteriterbase() {}

	//! Destructor
	~obasiciofilteriterbase() noexcept {}

private:

	//! Convert the current contents of the buffer, write converted character sequence to the output iterator

	bool internal_flush(//! Buffer
			    const ref<adapterobj_t> &b)
	{
		b->filter();

		output_type_t *p=&b->outbuf[0];
		size_t n=b->next_out-p;

		b->reset();

		flush(p, n);

		return n > 0;
	}

	//! Subclass must consume the entirety of this sequence

	virtual void flush(output_type_t *out_seq, size_t cnt_seq)
=0;

public:
	//! Iterator operator

	void collect(input_type_t val)
	{
		// A nullptr exception here means flush() has already been
		// called in the iofilter.

		ref<adapterobj_t> b=buffer;

		while (b->avail_in >= b->inbuf.size())
			internal_flush(b);

		b->inbuf[buffer->avail_in++]=val;
	}

protected:
	//! Flush any buffered data.

	void flush()
	{
		// A nullptr exception here means flush() has already been
		// called in the iofilter.

		ref<adapterobj_t> b=buffer;

		buffer=ptr<adapterobj_t>();

		while (b->avail_in > 0)
			internal_flush(b);

		while (internal_flush(b))
			;
	}
};

//! Convert a \ref iofilteradapterObj "IO filters" to an output iterator

//! The first template parameter is an output iterator type. This template
//! defines an output iterator that collects the output sequence, uses some
//! \ref iofilteradapterObj "IO filter" to convert the output sequence, and
//! places the converted sequence into another output iterator.
//!
//! The first template parameter is the output iterator type where the
//! converted sequence gets placed into, and the second parameter to the
//! constructor is an instance of the output iterator.
//!
//! The second template parameter is the input type to the IO filter, the
//! third template parameter is the IO filter's output type, which gets
//! sent to the underlying output iterator. The first parameter to the
//! constructor is a reference to the appropriate IO filter adapter.
//!
//! flush() filters any buffered but unconverted values through the IO filter,
//! places the remaining, converted sequence into the output iterator that was
//! originally passed to the constructor, then returns the resulting value of
//! the original output iterator. It's expected that this output iterator would
//! no longer be usable, from that point on.


template<typename out_iter_type, typename input_type, typename output_type>
class obasiciofilteriter : public obasiciofilteriterbase<input_type,
							 output_type>,
			   public std::iterator<std::output_iterator_tag,
						input_type,
						void, void, void > {

	//! Base class
	typedef obasiciofilteriterbase<input_type, output_type> base_t;

	//! Underlying output iterator
	out_iter_type iter;

	//! This class
	typedef obasiciofilteriter<out_iter_type, input_type,
				   output_type> iter_t;

public:

	//! Constructor
	obasiciofilteriter(//! The IO filter to use
			   const ref<iofilteradapterObj<input_type, output_type>
			   > &bufferArg,

			   //! Output iterator that's written to
			   const out_iter_type &iterArg)

		: base_t(bufferArg), iter(iterArg)
	{
	}

	//! Default constructor
	obasiciofilteriter()
	{
	}

private:
	//! Flush output of the filter into the output iterator

	void flush(output_type *out_seq, size_t cnt_seq)
	{
		while (cnt_seq)
		{
			*iter++ = *out_seq++;
			--cnt_seq;
		}
	}

public:

	//! Flush buffered input, return the resulting output iterator
	out_iter_type flush()
	{
		base_t::flush();
		return iter;
	}

	//! Iterator operator
	void operator=(input_type value)
	{
		this->collect(value);
	}

	//! Iterator operator
	iter_t &operator++() noexcept
	{
		return *this;
	}

	//! Iterator operator
	iter_t &operator++(int) noexcept
	{
		return *this;
	}

	//! Iterator operator
	iter_t &operator*() noexcept
	{
		return *this;
	}
};

//! Convert a \ref iofilteradapterObj "IO filters" to an input iterator

//! The first template parameter is an input iterator type. This template
//! class reads from an input sequence defined by a beginning and an ending
//! iterator of the template input iterator type. This template class
//! also implements an input iterator itself, over a sequence defined by
//! processing the original input sequence with an
//! \ref iofilteradapterObj "IO filter".
//!
//! The second template parameter specifies the type of the original
//! input sequence, and the input type of the IO filter. The third template
//! parameter specifies the output value type of the IO filter, and the
//! type iterated by the input iterator defined by this template class.
//!
//! The constructor takes a reference to an appropriate IO filter adapter
//! object reference, a beginning iterator and an ending iterator of the
//! underlying sequence. This constructs the beginning iterator for the
//! converted input sequence. The defaults constructor results in an ending
//! iterator value.

template<typename in_iter, typename input_type, typename output_type>
class ibasiciofilteriter : public std::iterator<std::input_iterator_tag,
						output_type,
						void, void, void > {

	//! Current input iterator
	in_iter cur_iter;

	//! Ending input iterator
	in_iter end_iter;

	//! This class
	typedef ibasiciofilteriter<in_iter, input_type, output_type> iter_t;

protected:
	//! Input type, narrow or wide characters
	typedef input_type input_type_t;

	//! Output type, narrow or wide characters
	typedef output_type output_type_t;

public:
	//! \ref iofilteradapterObj "iofilteradapterObj" type
	typedef iofilteradapterObj<input_type, output_type> adapterobj_t;

private:
	//! IO filter instance
	ptr<adapterobj_t> buffer;

public:
	//! Constructor
	ibasiciofilteriter(//! The IO filter to use
			   const ref<adapterobj_t> &bufferArg,

			   //! Beginning iterator for the input sequence
			   const in_iter &beg_iterArg,

			   //! Ending iterator for the input sequence
			   const in_iter &end_iterArg)

		: cur_iter(beg_iterArg), end_iter(end_iterArg),
		  buffer(bufferArg)
	{
		internal_fill();
	}

	//! Default constructor, results in an ending iterator value
	ibasiciofilteriter() {}

	//! Destructor
	~ibasiciofilteriter() noexcept {}

private:

	//! Convert the next input sequence chunk.

	void internal_fill()
	{
		ptr<adapterobj_t> buffer_save=buffer;

		buffer=ptr<adapterobj_t>();

		// If an exception gets thrown, this becomes the ending
		// iterator, thusly.

		internal_fill(buffer_save);

		buffer=buffer_save;
	}

	//! Convert the next input sequence chunk.

	void internal_fill(ptr<adapterobj_t> &buffer)
	{
		while (!buffer.null())
		{
			buffer->reset();

			while (buffer->avail_in < buffer->inbuf.size()
			       && cur_iter != end_iter)
			{
				buffer->inbuf[buffer->avail_in++] = *cur_iter++;
			}

			const input_type *p=buffer->next_in;

			buffer->filter();

			if (buffer->outp != buffer->next_out)
				break;

			if (p == buffer->next_in && cur_iter == end_iter)
				buffer=ptr<adapterobj_t>();
		}
	}

	//! A helper object for the post-increment operator

	class placeholder {

	public:
		//! Value being saved

		output_type_t val;

		//! Constructor
		placeholder(output_type_t valArg) noexcept : val(valArg) {}

		//! Destructor
		~placeholder() noexcept {}

		//! The post-increment * operator returns the value.

		output_type_t operator*() const noexcept { return val; }
	};

public:
	//! Iterator operator
	bool operator==(const iter_t &o) const noexcept
	{
		return buffer.null() && o.buffer.null();
	}

	//! Iterator operator
	bool operator!=(const iter_t &o) const noexcept
	{
		return !operator==(o);
	}

	//! Iterator operator
	placeholder operator++(int)
	{
		placeholder h(operator*());

		operator++();

		return h;
	}

	//! Iterator operator
	iter_t &operator++()
	{
		if (!buffer.null() &&
		    ++buffer->outp == buffer->next_out)
			internal_fill();
		return *this;
	}

	//! Iterator operator
	output_type_t operator*()
	{
		return *buffer->outp;
	}
};

extern template class obasiciofilteriterbase<char, wchar_t>;
extern template class obasiciofilteriterbase<wchar_t, char>;
extern template class obasiciofilteriterbase<char, char>;

#if 0
{
#endif
}
#endif
