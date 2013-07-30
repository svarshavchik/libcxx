diff --git a/gnutls/x509_crtobj.C b/gnutls/x509_crtobj.C
index c76f135..15a692a 100644
--- gnutls/x509_crtobj.C
+++ gnutls/x509_crtobj.C
@@ -283,7 +283,7 @@ void gnutls::x509::crtObj::get_basic_constraints(bool &ca,
 						 int &pathLenConstraint)
 	const
 {
-	unsigned int caRet;
+	int caRet;
 	unsigned criticalRet;
 
 	chkerr(gnutls_x509_crt_get_basic_constraints(crt, &criticalRet,
@@ -466,7 +466,7 @@ void gnutls::x509::crtObj::get_all_extensions_list(std::list< std::pair<std::str
 
 	while (1)
 	{
-		unsigned int critflag=0;
+		int critflag=0;
 		size_t buf_s=0;
 		int rc=gnutls_x509_crt_get_extension_info(crt, indx,
 							  NULL, &buf_s,
