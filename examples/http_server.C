#include <x/fdlistener.H>
#include <x/netaddr.H>
#include <x/http/fdserver.H>
#include <x/http/fdserverimpl.H>
#include <x/http/fdtlsserver.H>
#include <x/http/fdtlsserverimpl.H>
#include <x/http/form.H>
#include <x/gnutls/session.H>
#include <x/xml/escape.H>
#include <iterator>

class listenersObj : virtual public x::obj {

public:
	x::gnutls::session::base::factory factory;

	x::fdlistener http_listener;
	x::fdlistener https_listener;

	static x::fdlistener create_listener(const char *port)
	{
		std::list<x::fd> socketlist;

		x::netaddr::create("", 0)->bind(socketlist, false);

		std::cout << "Listening on " << port << " port "
			  << socketlist.front()->getsockname()->port()
			  << std::endl;

		return x::fdlistener::create(socketlist);
	}

	static x::gnutls::session::base::factory create_factory()
	{
		std::cout << "Creating certificate" << std::endl;

		x::gnutls::credentials::certificate
			cred(x::gnutls::credentials::certificate::create());

		x::gnutls::x509::privkey
			key(x::gnutls::x509::privkey::create());

		key->generate(GNUTLS_PK_RSA, GNUTLS_SEC_PARAM_NORMAL);
		key->fix();

		x::gnutls::x509::crt cert(x::gnutls::x509::crt::create());

		time_t now=time(NULL);

		cert->set_basic_constraints(true, 1);
		cert->set_activation_time(now-60);
		cert->set_expiration_time(now + 60 * 60 * 24 * 30);
		cert->set_dn_by_oid(GNUTLS_OID_X520_COMMON_NAME, "localhost");
		cert->set_issuer_dn_by_oid(GNUTLS_OID_X520_COMMON_NAME,
					   "localhost");
		cert->set_key(key);
		cert->set_serial(1);
		cert->set_version();
		cert->sign(cert, key);

		std::list<x::gnutls::x509::crt> certchain;

		certchain.push_back(cert);

		cred->set_key(certchain, key);

		x::gnutls::session::base::factory f(x::gnutls::session::base
						    ::factory::create());

		f->credentials_set(cred);
		return f;
	}

	listenersObj()
		: factory(create_factory()),
		  http_listener(create_listener("http")),
		  https_listener(create_listener("https"))
	{
	}

	~listenersObj() noexcept {}
};

typedef x::ref<listenersObj> listeners;

template<typename impl_class>
class myserverimpl : public impl_class, virtual public x::obj {

public:
	listeners daemons;

	myserverimpl(const listeners &daemonsArg) : daemons(daemonsArg)
	{
	}

	~myserverimpl() noexcept
	{
	}

	void received(const x::http::requestimpl &req, bool hasbody)
	{
		std::pair<x::http::form::parameters, bool>
			form=this->getform(req, hasbody);

		if (!form.second && hasbody)
		{
			impl_class::received_unknown_request(req, hasbody);
			// Throws an exception.
			return;
		}

		std::stringstream o;

		o << "<html><head><title>Your request</title></head><body>";

		bool stop=form.first->find("stop") != form.first->end();

		if (stop)
		{
			o << "<h1>Goodbye</h1>";
		}
		else
		{
			o << "<h1>Your request headers:</h1>"
				"<p>URI: "
			  << x::xml::escapestr(x::tostring(req.getURI()))
			  << "</p><table><thead><tr><th>Header</th><th>Contents</th></tr></thead><tbody>";

			for (auto hdr:req)
			{
				o << "<tr><td>" << x::xml::escapestr(hdr.first)
				  << "</td><td>"
				  << x::xml::escapestr(hdr.second.begin(),
						       hdr.second.end())
				  << "</td></tr>";
			}
			o << "</tbody></table>"
			  << "<h1>Your form parameters</h1>"
				"<table><thead><tr><th>Parameter</th><th>Value</th></tr></thead><tbody>";

			for (auto p:*form.first)
			{
				o << "<tr><td>" << x::xml::escapestr(p.first)
				  << "</td><td>"
				  << x::xml::escapestr(p.second)
				  << "</td></tr>";
			}
			o << "</tbody></table><h2>Submit a form, and see what happens:</h2><form method='post'><table><tbody><tr><td>Text input:</td><td><input type=text name=textfield /></td></tr>"
				"<tr><td>Checkbox:</td><td><input type=checkbox name=yesno /></td></tr>"
				"<tr><td colspan=2><input type=submit name=submit value='Submit Form' /></td></tr>"
				"<tr><td colspan=2><hr /></td></tr><tr><td colspan=2><input type=submit name=stop value='Stop server' /></td></tr></tbody></table>";
		}

		o << "</body></html>";

		this->send(req, 
			   "text/html; charset=utf-8",
			   std::istreambuf_iterator<char>(o.rdbuf()),
			   std::istreambuf_iterator<char>());

		if (stop)
		{
			daemons->http_listener->stop();
			daemons->https_listener->stop();
		}
	}
};

template<typename server_type>
class myhttpserver : virtual public x::obj {

public:
	listeners daemons;

	myhttpserver(const listeners &daemonsArg) : daemons(daemonsArg)
	{
	}

	~myhttpserver() noexcept {}

	server_type create()
	{
		return server_type::create(daemons);
	}
};

typedef x::ref<myhttpserver< x::ref<myserverimpl<x::http::fdserverimpl> >
			     > > http_instance;

typedef x::ref<myhttpserver< x::ref<myserverimpl<x::gnutls::http
						 ::fdtlsserverimpl> >
			     > > https_instance;

int main()
{
	try {
		auto daemons(listeners::create());

		daemons->http_listener
			->start(x::http::fdserver::create(),
				http_instance::create(daemons));

		daemons->https_listener
			->start(x::gnutls::http::fdtlsserver::create(),
				daemons->factory,
				https_instance::create(daemons));

		daemons->http_listener->wait();
		daemons->https_listener->wait();

	} catch (const x::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
