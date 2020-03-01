@LIBCXX_AM@

noinst_PROGRAMS=\
	classhier			\
	http_authserver			\
	http_client			\
	http_server			\
	http_server_upload		\
	mimeencoder			\
	 #

classhier_SOURCES=classhier.C
classhier_LDADD=-lcxx

http_authserver_SOURCES=http_authserver.C
http_authserver_LDADD=-lcxxtls -lcxx

http_client_SOURCES=http_client.C
http_client_LDADD=-lcxxtls -lcxx

http_server_SOURCES=http_server.C
http_server_LDADD=-lcxxtls -lcxx

http_server_upload_SOURCES=http_server_upload.C
http_server_upload_LDADD=-lcxx

mimeencoder_SOURCES=mimeencoder.C
mimeencoder_LDADD=-lcxx
