/*
** Copyright 2013-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_ftp_clientobj_H
#define x_ftp_clientobj_H

#include <x/fdfwd.H>
#include <x/fditer.H>
#include <x/fdbaseobj.H>
#include <x/obj.H>
#include <x/netaddrfwd.H>
#include <x/fdbasefwd.H>
#include <x/sockaddrfwd.H>
#include <x/fdtimeoutfwd.H>
#include <x/basicstreamobj.H>
#include <x/refiteratorfwd.H>
#include <x/mpobj.H>
#include <x/threads/timerfwd.H>
#include <x/threads/timertaskfwd.H>
#include <x/ymdhms.H>
#include <x/gnutls/sessionfwd.H>
#include <x/gnutls/credentials_certificatefwd.H>
#include <x/namespace.h>

#include <algorithm>
#include <iterator>

namespace LIBCXX_NAMESPACE::ftp {
#if 0
}
#endif

//! FTP client connection implementer

class clientObj : virtual public obj {

	//! Base class passed to the nlst implementation

	class string_callback_base {

	public:
		//! Constructor
		string_callback_base();

		//! Destructor
		~string_callback_base();

		//! Subclass writes the file to the output iterator
		virtual void operator()(const std::string &)=0;
	};

	//! Response collector

	//! response() archives each line of the received response here.

	class LIBCXX_INTERNAL response_collector;
	//! Helper class used by make_response_collector()

	template<typename lambda_type>
	class LIBCXX_INTERNAL response_collector_impl;

	//! Make a response collector from a lambda.
	template<typename lambda_type>
	static response_collector_impl<lambda_type>
	make_response_collector(lambda_type &&lambda) LIBCXX_INTERNAL;

	//! Implement a lambda for a response collector
public:

	struct impl_tls_extraObj;

	//! Additional TLS protection metadata

	//! \internal
	typedef ref<impl_tls_extraObj> impl_tls;

	//! Additional TLS protection metadata

	//! \internal
	typedef ptr<impl_tls_extraObj> impl_tlsptr;

	//! Current connection: socket addresses, default timeout handler

	//! \internal
	struct impl_common {

		//! Underlying socket
		fdbase socket;

		//! TLS protection, if enabled
		impl_tlsptr tls;

		//! Socket name
		sockaddr sockname;

		//! Peer name
		sockaddr peername;

		//! Default timeout handler
		fdtimeout timeout;

		//! Whether the connection is broken for good.
		bool broken=false;

		//! Constructor
		impl_common(const fdbase &connArg) LIBCXX_INTERNAL;

		//! Destructor
		~impl_common() LIBCXX_INTERNAL;
	};

	//! Current connection, default timeout handler, stream.

	struct impl : public impl_common {

		//! Current timeout, either or default one or user-supplied
		fdbaseptr current_timeout;

		//! Current stream for reading/writing to the socket connection
		iostream stream;

		//! Constructor
		impl(const fdbase &connArg) LIBCXX_INTERNAL;

		//! Constructor
		impl(const fdbase &connArg,
		     const fdtimeout &timeoutArg) LIBCXX_INTERNAL;

		//! Destructor
		~impl() LIBCXX_INTERNAL;

		//! Read a response

		//! Process one response from the server.

		void response(const response_collector &collector)
			LIBCXX_INTERNAL;

		//! Read a response

		//! Returns '1', '2', or '3', the first character in the
		//! response, or throw an exception, referencing the command
		//! that's passed as a parameter.

		char response(//! Response to this command
			      const std::string &command,

			      //! Complete response
			      const response_collector &resp) LIBCXX_INTERNAL;

		//! Read a response

		char response(//! Response to this command
			      const std::string &command) LIBCXX_INTERNAL;

		//! Call response() as long as its return value is '1'
		char ok_response(const std::string &command) LIBCXX_INTERNAL;

		//! Call response() as long as its return value is '1'
		char ok_response(const std::string &command,
				 const response_collector &collector)
			LIBCXX_INTERNAL;

		//! Send a command
		void command(const std::string &command)
			LIBCXX_INTERNAL;

		//! Send a command

		//! Does not escape characters in command

		void command_raw(const std::string &command)
			LIBCXX_INTERNAL;

		//! Set the default timeouts
		void set_default_timeouts();

		//! And cancel them, we're done.
		void cancel_default_timeouts();
	};

	//! The implementation object.

	class conn_infoObj : virtual public obj {

	public:

		//! Mutex-protected implemenation object

		mpobj<impl> instance;

		//! Constructor
		conn_infoObj(const fdbase &connArg) LIBCXX_INTERNAL;

		//! Constructor
		conn_infoObj(const fdbase &connArg,
			     const fdtimeout &timeoutArg) LIBCXX_INTERNAL;

		//! Destructor
		~conn_infoObj() LIBCXX_INTERNAL;
	};

	//! Current connection handler

	typedef ref<conn_infoObj> conn_info;

private:
	class LIBCXX_HIDDEN lock;
	class LIBCXX_HIDDEN lock_abor;

	//! Current connection.
	mutable conn_info conn;

	//! Timer thread that issues no-OPS
	timer noop_timer_thread;

	//! The noop timer task
	timertask noop_task;

	//! Mccguffin that cancels the timer thread task
	ref<obj> noop_task_mcguffin;

	//! Create the noop task

	static timertask create_noop_task(const conn_info &conn)
		LIBCXX_INTERNAL;

	//! Whether to use passive connections

	bool passive;

public:
	//! Constructor
	clientObj(const fdbase &connArg, bool passiveArg=true);

	//! Constructor

	//! Use a caller-supplied timeout, for the initial greeting.
	clientObj(const fdbase &connArg,
		  const fdtimeout &timeoutArg,
		  bool passiveArg=true);

	//! Constructor

	//! Use TLS for the connection, must link with -lcxxtls.

	clientObj(const fdbase &connArg,
		  const gnutls::credentials::certificate &credentialsArg,
		  const std::string &server_nameArg="",
		  bool passiveArg=true);

	//! Constructor

	//! Use a caller-supplied timeout, for the initial greeting.
	//! Use TLS for the connection, must link with -lcxxtls.
	clientObj(const fdbase &connArg,
		  const gnutls::credentials::certificate &credentialsArg,
		  const fdtimeout &timeoutArg,
		  const std::string &server_nameArg="",
		  bool passiveArg=true);

private:
	//! Issue AUTH TLS, called from the TLS constructors

	void auth_tls(const fdbase &connArg,
		      const gnutls::credentials::certificate &credentialsArg,
		      const std::string &server_nameArg) LIBCXX_INTERNAL;

	//! Issue AUTH TLS command, wait for response.

	void auth_tls(lock &) LIBCXX_PUBLIC;
public:

	//! Destructor
	~clientObj();

	//! Log in

	void login(const std::string &userid,
		   const std::string &password);

	//! Install a timeout handler

	void timeout(const fdbase &timeoutArg);

	//! Reset default timeout handler
	void timeout();

private:
	//! Reset the default timeout handler

	//! \internal
	void reset_default_timeout(lock &l) LIBCXX_INTERNAL;

	//! Timeout handler has been reset, rebuild the input/output stream

	//! \internal
	void current_timeout_updated(lock &l) LIBCXX_INTERNAL;

	//! Throw an \ref exception "exception".

	//! \internal
	static void throw_error_from_response_str(const std::string &command,
						  const std::string &response_str)
		LIBCXX_INTERNAL __attribute__((noreturn));

	//! Process initial greeting
	void greeting() LIBCXX_INTERNAL;

	//! Open a data connection

	fd data(lock_abor &, bool, const std::string &,
		const fdtimeoutconfig &) LIBCXX_INTERNAL;


	//! Create a data connection using PORT

	//! Returns a bound, listening socket.
	static fd port(lock &) LIBCXX_INTERNAL;

	//! Create a data connction using PASV

	//! Returns address to connect to.
	static sockaddr pasv(lock &) LIBCXX_INTERNAL;

	class LIBCXX_HIDDEN tls_socket;

	//! If this is a TLS connection, enable it.
	tls_socket enable_tls(lock &lock, fdbase &&new_connection,
			      const fdtimeoutconfig &config)
		const LIBCXX_INTERNAL;

	//! Issue a TYPE command, based on 'flag', then create a data connection
	tls_socket transfer(lock_abor &l, bool flag,
			    const fdtimeoutconfig &timeout,
			    const std::string &allo_cmd,
			    const std::string &cmd,
			    std::string &response_str) const LIBCXX_INTERNAL;

	//! Create a data connection for a transfer
	tls_socket transfer(lock_abor &l,
			    const fdtimeoutconfig &timeout,
			    const std::string &allo_cmd,
			    const std::string &cmd,
			    std::string &response_str) const LIBCXX_INTERNAL;
	//! Helper class for nlst

	template<typename output_iter_type>
	class string_callback_impl : public string_callback_base {
	public:
		//! Current output iterator

		output_iter_type iter;

		//! Constructor
		string_callback_impl(output_iter_type &&iterArg)
			: iter(std::move(iterArg)) {}

		//! Constructor
		string_callback_impl(const output_iter_type &iterArg)
			: iter(iterArg) {}

		//! Destructor
		~string_callback_impl() {}

		//! Write the file to the output iterator
		void operator()(const std::string &filename) override
		{
			*iter=filename;
			++iter;
		}
	};

	//! Check for unprintable characters
	static void check_unprintable(const std::string &argument,
				      const char *what) LIBCXX_INTERNAL;

	//! Execute the NLST or LIST command.
	void nlst_or_list_output(const char *,
				 string_callback_base &,
				 const std::string &) const;

	//! Execute the NLST or LIST command.
	void nlst_or_list_output(const char *,
				 string_callback_base &,
				 const std::string &,
				 const fdtimeoutconfig &) const;

	class LIBCXX_HIDDEN default_timeout_config;

public:

	//! Directory listing

	template<typename output_iter_type>
	output_iter_type nlst(//! Write files in the directory to this
			      //! output iterator over std::strings
			      output_iter_type &&iter,

			      //! Directory name
			      const std::string &directory="")
	{
		string_callback_impl<typename std::decay<output_iter_type>
				     ::type> impl(std::move(iter));

		nlst_or_list_output("NLST", impl, directory);
		return impl.iter;
	}

	//! Directory listing
	template<typename output_iter_type>
	output_iter_type nlst(//! Write files in the directory to this
			      //! output iterator over std::strings
			      output_iter_type &&iter,

			      //! Directory name
			      const std::string &directory,

			      //! Use this timeout configuration for the data transfer
			      const fdtimeoutconfig &config)
	{
		string_callback_impl<typename std::decay<output_iter_type>
				     ::type> impl(std::move(iter));

		nlst_or_list_output("NLST", impl, directory, config);
		return impl.iter;
	}

	//! Directory listing
	template<typename output_iter_type>
	output_iter_type nlst(//! Write files in the directory to this
			      //! output iterator over std::strings
			      output_iter_type &&iter,

			      //! Use this timeout configuration for the data transfer
			      const fdtimeoutconfig &config)
	{
		return nlst(std::move(iter), "", config);
	}

	//! Directory listing

	template<typename output_iter_type>
	output_iter_type list(//! Write files in the directory to this
			      //! output iterator over std::strings
			      output_iter_type &&iter,

			      //! Directory name
			      const std::string &directory="")
	{
		string_callback_impl<typename std::decay<output_iter_type>
				     ::type> impl(std::move(iter));

		nlst_or_list_output("LIST", impl, directory);
		return impl.iter;
	}

	//! Directory listing
	template<typename output_iter_type>
	output_iter_type list(//! Write files in the directory to this
			      //! output iterator over std::strings
			      output_iter_type &&iter,

			      //! Directory name
			      const std::string &directory,

			      //! Use this timeout configuration for the data transfer
			      const fdtimeoutconfig &config)
	{
		string_callback_impl<typename std::decay<output_iter_type>
				     ::type> impl(std::move(iter));

		nlst_or_list_output("LIST", impl, directory, config);
		return impl.iter;
	}

	//! Directory listing
	template<typename output_iter_type>
	output_iter_type list(//! Write files in the directory to this
			      //! output iterator over std::strings
			      output_iter_type &&iter,

			      //! Use this timeout configuration for the data transfer
			      const fdtimeoutconfig &config)
	{
		return list(std::move(iter), "", config);
	}

	//! Change working directory
	void chdir(const std::string &str);

	//! Change to parent directory
	void cdup();

	//! Structure mount, server-specific function
	void smnt(const std::string &str);

	//! QUIT
	void logout();

	//! REIN
	void reinit();

private:

	//! Callback used by retr_impl() to return the contents of the file.

	struct retr_callback_base {

		//! Constructor
		retr_callback_base();

		//! Destructor
		~retr_callback_base();

		//! Next chunk of data.
		virtual void chunk(const char *p, size_t n)=0;
	};

	//! Write RETRed file to an output iterator

	template<typename output_iter_type> struct retr_callback
		: public retr_callback_base {

	public:
		//! Output iterator
		output_iter_type iter;

		//! Constructor
		retr_callback(output_iter_type &&iterArg)
			: iter(std::move(iterArg))
		{
		}

		//! Constructor
		retr_callback(const output_iter_type &iterArg)
			: iter(iterArg)
		{
		}

		//! Destructor
		~retr_callback() {}

		//! Next chunk.
		void chunk(const char *p, size_t n) override
		{
			iter=std::copy(p, p+n, iter);
		}
	};

	//! RETR file to an output iterator

	void retr_impl(//! Callback for saving the contents
		       retr_callback_base &output,

		       //! File
		       const std::string &file,

		       //! Use this timeout configuration for the data transfer
		       const fdtimeoutconfig &config,

		       //! Transfer in binary mode
		       bool binary) const;

	//! RETR file to an output iterator

	void retr_impl(//! Callback for saving the contents
		       retr_callback_base &output,

		       //! File
		       const std::string &file,

		       //! Transfer in binary mode
		       bool binary) const;

public:

	//! RETR a file, write its contents to an output iterator

	//! Returns final output iterator value.

	template<typename output_iter_type>
	output_iter_type get(//! Output iterator
			     const output_iter_type &iter,

			     //! File
			     const std::string &file,

			     //! Use this timeout configuration for the data transfer
			     const fdtimeoutconfig &config,

			     //! Transfer in binary mode
			     bool binary=true) const
	{
		retr_callback<typename std::decay<output_iter_type>
			      ::type> callback(iter);
		retr_impl(callback, file, config, binary);

		return callback.iter;
	}

	//! RETR a file, write its contents to an output iterator

	//! Returns final output iterator value.

	template<typename output_iter_type>
	output_iter_type get(//! Output iterator
			     const output_iter_type &iter,

			     //! File
			     const std::string &file,

			     //! Transfer in binary mode
			     bool binary=true) const
	{
		retr_callback<typename std::decay<output_iter_type>
			      ::type> callback(iter);
		retr_impl(callback, file, binary);

		return callback.iter;
	}

	//! Call get(), saving the results in a file

	template<typename ...Args>
	void get_file(const std::string &filename,
		      const std::string &file,
		      Args && ...args);

	//! Call get(), saving the results in a file

	template<typename ...Args>
	void get_file(const std::string &filename,
		      mode_t mode,
		      const std::string &file,
		      Args && ...args);
private:

	//! Callback used by do_stor()

	struct stor_callback_base {

		//! Constructor
		stor_callback_base();

		//! Destructor
		~stor_callback_base();

		//! Write new file to this output stream
		virtual void do_stor(fdbaseObj &o)=0;

		//! Whether this file's size is known
		virtual bool can_allo()=0;

		//! File's size
		virtual off64_t allo_size()=0;

		//! The 100 line from the server
		std::string init_response;
	};

	struct LIBCXX_HIDDEN put_fd_callback;

	template<typename iterator_category, typename iterator_type>
	struct stor_callback_impl_allo;

	template<typename iterator_type>
	struct stor_callback_impl_stor;

	//! Return "STOR " + file
	std::string stor_command(const std::string &filename);

	//! Return "APPE " + file
	std::string appe_command(const std::string &filename);

	//! Execute a STOR command

	void do_stor(//! Filename to STOR
		     const std::string &file,

		     //! Use binary transfer
		     bool binary,

		     //! Callback, for file content, and metrics
		     stor_callback_base &callback);

	//! Execute a STOR command
	void do_stor(//! Filename to STOR
		     const std::string &file,

		     //! Timeout configuration for the data connection
		     const fdtimeoutconfig &config,

		     //! Use binary transfer
		     bool binary,

		     //! Callback, for file content, and metrics
		     stor_callback_base &callback);

	//! Parse 257 reply from the server, extract the filename portion.

	static std::string parse_257(const std::string &reply)
		LIBCXX_INTERNAL;

public:
	//! STOR a file

	template<typename input_iter_type>
	void put(const std::string &file,  //!< Filename

		 //! Beginning input iterator for the file's contents
		 input_iter_type beg_iter,

		 //! Ending input iterator for the file's contents
		 input_iter_type end_iter,

		 //! Binary transfer
		 bool binary=true)
	{
		stor_callback_impl_stor<input_iter_type>
			callback(beg_iter, end_iter);
		do_stor(stor_command(file), binary, callback);
	}

	//! STOR a file

	template<typename input_iter_type>
	void put(const std::string &file,  //!< Filename

		 //! Beginning input iterator for the file's contents
		 input_iter_type beg_iter,

		 //! Ending input iterator for the file's contents
		 input_iter_type end_iter,

		 //! Timeout configuration for the transfer
		 const fdtimeoutconfig &config,

		 //! Binary transfer
		 bool binary=true)
	{
		stor_callback_impl_stor<input_iter_type>
			callback(beg_iter, end_iter);
		do_stor(stor_command(file), config, binary, callback);
	}

	//! Put a FILE

	void put_file(const std::string &localfile, //!< Local file
		      const std::string &remotefile, //!< Remote file

		      //! Binary transfer flag
		      bool binary=true);

	//! Put a FILE

	void put_file(const std::string &localfile, //!< Local file
		      const std::string &remotefile, //!< Remote file

		      //! Timeout configuration for the transfer
		      const fdtimeoutconfig &config,

		      //! Binary transfer flag
		      bool binary=true);

	//! STOU

	template<typename input_iter_type>
	std::string put_unique(//! Beginning input iterator for the file's contents
			       input_iter_type beg_iter,

			       //! Ending input iterator for the file's contents
			       input_iter_type end_iter,

			       //! Binary transfer
			       bool binary=true)
	{
		stor_callback_impl_stor<input_iter_type>
			callback(beg_iter, end_iter);
		do_stor("STOU", binary, callback);
		return callback.init_response;
	}

	//! STOU

	template<typename input_iter_type>
	std::string put_unique(//! Beginning input iterator for the file's contents
			       input_iter_type beg_iter,

			       //! Ending input iterator for the file's contents
			       input_iter_type end_iter,

			       //! Timeout configuration for the transfer
			       const fdtimeoutconfig &config,

			       //! Binary transfer
			       bool binary=true)
	{
		stor_callback_impl_stor<input_iter_type>
			callback(beg_iter, end_iter);
		do_stor("STOU", config, binary, callback);
		return callback.init_response;
	}

	//! STOU

	std::string put_unique_file(//! Local file
				    const std::string &localfile,

				    //! Binary transfer
				    bool binary=true);
	//! STOU

	std::string put_unique_file(//! Local file
				    const std::string &localfile,

				    //! Binary transfer
				    const fdtimeoutconfig &config,

				    //! Binary transfer
				    bool binary=true);

	//! APPE a file

	template<typename input_iter_type>
	void append(const std::string &file,  //!< Filename

		    //! Beginning input iterator for the file's contents
		    input_iter_type beg_iter,

		    //! Ending input iterator for the file's contents
		    input_iter_type end_iter,

		    //! Binary transfer
		    bool binary=true)
	{
		stor_callback_impl_stor<input_iter_type>
			callback(beg_iter, end_iter);
		do_stor(appe_command(file), binary, callback);
	}

	//! APPE a file

	template<typename input_iter_type>
	void append(const std::string &file,  //!< Filename

		    //! Beginning input iterator for the file's contents
		    input_iter_type beg_iter,

		    //! Ending input iterator for the file's contents
		    input_iter_type end_iter,

		    //! Timeout configuration for the transfer
		    const fdtimeoutconfig &config,

		    //! Binary transfer
		    bool binary=true)
	{
		stor_callback_impl_stor<input_iter_type>
			callback(beg_iter, end_iter);
		do_stor(appe_command(file), config, binary, callback);
	}

	//! APPE a FILE

	void append_file(const std::string &localfile, //!< Local file
			 const std::string &remotefile, //!< Remote file

			 //! Binary transfer flag
			 bool binary=true);

	//! APP a FILE

	void append_file(const std::string &localfile, //!< Local file
			 const std::string &remotefile, //!< Remote file

			 //! Timeout configuration for the transfer
			 const fdtimeoutconfig &config,

			 //! Binary transfer flag
			 bool binary=true);

	//! Delete a file
	void unlink(const std::string &file);

	//! Rename a file

	void rename(const std::string &from_file,
		    const std::string &to_file);

	//! Make directory.

	//! Returns the 257 response from the server, which should be a full
	//! pathname.

	std::string mkdir(const std::string &dir);

	//! Remove directory
	void rmdir(const std::string &dir);

	//! Current directory
	std::string pwd() const;

	//! SITE command
	std::string site(const std::string &cmd);

	class stat_callback;

	template<typename lambda_type> class stat_callback_impl;

	//! Implement filestat().

	void do_filestat(stat_callback &callback,
			 const std::string &dirname) const;

	//! Implement dirstat().
	void do_dirstat(stat_callback &callback,
			const std::string &dirname,
			const fdtimeoutconfig &config) const;

	//! Implement dirstat().

	void do_dirstat(stat_callback &callback,
			const std::string &dirname) const;
public:
	//! Parse time-val

	//! \internal
	static ymdhms parse_timeval(const std::string &s) LIBCXX_INTERNAL;

	//! MDTM command
	ymdhms timestamp(const std::string &filename);

	//! SIZE command
	off64_t size(const std::string &filename);

	template<typename lambda_type>
	void filestat(lambda_type &&lambda,
		      const std::string &filename="") const;

	template<typename lambda_type>
	void dirstat(lambda_type &&lambda,
		      const std::string &dirname="") const;

	template<typename lambda_type>
	void dirstat(lambda_type &&lambda,
		     const fdtimeoutconfig &config) const;

	template<typename lambda_type>
	void dirstat(lambda_type &&lambda,
		     const std::string &dirname,
		     const fdtimeoutconfig &config) const;
};

//! Implement can_allo() and allo_size() callback from do_stor()

//! Generic implementation for forward, bidirectional, and random access
//! iterators.

template<typename iterator_category, typename iterator_type>
struct clientObj::stor_callback_impl_allo : public stor_callback_base {

public:

	iterator_type b, //!< Contents' beginning iterator
		e;	//!< Contents' ending iterator

	//! Constructor
	stor_callback_impl_allo(iterator_type bArg, iterator_type eArg)
		: b(bArg), e(eArg)
	{
	}

	//! Destructor
	~stor_callback_impl_allo() {}

	//! Yes, ALLO can be done
	bool can_allo() override
	{
		return true;
	}

	//! Size of the file to be STORed
	off64_t allo_size() override
	{
		return std::distance(b, e);
	}
};

//! STORing an input sequence.

//! can_allo() and allo_size() callbacks from do_stor() cannot be implemented.

template<typename iterator_type>
struct clientObj::stor_callback_impl_allo<std::input_iterator_tag, iterator_type>
	: public stor_callback_base {

public:

	iterator_type b, //!< Contents' beginning iterator
		e;	//!< Contents' ending iterator

	//! Constructor
	stor_callback_impl_allo(iterator_type bArg, iterator_type eArg)
		: b(bArg), e(eArg)
	{
	}

	//! Destructor
	~stor_callback_impl_allo() {}

	//! We can't
	bool can_allo() override
	{
		return false;
	}

	//! Zero, unknown
	off64_t allo_size() override
	{
		return 0;
	}
};

//! Implement do_stor() callback from do_stor(), naturally.

//! Subclass the appropriate stor_callback_impl_allo version, based on the
//! input iterator type.

template<typename iterator_type>
struct clientObj::stor_callback_impl_stor
	: public stor_callback_impl_allo<typename std::iterator_traits<
						 iterator_type>
					 ::iterator_category,
					 iterator_type> {

public:
	//! Constructor
	stor_callback_impl_stor(iterator_type bArg, iterator_type eArg)
		: stor_callback_impl_allo<typename std::iterator_traits<
						  iterator_type>
					  ::iterator_category,
					  iterator_type>(bArg, eArg)
	{
	}

	//! Destructor
	~stor_callback_impl_stor()
	{
	}

	//! Write new contents to the output iterator
	void do_stor(fdbaseObj &o) override
	{
		o.write_full(this->b, this->e);
	}
};


#if 0
{
#endif
}
#endif
