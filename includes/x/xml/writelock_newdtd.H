#ifdef x_xml_newdtd_H
#ifdef x_xml_writelock_H

inline newdtd writelockObj::get_external_dtd()
{
	return do_get_external_dtd_newdtd();
}

inline newdtd writelockObj::get_internal_dtd()
{
	return do_get_internal_dtd_newdtd();
}

#endif
#endif
