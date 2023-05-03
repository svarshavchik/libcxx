
class testimpl {

public:

	typedef LIBCXX_NAMESPACE::http::fdclientimpl fdclientimpl;

	class fdserverObj : public LIBCXX_NAMESPACE::http::fdserverimpl,
		virtual public LIBCXX_NAMESPACE::obj {

	public:
		fdserverObj()
		{
		}
		~fdserverObj()
		{
		}

		using LIBCXX_NAMESPACE::http::fdserverimpl::run;

		void run(const LIBCXX_NAMESPACE::fd &socket)
		{
			auto dummy=LIBCXX_NAMESPACE::fd::base::socketpair();

			LIBCXX_NAMESPACE::http::fdserverimpl
				::run(socket, dummy.first);
		}
	};

	typedef LIBCXX_NAMESPACE::ptr<fdserverObj> fdserver;

	typedef LIBCXX_NAMESPACE::http::fdserverimpl fdserverimpl;

	class filedescObj : virtual public LIBCXX_NAMESPACE::obj {

		LIBCXX_NAMESPACE::fdptr filedesc;

	public:
		filedescObj()
		{
		}

		filedescObj(const LIBCXX_NAMESPACE::fd &filedescArg)
			: filedesc(filedescArg)
		{
		}

		~filedescObj()
		{
		}

		std::pair<LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t,
			LIBCXX_NAMESPACE::http::fdimplbase::input_iter_t>
			getistream()
		{
			return std::make_pair(LIBCXX_NAMESPACE::http::fdimplbase
					      ::input_iter_t(filedesc),
					      LIBCXX_NAMESPACE::http::fdimplbase
					      ::input_iter_t());
		}

		LIBCXX_NAMESPACE::http::fdimplbase::output_iter_t getostream()
		{
			return LIBCXX_NAMESPACE::http::fdimplbase
				::output_iter_t(filedesc);
		}

		LIBCXX_NAMESPACE::fdptr &get_fd() noexcept
		{
			return filedesc;
		}

		template<typename type_t>
		auto runserver(const type_t &obj)
		{
			return LIBCXX_NAMESPACE::run(obj, filedesc);
		}

		void install(fdclientimpl &obj)
		{
			obj.install(filedesc, LIBCXX_NAMESPACE::fdptr());
		}
	};

	typedef LIBCXX_NAMESPACE::ptr<filedescObj> filedesc;

	static void createClientServerPair(filedesc &clientSide,
					   filedesc &serverSide)
	{
		std::pair<LIBCXX_NAMESPACE::fd, LIBCXX_NAMESPACE::fd>
			p(LIBCXX_NAMESPACE::fd::base::socketpair());

		clientSide=filedesc::create(p.first);
		serverSide=filedesc::create(p.second);
	}
};
