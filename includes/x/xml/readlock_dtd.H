#ifdef x_xml_dtd_H
#ifdef x_xml_readlock_H

inline dtd readlockObj::get_external_dtd() const
{
	return do_get_external_dtd_dtd();
}

inline dtd readlockObj::get_internal_dtd() const
{
	return do_get_internal_dtd_dtd();
}

#endif
#endif
