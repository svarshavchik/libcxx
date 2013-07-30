diff --git a/gnutls/sessionobj.C b/gnutls/sessionobj.C
index b1827b4..4c2832f 100644
--- gnutls/sessionobj.C
+++ gnutls/sessionobj.C
@@ -68,7 +68,7 @@ gnutls::sessionObj::sessionObj(unsigned modeArg,
 {
 	gnutls::init::gnutls_init();
 
-	chkerr(gnutls_init(&sess, mode), "gnutls_init");
+	chkerr(gnutls_init(&sess, (gnutls_connection_end_t)mode), "gnutls_init");
 	gnutls_session_set_ptr(sess, this);
 	gnutls_transport_set_ptr2(sess,
 				  (gnutls_transport_ptr_t) &transport,
