#include <x/fdlistener.H>
#include <x/netaddr.H>
#include <x/http/fdserver.H>
#include <x/http/serverauth.H>
#include <x/http/form.H>
#include <iterator>

class serverObj : public x::http::fdserverimpl, virtual public x::obj {

	x::http::serverauth serverauth;

public:
	serverObj(const x::http::serverauth &serverauthArg)
		: serverauth(serverauthArg)
	{
	}

	~serverObj()
	{
	}

	void received(const x::http::requestimpl &req, bool hasbody) override;
};

void serverObj::received(const x::http::requestimpl &req, bool hasbody)
{
	x::http::responseimpl resp;
	std::string username;
	std::list<x::http::responseimpl::challenge_info> challenges;

	auto scheme=serverauth->check_authentication
		(req, resp, username, challenges,
		 []
		 (gcry_md_algos algorithm,
		  const std::string &realm,
		  const std::string &username,
		  const x::uriimpl &uri)
		 {
			 if (username != "citizenkane")
				 return std::string();

			 return x::http::serverauth::base::compute_a1(algorithm,
								      username,
								      "rosebud",
								      realm);
		 });

	if (hasbody)
		discardbody();

	if (scheme == x::http::auth::unknown)
		x::http::responseimpl::throw_unauthorized(challenges);

	std::string content=
		"Congratulations, you've authenticated as " + username + "\n";

	resp.append(x::mime::structured_content_header::content_type,
		    "text/plain; charset=UTF-8");

	send(resp, req, content.begin(), content.end());
}

class serverfactoryObj : virtual public x::obj {

	x::http::serverauth serverauth;

public:
	serverfactoryObj()
		: serverauth(x::http::serverauth::create("HTTP AuthServer",
							 "/"))
	{
	}

	~serverfactoryObj()
	{
	}

	x::ref<serverObj> create()
	{
		return x::ref<serverObj>::create(serverauth);
	}
};

void serverauth()
{
	std::list<x::fd> socketlist;

	x::netaddr::create("", 0)->bind(socketlist, false);

	auto listener=x::fdlistener::create(socketlist);

	listener->start(x::http::fdserver::create(),
			x::ref<serverfactoryObj>::create());

	std::cout << "Listening on http://localhost:"
			  << socketlist.front()->getsockname()->port()
			  << std::endl
		  << "Press ENTER when done: " << std::flush;

	std::string dummy;

	std::getline(std::cin, dummy);
}

int main()
{
	try {
		serverauth();

	} catch (const x::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
