/*
** Copyright 2012-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/

/*! \file

This file defines a flexible logging mechanism

*/

#ifndef x_logger_H
#define x_logger_H

#include <x/namespace.h>
#include <x/obj.H>
#include <x/ptr.H>
#include <x/property_properties.H>
#include <x/property_value.H>
#include <x/singletonfwd.H>
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <list>
#include <locale>

namespace LIBCXX_NAMESPACE {
#if 0
};
#endif

#include <x/sys/tidtsys.h>

//! Retrieve the executing thread id.

//! Not in glibc, but it should be.
//!
tid_t gettid() noexcept;

//! The default \c %TRACE logging level

#define LOGLEVEL_TRACE		10

//! The default \c %DEBUG logging level

#define LOGLEVEL_DEBUG		20

//! The default \c %INFO logging level

#define LOGLEVEL_INFO		30

//! The default \c %WARNING logging level

#define LOGLEVEL_WARNING	40

//! The default \c %ERROR logging level

#define LOGLEVEL_ERROR		50

//! The default \c %FATAL logging level

#define LOGLEVEL_FATAL		100

//! Application logging

//! Instances of this %logger class are not generally used directly, rather
//! the #LOG_FUNC_SCOPE_DECL, #LOG_FUNC_SCOPE,
//! #LOG_CLASS_SCOPE, and #LOG_CLASS_INIT macros declare
//! an object called &quot;<tt>%logger</tt>&quot;
//! at a class or a function level,
//! then, one of the \c LOG_* macros generates a logging message.
//!
//! Once a logging scope is set up, the following macros report a log event:
//!
//! #LOG_TRACE(msg), #LOG_DEBUG(msg), #LOG_INFO(msg), #LOG_WARNING(msg),
//! #LOG_ERROR(msg), #LOG_FATAL(msg)
//!
//! These logging levels are listed in order from most verbose to least verbose.
//! When the current logging level is set to \c ERROR, log events from
//! #LOG_ERROR and #LOG_FATAL macros are reported. When the current logging
//! level is set to \c DEBUG, all macros, except for #LOG_TRACE report
//! logging events.
//!
//! \c msg may be any arbitrary expression that occurs to the right of a
//! \c std::ostream object:
//!
//! \code
//!     LOG_INFO("Opening " << filename);
//! \endcode
//!
//! The log message does not get formatted unless the corresponding log level
//! is enabled. This logging implementation is fairly lightweight.
//!
//! If the log message contains newlines, each individual line gets logged
//! as a separate message (and empty log lines are removed).
//!
//! The #LOG_FUNC_SCOPE, #LOG_FUNC_SCOPE_DECL,
//! #LOG_CLASS_SCOPE, and #LOG_CLASS_INIT macros declare
//! an object "logger", in the current scope. Use these macros to define a
//! hierarchy of logging scopes.
//!
//! \note
//! Log messages are formatted in the application global default locale,
//! modifier with the \c std::locale::numeric category from the "C" locale.
//! Formatting various numbers, in log messages, with thousands' separators
//! looks out of place.
//!
//! \par Class level
//!
//! \code
//!
//! // Header file:
//!
//! class DialogWindow {
//!
//!  LOG_CLASS_SCOPE;
//!
//! // ...
//!
//!   class Widget {
//!
//!     LOG_CLASS_SCOPE;
//! // ...
//! };
//!
//! // In one of the translation units (C++ source files) that implement
//! // these classes, outside of any function:
//!
//! LOG_CLASS_INIT(DialogWindow);
//! LOG_CLASS_INIT(DialogWindow::Widget);
//!
//! \endcode
//!
//! Each occurence of the #LOG_CLASS_SCOPE macro declares a static class member
//! called \c %logger, which get subsequently referenced by any
//! #LOG_TRACE, #LOG_DEBUG, #LOG_INFO, #LOG_WARNING, #LOG_ERROR, and #LOG_FATAL
//! macros invoked in any class method.
//!
//! \note
//! If inline functions are defined that use the \c LOG macros, the
//! \c LIBCXX_PUBLIC macro should be used like this:
//!
//! \code
//! class DialogWindow {
//!
//!     LOG_CLASS_SCOPE LIBCXX_PUBLIC;
//!
//! // ...
//!
//!     void draw()
//!     {
//!         LOG_DEBUG("Draw called");
//! // ...
//! \endcode
//!
//! This might be necessary when building a shared library. It is not required
//! if the \c LOG macros referencing this scope get used only in the same
//! translation unit that also declares LOG_CLASS_INIT.
//!
//! \par Function level
//!
//! \code
//! LOG_FUNC_SCOPE_DECL(DialogWindow::Widget::onclicked, dialog_widget_onclicked);
//!
//! void DialogWindow::Widget::onclicked()
//! {
//!     LOG_FUNC_SCOPE(DialogWindow_widget_onclicked);
//!
//! // ...
//!
//!     LOG_INFO("Click processing completed");
//! }
//! \endcode
//!
//! #LOG_FUNC_SCOPE defines a \c %logger object reference at the function level.
//! The reference points to an object previously declared outside the function,
//! at the namespace scope.
//! Occurences of
//! #LOG_TRACE, #LOG_DEBUG, #LOG_INFO, #LOG_WARNING, #LOG_ERROR, and #LOG_FATAL
//! macros that appear within this function will subsequently refer to this
//! static logger object.
//!
//! This mechanism implements a flexible mechanism to define
//! logging scopes. A \c %logger object defined at the class level becomes
//! the default \c %logger object referenced by all %LOG_ macros in any
//! class method, unless a function-level \c %logger object defines a more
//! granular logging scope.
//!
//! It is also possible to declare additional scopes within a single
//! statement block, thus defining an even more granular logging scope.
//!
//! Since these macros define static \c %logger objects, all of these objects
//! get instantiated at once, when the application starts up, and they incur
//! no additional overhead when creating a class instance, or entering
//! a function.
//!
//! \par Logging configuration
//!
//! The actual logging is controlled by setting application properties.
//! See \ref property "property" for more information.
//!
//! The following logging configuration gets loaded automatically.
//! Additional %property settings have the effect of modifying this
//! configuration:
//!
//! - Define the \c TRACE, \c DEBUG, \c INFO, \c WARNING, \c ERROR, and
//! \c FATAL log levels. It is possible to define additional logging levels,
//! this is currently undocumented, and there's little need for it. The benefit
//! from setting up additional configuration level seems to be dubious.
//!
//! - Define the \em interactive, \em brief, \em full, and \em syslog log message formats.
//!
//! - Set the root log level to \b error.
//!
//! - Set up a log handler called "default", that reports log %messages to
//! standard error, and the log format to either \b interactive or \b brief.
//! A standard error going to an interactive terminal sets the log format is
//! \b interactive, and standard error going to a non-interactive terminal
//! results in a \b brief default log format. This has the effect of
//! automatically prepending the current time when the standard error gets
//! captured to the log file.
//!
//! The following %property configuration settings take effect only when
//! specified in the application's property file (the filename set in the
//! \c PROPERTIES environment variable, or from
//! <span style="font-style: italic">filename</span><span style="font-family: monospace">.properties</span>, where
//! <i>filename</i> is the application's executable filename).
//! They cannot be specified by the \c --set-property command line option,
//! or loaded by the application manually.
//! For speed, the logging configuration gets
//! parsed once, when the application initializes. The %property settings
//! that define logging configuration are not consulted for every log message.
//! However, some %property settings do take effect at runtime. They are
//! described later.
//!
//! \par Log configuration
//!
//! Application code:
//!
//! \code
//! class DialogWindow {
//!
//!  LOG_CLASS_SCOPE;
//!
//! // ...
//!
//!   class Widget {
//!
//!     LOG_CLASS_SCOPE;
//! // ...
//! };
//!
//! // ...
//!
//! LOG_CLASS_INIT(DialogWindow);
//! LOG_CLASS_INIT(DialogWindow::Widget);
//! \endcode
//!
//! In the application properties file:
//!
//! \code
//! DialogWindow::@log::level=warning
//!
//! DialogWindow::Widget::@log::level=trace
//! DialogWindow::Widget::@log::handler::default=full
//! \endcode
//!
//! The effect of the above settings are:
//!
//! - Log %messages at #LOG_TRACE, and above, at the \c DialogWindow::widget
//! scope are reported. The \c default log handler's format is set to
//! \b full.
//!
//! - Log %messages at #LOG_WARNING, and above, at the \c DialogWindow
//! scope are reported.
//!
//! Settings for each logging scope take their default values from their
//! parent scope, so \c DialogWindow's log format defaults to the root
//! log format's.
//!
//! Recall that the %property configuration file may use
//! either semicolons or periods, as hierarchy delimiters. The above example
//! is the same as:
//!
//! \code
//! DialogWindow.@log.level=warning
//!
//! DialogWindow.Widget.@log.level=trace
//! DialogWindow.Widget.@log.handler.default=full
//! \endcode
//!
//! Use the following setting to change the root scope's, which gets
//! inherited by all inferior scopes, unless overriden:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::log::level=trace
//! INSERT_LIBX_NAMESPACE::logger::log::handler::default::format=full
//! \endcode
//!
//! \par Aliases
//!
//! Aliases for the default log levels and formats provide
//! a better way to set up flexible logging:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::level::mydebug=debug
//! INSERT_LIBX_NAMESPACE::logger::format::myfull=$full
//!
//! # ...
//!
//! DialogWindow::Widget::@log::level=mydebug
//! DialogWindow::Widget::@log::handler::default::format=myfull
//!
//! \endcode
//!
//! \c INSERT_LIBX_NAMESPACE::logger::level::ALTLEVEL=LEVEL defines an alias \c ALTLEVEL that's
//! equivalent to \c LEVEL. Changing this alias affects all other logging scopes
//! that set their log level to \c ALTLEVEL.
//!
//! Similarly \c INSERT_LIBX_NAMESPACE::logger::format::ALTFORMAT=$FORMAT defines an alias for
//! a log message format. Redefining the alias affects all logging scopes
//! that use the alias.
//!
//! \par A custom log message format
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::format::custom=%Y-%m-%d %H:%M:%S @{class}: @{msg}
//!
//! # ...
//!
//! MessageHandler::QuitMessage::@log::handler::default::format=custom
//! \endcode
//!
//! Setting \c INSERT_LIBX_NAMESPACE::logger::format::NAME=string defines a new log message format,
//! \c NAME. \em string gives the actual format.
//!
//! After defining the custom log message format, it must be assigned to a
//! given logging scope. This is done by setting
//! \c SCOPE::\@log::handler::HANDLER::format=FORMAT, where "SCOPE" is the
//! logging
//! scope ("MessageHandler::QuitMessage" in the above example), HANDLER is
//! the logging handler's name (the default logging configuration sets up
//! a long handler named "default" at the root level, which gets inherited
//! by all inferior logging scopes), and "FORMAT" is the custom format's
//! name.
//!
//! The contents of the format string contain the following placeholders:
//!
//! - \c \%\% - a literal \% character.
//!
//! - \c \@\@ - a literal \@ character.
//!
//! - \c \%c - current time. "c" is a single character -
//! see %strftime(3) for a list of available time characters. Note -- optional
//! conversion flags and glibc extensions are not yet implemented.
//!
//! - \c \@{level} - the message's log level: "warn", "debug"...
//!
//! - \c \@{class} - the name of logging scope, followed by a literal colon
//! and a space character.
//!
//! - \c \@{thread} - the thread's configured name.
//!
//! - \c \@{msg} - the formatted log message.
//!
//! - \c \@{pid} - the process or the thread ID, or a thread context.
//!
//! By default, \@{pid} produces a meaningless, numerical, thread ID.
//! Instantiating a logger::context object provides an additional,
//! informative, label:
//!
//! \code
//! logger::context log_context("listenerThread");
//! \endcode
//!
//! A logger::context object must be instantiated on the stack, not the heap.
//! This example appends ":listenerThread" to \@{pid}, until \c log_context
//! goes out of scope.
//!
//! Multiple \c logger::context objects can be instantiated on the stack.
//! \@{pid} gets augmented by the concatenation of all \c logger::context
//! objects on the stack, with "/" separating their concatenated labels.
//!
//! \par The default log formats
//!
//! Here are the default logging formats:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::format::interactive=@{class}: @{msg}
//! INSERT_LIBX_NAMESPACE::logger::format::brief=%a %H:%M:%S @{class}: @{msg}
//! INSERT_LIBX_NAMESPACE::logger::format::full=%a %H:%M:%S @@@{pid} @{thread} @{class}: @{msg}
//! INSERT_LIBX_NAMESPACE::logger::format::syslog=@{class}: @{msg}
//! \endcode
//!
//! Although the configuration file can redefine them, using aliases is better.
//!
//! \par Log handlers
//!
//! The "default" log handler reports the log %messages to standard error.
//! The predefined root scope's settings are as follows:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::log::handler::default=stderr
//! INSERT_LIBX_NAMESPACE::logger::log::handler::default::format=brief
//! \endcode
//!
//! The "default" part is just an arbitrary label that identifies a destination
//! for logging %messages. Two attributes must be defined for a logging
//! message destination:
//!
//! * %INSERT_LIBX_NAMESPACE::%logger::%log::%handler::<em>label</em> - specifies the log handler.
//!
//! * %INSERT_LIBX_NAMESPACE::%logger::%log::%handler::<em>label</em>=<tt>format</tt> - specifies the log message format.
//!
//! The "default" label is a predefined logging destination that's defined at
//! the root scope, and gets inherited by all children logging scope.
//! Each individual logging scope may redefine the "default" handler or the
//! format. The redefined setting takes effect for this logging scope, and all
//! children scopes.
//!
//! \c stderr is a predefined log handler that writes the log %messages to
//! standard error. Here's how to define a handler that writes the log %messages
//! to a file:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::handler::logfile=logdir/%Y/%m/%d/applog.%H.txt
//!
//! INSERT_LIBX_NAMESPACE::logger::log::handler::default=logfile
//! \endcode
//!
//! <tt>INSERT_LIBX_NAMESPACE::%logger::%handler::</tt><em>label</em> defines a new log handler,
//! called &quot;<em>label</em>&quot;.
//! The above example defines a log handler called "logfile" which writes
//! log %messages to the given file. Then,
//! the default handler at the root scope is replaced with "logfile".
//!
//! <tt>%%x</tt> placeholders in the filename are replaced with the current
//! time. See the %strftime(3) man page for a list of available placeholders.
//! A filename containing <tt>%%x</tt> placeholders automatically rotates the
//! log file. The above example creates a new log file once per hour, since
//! the log filename contains the current hour.
//!
//! \note
//! The log files get created at most once a minute. A log filename containing
//! <tt>%%s</tt> still creates one log file per minute.
//!
//! \note
//! Any intermediate directories get automatically created. Existing files
//! get appended to.
//!
//! \par Automatic log rotation
//!
//! Older log files can be removed automatically by adding two more settings
//! to the log file handler configuration:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::handler::logfile=logdir/%Y/%m/%d/applog.%H.txt
//! INSERT_LIBX_NAMESPACE::logger::handler::logfile::rotate=logdir/*/*/*/*.log
//! INSERT_LIBX_NAMESPACE::logger::handler::logfile::keep=4
//! \endcode
//!
//! Set "rotate" to a shell wildcard pattern that matches the filenames that get
//! created by the log file handler. The "keep" setting specifies how many of
//! of the matched files should be preserved, the oldest ones will be removed
//! (based on the log file timestamps). In this example, the four most recent
//! log files get preserved, and the older log files get removed.
//!
//! \par External log rotation
//!
//! Log files may also be rotated by an external process. The log filename
//! should not contain any <tt>%%x</tt> time-related placeholders, and
//! arrangements should be made to have an external process rename the log
//! file. Once a minute, the %logger class checks if the log file was renamed,
//! if so the current log file descriptor is closed, and reopened.
//!
//! \note
//! The log file should not be compressed immediately after it's renamed.
//! The %logger class may continue to append additional log entries to the
//! existing file for up to a minute, before the log file gets closed and
//! reopened. Do not compress the renamed log file, until a new log file gets
//! recreated.
//!
//! \par Multiple logs, and inheritance
//!
//! The configuration file may specify multiple log handlers for a given scope.
//! If so, each log message gets logged by each handler:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::handler::logfile=logdir/%Y/%m/%d/applog.%H%M%S.txt
//!
//! MainDialog::ConfirmWidget::@log::handler::extralog=logfile
//! MainDialog::ConfirmWidget::@log::handler::extralog::format=full
//! \endcode
//!
//! The "default" log handler, that writes %messages to standard error, remains
//! in scope globally. The "MainDialog::ConfirmWidget" scope defines an
//! additional log handler, with the label "extralog" that logs to the given
//! file using the \b full log format.
//!
//! All inferior scopes of \c MainDialog::ConfirmWidget inherit the parent's
//! settings, and send log %messages
//! to both the \c default and the \c extralog handlers.
//! &quot;<tt>inherit=false</tt>&quot; overrides the automatic inheritance:
//!
//! \code
//! MainDialog::ConfirmWidget::reset::@log::inherit=false
//! MainDialog::ConfirmWidget::reset::@log::handler::default=stderr
//! MainDialog::ConfirmWidget::reset::@log::handler::default::format=brief
//! \endcode
//!
//! Nothing gets inherited because of &quot;<tt>inherit=false</tt>&quot;,
//! including the \c default log handler, so it must be redefined, otherwise
//! all logging %messages from this scope, and below, will vanish because they
//! have no defined log destination.
//!
//! \par Logging to syslog
//!
//! Logging to the syslog facility is implemented by the correspondingly-named
//! \c syslog handler. It's just another handler:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::log::handler::default=syslog
//! INSERT_LIBX_NAMESPACE::logger::log::handler::default::format=syslog
//! \endcode
//!
//! There's a corresponding \b syslog message format that contains only
//! &quot;<tt>\@{class}: \@{msg}</tt>&quot;. \c syslog adds its own timestamps,
//! so the additional timestamps added by \b full or \b brief message formats
//! will be redundant.
//!
//! Sending log %messages to both standard error and syslog involves defining
//! an additional handler, instead of replacing the \c default one:
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::log::handler::default2=syslog
//! INSERT_LIBX_NAMESPACE::logger::log::handler::default2::format=syslog
//! \endcode
//!
//! Log %messages get sent to the default handler, using the default format,
//! and to the \c syslog handler, in the \c syslog format.
//!
//! The default configuration uses \c LOG_USER syslog facility.
//!
//! \code
//! INSERT_LIBX_NAMESPACE::logger::syslog::facility=MAIL
//! \endcode
//!
//! This setting changes the \c syslog facility to \c LOG_MAIL.
//!
//! \par Runtime reconfiguration
//!
//! A limited subset of logging %properties may be set at runtime. Not all
//! %property settings take effect. For speed, %properties that define the
//! logging configuration are loaded and parsed once, when the application
//! starts, and setting additional logging %properties generally has no
//! effect, except as described here.
//!
//! The following logging %properties may be set at runtime:
//!
//! \code
//! property::update_properties("myclass::@log::level=error\n");
//! \endcode
//!
//! Setting the <tt>\@log::level</tt> %property, for a given class, immediately
//! changes the logging level for that scope.
//!
//! \code
//! property::update_properties("myclass::@log::handler::default::format=brief\n");
//! \endcode
//!
//! Setting <tt>\@log::handler::</tt><i>name</i><tt>:</tt><tt>:format</tt> %property changes the
//! format of the log message, within the given scope.
//!
//! Additionally,
//! setting <tt>\@log::handler::<i>name</i></tt> %property replaces an existing
//! %log handler within the given scope.
//! This cannot be used to define a new handler in a logging scope, only to
//! redefine an existing one.
//!
//! \par Logging in template classes
//!
//! Here's an example of enabling logging in template classes:
//!
//! \code
//! template<typename int_type, typename char_type> class template_class {
//!
//!   LOG_CLASS_SCOPE;
//!
//! public:
//!   static void logsomething()
//!   {
//!      LOG_ERROR("Something");
//!   }
//!
//!   class iterator {
//!
//!     LOG_CLASS_SCOPE;
//!
//!   public:
//!     static void logsomething()
//!     {
//!        LOG_ERROR("Something");
//!     }
//!   };
//! };
//!
//! template<typename int_type, typename char_type>
//! LOG_CLASS_INIT(template_class<int_type, char_type>);
//!
//! template<typename int_type, typename char_type>
//! LOG_CLASS_INIT(template_class<int_type, char_type>::iterator);
//! \endcode
//!
//! Note the explicit instantiation of the \c LOG_CLASS_INIT macro.
//!
//! Given the above definition, and the following example:
//!
//! \code
//!   template_class<int,char>::logsomething();
//!   template_class<int,int>::iterator::logsomething();
//! \endcode
//!
//! This results in the following messages getting logged:
//!
//! \code
//! template_class<int_type, char_type>: Something
//! template_class<int_type, char_type>::iterator: Something
//! \endcode
//!
//! Note that the class name in both examples is the same, and references the
//! template parameter names, rather than the actual names. This is also
//! reflected in the resulting property hierarchies
//! that control the logging configuration. &ldquo;<code>--properties=all</code>&rdquo;
//! will report that the property hierarchies are
//! &ldquo;<code>template_class::int_type::char_type::\@log</code>&rdquo;
//! and
//! &ldquo;<code>template_class::int_type::char_type::iterator::\@log</code>&rdquo;.

class LIBCXX_HIDDEN logger {

	//! Helper function for setting log levels by name.

	class debuglevelpropstr {

	public:

		//! Return the log level setting
		static std::string tostr(short n, const const_locale &l)
			LIBCXX_INTERNAL;

		//! Set the log level by name
		static short fromstr(const std::string &str,
			      const const_locale &l)
			LIBCXX_INTERNAL;
	};

public:

	//! Thread-specific context name

	//! Instantiate these objects on the stack in order to replace
	//! the bland thread ID "\@{pid}" with a custom name.

	class LIBCXX_PUBLIC context {

		//! Name of the context
		std::string name;

		//! Next, stacked, context.
		context *next_context;

	public:
		//! Default constructor

		context(//! Current context name
			const std::string &nameArg) noexcept;

		//! Default destructor
		~context();

		//! Retrieve the context string

		//! Recursively walks through the context stack, and returns
		//! a slash-separated path.

		std::string getContext() noexcept;
	};

	friend class context;

	friend class debuglevelpropstr;
private:
	//! Current context stack

	static __thread context *context_stack LIBCXX_INTERNAL;

	class LIBCXX_INTERNAL scopedestObj;
	class LIBCXX_INTERNAL handlername;
	class LIBCXX_INTERNAL handlerfmt;

	class inheritObj;

	//! Base class for scope-related data

	class scopebase {

	public:
		//! Debug level for this logging scope.

		property::value<short, debuglevelpropstr> debuglevel;

		//! Container for log handlers
		typedef std::unordered_map<std::string,
					   ref<scopedestObj> > handlers_t;

		//! Log handlers
		handlers_t handlers;

		//! Internal constructor
		scopebase(inheritObj &inherit) LIBCXX_INTERNAL ;

		//! Internal destructor
		~scopebase() LIBCXX_INTERNAL ;

		//! Internal function

		static ptr<inheritObj> getscope(const std::string &name)
			LIBCXX_INTERNAL;

		//! Internal function
		static std::string
		get_debuglevel_propname(inheritObj &inherit)
			LIBCXX_INTERNAL;

		//! Internal function
		static short
		get_debuglevel_propvalue(inheritObj &inherit)
			LIBCXX_INTERNAL;

	};

	//! Assembled information about this logging scope

	scopebase scope;

	//! Name of this logging scope
	const char *name;

public:
	//! Construct a %logger object

	//! The constructor locates the logging configuration for the
	//! corresponding logging module.
	logger(//! Module name

	       //! The module name is generally a fully-qualified C++ class
	       //! name, such as "namesp::foo::bar". Either colons or periods
	       //! may be used as hierarchy delimiteres.
	       const char *moduleName) noexcept LIBCXX_PUBLIC;

	//! Destructor
	~logger() LIBCXX_PUBLIC;

	//! Retrieve the current log level

	//! \internal
	//! This is used by the LOG mactos.
	inline short getDebugLevel() const noexcept { return scope.debuglevel.get(); }

	//! Make a log entry

	//! \internal
	//! This is used by the LOG macros.
	void operator()(//! The log message
			const std::string &s,
			//! The log level
			short loglevel) const noexcept LIBCXX_PUBLIC;

	static bool logger_subsystem_initialized() noexcept; //!< \internal

private:
	//! Make a log entry

	//! \internal
	//!

	void domsg(std::unordered_map<std::string, std::string> &vars,
		   const std::time_put<char> &timecvt,
		   struct tm &tmbuf,
		   short loglevel) const noexcept LIBCXX_INTERNAL;
};

//! Declare a logger object that will be used in a functional scope.

//! Use this macro to declare a logger object within the scope of a function:
//!
//! \code
//! LOG_FUNC_SCOPE_DECL(DialogWindow::Widget::onclicked, dialog_widget_onclicked);
//!
//! void DialogWindow::Widget::onclicked()
//! {
//!     LOG_FUNC_SCOPE(DialogWindow_widget_onclicked);
//!
//! // ...
//!
//!     LOG_INFO("Click processing completed");
//! }
//! \endcode
//!
//! The LOG_FUNC_SCOPE_DECL macro declares a static logger object which will
//! be used inside a function. LOG_FUNC_SCOPE_DECL must be used outside of the
//! function (namespace scope). LOG_FUNC_SCOPE_DECL declares a logging scope
//! specified by its first parameter. The name of the logger object statically
//! declared at namespace scope is specified by the second parameter, and it
//! must be a valid, unique, identifier within its translation unit.

#define LOG_FUNC_SCOPE_DECL(name,decl)					\
	static const LIBCXX_NAMESPACE::logger decl(LOG_CLASS_INIT2(name))

//! Activate a logger object within a functional scope

//! Use this macro to activate an object previously declared by
//! LOG_FUNC_SCOPE_DECL within the current function:
//!
//! \code
//! LOG_FUNC_SCOPE_DECL(DialogWindow::Widget::onclicked, dialog_widget_onclicked);
//!
//! void DialogWindow::Widget::onclicked()
//! {
//!     LOG_FUNC_SCOPE(DialogWindow_widget_onclicked);
//!
//! // ...
//!
//!     LOG_INFO("Click processing completed");
//! }
//! \endcode
//!
//! The object specified by the parameter to this macro becomes the target
//! of log %messages by all LOG macros used within the scope

#define LOG_FUNC_SCOPE(name) \
	const LIBCXX_NAMESPACE::logger &logger=name

//! Declare a logger class member

//! Use this macro to declare a static class member called \c logger, that's
//! an instance of a logger object.
//!
//! \code
//! class DialogWindow {
//!
//!  LOG_CLASS_SCOPE;
//!
//! // ...
//!
//!   class Widget {
//!
//!     LOG_CLASS_SCOPE;
//! // ...
//! };
//! \endcode
//!
//! This merely declares the \c logger object, it still must be initialized
//! somewhere in one of the translation units (C++ source files), using
//! #LOG_CLASS_INIT.

#define LOG_CLASS_SCOPE \
	static const LIBCXX_NAMESPACE::logger logger

//! Initialize a logger class member

//! Use this macro in conjunction with LOG_CLASS_SCOPE to initialize
//! the \c logger class member:
//!
//! \code
//!
//! // In the header file:
//!
//! class DialogWindow {
//!
//!  LOG_CLASS_SCOPE(DialogWindow);
//!
//! // ...
//!
//! };
//!
//! // In one of the C++ source files:
//!
//! LOG_CLASS_INIT(DialogWindow);
//!
//! \endcode

#define LOG_CLASS_INIT(...) \
	const LIBCXX_NAMESPACE::logger __VA_ARGS__::logger(LOG_CLASS_INIT2(__VA_ARGS__))

//! A helper macro

//! \internal
//!
#define LOG_CLASS_INIT2(...) # __VA_ARGS__

//! stringstream class used for generating log messages.

//! This is subclassed directly from std::stringstream. We imbue the
//! string with the global locale, but with the numeric category taken
//! from the C locale. Log messages often have numbers, and such, and
//! thousands separators often look quite absurd, in this context

class log_stringstream : public std::stringstream {

public:

	//! The constructor will fix up the locale

	log_stringstream();

	//! Destructor
	~log_stringstream();
};

//! Internal macro

//! \internal
//!
#define LOG_ENTRY(obj,level,txt) \
	do {short xx_v=(level); const LIBCXX_NAMESPACE::logger &xx_l=(obj); \
		if (xx_l.getDebugLevel() <= xx_v)			\
		{							\
			LIBCXX_NAMESPACE::log_stringstream xx_s;	\
			(std::stringstream &)xx_s << txt;		\
			xx_l(xx_s.str(), xx_v);				\
		} } while(0)


//! Internal macro

//! \internal
//!
#define LOG_CUSTOM_TRACE(obj,msg)	LOG_ENTRY((obj), LOGLEVEL_TRACE, msg)

//! Internal macro

//! \internal
//!
#define LOG_CUSTOM_DEBUG(obj,msg)	LOG_ENTRY((obj), LOGLEVEL_DEBUG, msg)

//! Internal macro

//! \internal
//!
#define LOG_CUSTOM_INFO(obj,msg)	LOG_ENTRY((obj), LOGLEVEL_INFO, msg)

//! Internal macro

//! \internal
//!
#define LOG_CUSTOM_WARNING(obj,msg)	LOG_ENTRY((obj), LOGLEVEL_WARNING, msg)

//! Internal macro

//! \internal
//!
#define LOG_CUSTOM_ERROR(obj,msg)	LOG_ENTRY((obj), LOGLEVEL_ERROR, msg)

//! Internal macro

//! \internal
//!
#define LOG_CUSTOM_FATAL(obj,msg)	LOG_ENTRY((obj), LOGLEVEL_FATAL, msg)

//! Log a trace message

//! Log a message at the \c %TRACE level. \c msg is an arbitrary expression
//! that's applied to a \c std::ostream using the << operator.
#define LOG_TRACE(msg)		LOG_CUSTOM_TRACE(logger, msg)

//! Log a debug message

//! Log a message at the \c %DEBUG level. \c msg is an arbitrary expression
//! that's applied to a \c std::ostream using the << operator.
#define LOG_DEBUG(msg)		LOG_CUSTOM_DEBUG(logger, msg)

//! Log an information message

//! Log a message at the \c %INFO level. \c msg is an arbitrary expression
//! that's applied to a \c std::ostream using the << operator.
#define LOG_INFO(msg)		LOG_CUSTOM_INFO(logger, msg)

//! Log a warning message

//! Log a message at the \c %WARNING level. \c msg is an arbitrary expression
//! that's applied to a \c std::ostream using the << operator.
#define LOG_WARNING(msg)	LOG_CUSTOM_WARNING(logger, msg)

//! Log an error message

//! Log a message at the \c %ERROR level. \c msg is an arbitrary expression
//! that's applied to a \c std::ostream using the << operator.
#define LOG_ERROR(msg)		LOG_CUSTOM_ERROR(logger, msg)

//! Log a fatal error message

//! Log a message at the \c %FATAL level. \c msg is an arbitrary expression
//! that's applied to a \c std::ostream using the << operator.
#define LOG_FATAL(msg)		LOG_CUSTOM_FATAL(logger, msg)

#if 0
{
#endif
}
#endif
