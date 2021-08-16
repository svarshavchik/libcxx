/*
** Copyright 2012-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_http_uploadobj_H
#define x_http_uploadobj_H

#include <x/http/useragentobj.H>
#include <x/mime/encoder.H>
#include <x/tuple_param_pack.H>
#include <x/obj.H>
#include <x/namespace.h>

#include <list>
#include <tuple>

namespace LIBCXX_NAMESPACE::http {
#if 0
}
#endif

//! A collection of files to upload in a request.

//! \see upload

class uploadObj : virtual public obj {

	//! Base class for file upload entries.

	//! \internal
	class filebaseObj : virtual public obj {

	public:
		//! Constructor
		filebaseObj();

		//! Destructor
		~filebaseObj();

		//! Add the file to an upload request.

		//! Invokes ua.add_file_upload(), forwarding parameters.

		virtual void add_file_upload(useragentObj &ua,
					     const std::string &charset,
					     std::list<mime::encoder> &list)=0;
	};

	//! Helper class for unpacking the file upload tuple into a param pack.

	template<typename Args> class add_to_request;

	//! A file to upload

	//! add() captures its parameters in a tuple, instantiating this class.
	//! add_file_upload() unpacks the tuple, and calls add_file_upload()
	//! in the invoking user agent object.

	template<typename ...Args>
	class fileimplObj : public filebaseObj {

	public:
		//! Captured parameters.

		std::tuple<Args...> args;

		//! Constructor
		template<typename ...InitArgs>
		fileimplObj(InitArgs && ...initArgs)
			: args(std::forward<InitArgs>(initArgs)...) {}

		//! Destructor
		~fileimplObj() {}

		//! Add a file upload object to the request.

		void add_file_upload(useragentObj &ua,
				     const std::string &charset,
				     std::list<mime::encoder> &list) override
		{
			add_to_request<typename
				       tuple_2_param_pack<sizeof...(Args)>
				       ::type>::add(ua, charset, list, args);
		}
	};

	//! Collected list of files to upload.

	std::list<ref<filebaseObj>> files;

public:

	//! Constructor
	uploadObj();

	//! Destructor
	~uploadObj();

	//! Add a file upload

	//! The parameters can be any one of the parameters used in the
	//! file upload version of \ref useragentObj::request() "useragent->request"().
	//! The parameters get captured and forwarded to one of the
	//! \ref useragentObj::add_file_upload "useragent->add_file_upload"()
	//! methods.

	template<typename ...Args>
	void add(Args && ...args)
	{
		files.push_back(ref<fileimplObj<typename std::decay<Args>
				::type...>>::create(std::forward<Args>(args)...)
				);
	}

	friend class useragentObj;

private:

	//! Add file upload parameters

	//! This is invoked by
	//! \ref useragentObj::request() "useragent->request"(). Iterate
	//! over the file upload objects, and call
	//! \ref useragentObj::add_file_upload "useragent->add_file_upload"()
	//! for each one of them.

	void add_file_upload(useragentObj &ua,
			     const std::string &charset,
			     std::list<mime::encoder> &list);
};

//! Convert file upload parameter tuple to a paramater pack.

//! Invokes \ref useragentObj::add_file_upload "useragent->add_file_upload"()
//! passing the parameters from the unpacked tuple.
//!
//! \internal

template<size_t ...n>
class uploadObj::add_to_request<tuple_param_pack<n...>> {

 public:

	//! Invoke add_file_upload().

	template<typename tuple_type>
	static void add(useragentObj &ua,
			const std::string &charset,
			std::list<mime::encoder> &list,
			const tuple_type &tuple)
	{
		ua.add_file_upload(list, charset, std::get<n>(tuple)...);
	}
};

//! File upload

template<typename ...Args>
void useragentObj::request_form_uploads(std::list<mime::encoder> &list,
					const std::string &charset,
					const upload &upload_list,
					Args && ...args)
{
	upload_list->add_file_upload(*this, charset, list);
	request_form_uploads(list, charset,
			     std::forward<Args>(args)...);
}

#if 0
{
#endif
}
#endif
