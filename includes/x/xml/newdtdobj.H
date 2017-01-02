/*
** Copyright 2013 Double Precision, Inc.
** See COPYING for distribution information.
*/

#ifndef x_xml_newdtdobj_H
#define x_xml_newdtdobj_H

#include <x/xml/dtdobj.H>

namespace LIBCXX_NAMESPACE {

	namespace xml {
#if 0
	};
};
#endif

//! A modifiable external or an internal document type.

class newdtdObj : virtual public dtdObj {

public:
	//! Constructor
	newdtdObj();

	//! Destructor
	~newdtdObj();

	//! Create a general entity

	void create_general_entity(const std::string &name,
				   const std::string &value);

	//! Created a parsed external entity
	void create_parsed_entity(const std::string &name,
				  const std::string &external_id,
				  const std::string &system_id);

	//! Create an unparsed external entity
	void create_unparsed_entity(const std::string &name,
				    const std::string &external_id,
				    const std::string &system_id,
				    const std::string &ndata);

	//! Create an internal parameter entity
	void create_internal_parameter_entity(const std::string &name,
					      const std::string &value);

	//! Create an external parameter entity
	void create_external_parameter_entity(const std::string &name,
					      const std::string &external_id,
					      const std::string &system_id);
	//! Include a parameter entity.

	virtual void include_parameter_entity(const std::string &name)=0;

private:
	//! Create an entity

	//! \internal

	virtual void create_entity(const std::string &name,
				   int type,
				   const std::string &external_id,
				   const std::string &system_id,
				   const std::string &content)=0;
};

#if 0
{
	{
#endif
	}
}
#endif
