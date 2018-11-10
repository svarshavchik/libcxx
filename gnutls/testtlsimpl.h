#include "libcxx_config.h"
#include "x/gnutls/session.H"
#include "x/http/fdtlsserver.H"
#include "x/http/fdtlsclientimpl.H"
#include "x/http/clientopts_t.H"

class testsessionfactory :
	public LIBCXX_NAMESPACE::gnutls::sessionObj::factoryObj {

public:
	testsessionfactory() {}
	~testsessionfactory() {}

	LIBCXX_NAMESPACE::gnutls::session create(unsigned mode,
						 const LIBCXX_NAMESPACE::fdbase
						 &transportArg) override;
};


inline LIBCXX_NAMESPACE::gnutls::session
testsessionfactory::create(unsigned mode,
			   const LIBCXX_NAMESPACE::fdbase &transportArg)
{
	LIBCXX_NAMESPACE::gnutls::credentials::certificate
		crt(LIBCXX_NAMESPACE::gnutls::credentials::certificate
		    ::create());

	if (mode == GNUTLS_CLIENT)
	{
		crt->set_x509_trust_file("testrsa1.crt",
					 GNUTLS_X509_FMT_PEM);
	}
	else
	{
		crt->set_x509_keyfile("testrsa1.crt", "testrsa1.key",
				      GNUTLS_X509_FMT_PEM);

		LIBCXX_NAMESPACE::gnutls::dhparams
			dh(LIBCXX_NAMESPACE::gnutls::dhparams::create());

		LIBCXX_NAMESPACE::gnutls::datum_t
			dh_dat(LIBCXX_NAMESPACE::gnutls::datum_t::create());

		dh_dat->load("dhparams.dat");
		dh->import_pk(dh_dat, GNUTLS_X509_FMT_PEM);
		crt->set_dh_params(dh);
	}

	LIBCXX_NAMESPACE::gnutls::session sess=
		LIBCXX_NAMESPACE::gnutls::session::create(mode, transportArg);

	sess->credentials_set(crt);
	sess->set_default_priority();
	sess->set_private_extensions();

	return sess;
}

class testtlsimpl {

public:

	typedef LIBCXX_NAMESPACE::gnutls::http::fdtlsclientimpl fdclientimpl;

	class fdserverObj :
		public LIBCXX_NAMESPACE::gnutls::http::fdtlsserverimpl,
		virtual public LIBCXX_NAMESPACE::obj {

	public:
		fdserverObj()
		{
		}
		~fdserverObj()
		{
		}

		void run(const LIBCXX_NAMESPACE::fd &socket)
		{
			auto dummy=LIBCXX_NAMESPACE::fd::base::socketpair();
			auto session=
				LIBCXX_NAMESPACE::ref<testsessionfactory>
				::create()->create(GNUTLS_SERVER, socket);

			LIBCXX_NAMESPACE::gnutls::http::fdtlsserverimpl
				::run(socket, dummy.first, session);
		}
	};

	typedef LIBCXX_NAMESPACE::gnutls::http::fdtlsserverimpl fdserverimpl;

	class filedescObj : virtual public LIBCXX_NAMESPACE::obj {

		std::mutex mutex;

		LIBCXX_NAMESPACE::fd filedesc;
		unsigned side;
		LIBCXX_NAMESPACE::ref<testsessionfactory> factory;
		LIBCXX_NAMESPACE::gnutls::sessionptr sess;

	public:
		filedescObj(const LIBCXX_NAMESPACE::fd &filedescArg,
			    unsigned sideArg)
			: filedesc(filedescArg), side(sideArg),
			  factory(LIBCXX_NAMESPACE::ref<testsessionfactory>
				  ::create())
		{
		}

		~filedescObj() throw()
		{
		}

		std::pair<LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t,
			  LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t>
		getistream()
		{
			std::unique_lock<std::mutex> lock(mutex);

			if (sess.null())
				sess=factory->create(side, filedesc);

			return std::make_pair(LIBCXX_NAMESPACE::http::fdimplbase
					      ::input_iter_t(sess),
					      LIBCXX_NAMESPACE::http::fdimplbase
					      ::input_iter_t());
		}

		LIBCXX_NAMESPACE::http::fdimplbase::output_iter_t getostream()
		{
			std::unique_lock<std::mutex> lock(mutex);

			if (sess.null())
				sess=factory->create(side, filedesc);

			return LIBCXX_NAMESPACE::http::fdimplbase
				::output_iter_t(sess);
		}

		LIBCXX_NAMESPACE::fd &get_fd() throw()
		{
			return filedesc;
		}

		template<typename type_t>
		auto runserver(const type_t &obj)
			-> decltype(LIBCXX_NAMESPACE::run(obj, filedesc))
		{
			try {
				return LIBCXX_NAMESPACE::run(obj, filedesc);
			} catch (const LIBCXX_NAMESPACE::exception &e)
			{
				std::cerr << "server: " << e << std::endl;
				throw;
			}
		}

		void install(LIBCXX_NAMESPACE::gnutls::http::fdtlsclientimpl &obj)
		{
			obj.install(factory->create(GNUTLS_CLIENT, filedesc),
				    filedesc,
				    LIBCXX_NAMESPACE::fdptr(),
				    noverifycert);
		}

		void handshake()
		{
			std::unique_lock<std::mutex> lock(mutex);

			if (sess.null())
				sess=factory->create(side, filedesc);

			int dummy;

			sess->handshake(dummy);
		}
	};


	typedef LIBCXX_NAMESPACE::ptr<filedescObj> filedesc;

	typedef LIBCXX_NAMESPACE::ptr<fdserverObj> fdserver;


	static void createClientServerPair(filedesc &clientSide,
					   filedesc &serverSide)
	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		clientSide=filedesc::create(p.first, GNUTLS_CLIENT);
		serverSide=filedesc::create(p.second, GNUTLS_SERVER);
	}
};
