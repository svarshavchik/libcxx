#include <x/fdlistener.H>
#include <x/netaddr.H>
#include <x/http/fdserver.H>
#include <x/http/fdserverimpl.H>
#include <x/http/form.H>
#include <x/xml/escape.H>
#include <iterator>

class serverObj : public x::http::fdserverimpl, virtual public x::obj {

public:
	x::fdlistener daemon;

	serverObj(const x::fdlistener &daemonArg) : daemon(daemonArg)
	{
	}

	~serverObj() noexcept
	{
	}

	struct file {
		std::string name;
		std::string filename;
		size_t size;

		file(const std::string &nameArg,
		     const std::string &filenameArg)
			: name(nameArg), filename(filenameArg), size(0)
		{
		}
	};

	void received(const x::http::requestimpl &req, bool hasbody)
	{
		std::vector<file> files;

		std::pair<x::http::form::parameters, bool> form=this->getform
			(req, hasbody,
			 [&files]
			 (const x::headersbase &headers,
			  const std::string &name_utf8,
			  const std::string &filename_utf8,
			  x::http::form::parameters::base::filereceiver &recv)
			 {
				 files.emplace_back(name_utf8,
						    filename_utf8);
				 recv.receive([&files]
					      (const std::vector<char> &chunk) {
						      files.back().size
							      += chunk.size();
					      },
					      [] {
						      // Close not implemented
					      });
			 }, "UTF-8");


		if (!form.second && hasbody)
		{
			received_unknown_request(req, hasbody);
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
			o << "<h1>Form upload test</h1>";

			if (!files.empty())
			{
				o << "<table>"
					"<thead>"
					"<tr>"
					"<th>field name</th>"
					"<th>filename</th>"
					"<th>size</th>"
					"</tr>"
					"</thead>"
					"<tbody>";

				for (const auto &file:files)
				{
					o << "<tr><td>"
					  << x::xml::escapestr(file.name)
					  << "</td><td>"
					  << x::xml::escapestr(file.filename)
					  << "</td><td>"
					  << file.size
					  << "</td></tr>";
				}
				o << "</tbody></table>";
			}

			o << "<h2>Upload a file:</h2><form method='post' enctype='multipart/form-data'><table><tbody><tr><td>Upload a file:</td><td><input type=file name=uploadfile /></td></tr>"
				"<tr><td colspan=2><input type=submit name=submit value='Submit Form' /></td></tr>"
				"<tr><td colspan=2><hr /></td></tr><tr><td colspan=2><input type=submit name=stop value='Stop server' /></td></tr></tbody></table>";
		}

		o << "</body></html>";

		send(req, 
		     "text/html; charset=utf-8",
		     std::istreambuf_iterator<char>(o.rdbuf()),
		     std::istreambuf_iterator<char>());

		if (stop)
		{
			daemon->stop();
		}
	}
};

class serverfactoryObj : virtual public x::obj {

public:
	x::fdlistener daemon;

	serverfactoryObj(const x::fdlistener &daemonArg) : daemon(daemonArg) {}
	~serverfactoryObj() noexcept {}

	x::ref<serverObj> create()
	{
		return x::ref<serverObj>::create(daemon);
	}
};

int main()
{
	try {
		auto listener=({

				std::list<x::fd> socketlist;

				x::netaddr::create("", 0)
					->bind(socketlist, false);

				std::cout << "Listening on port "
					  << socketlist.front()->getsockname()
					->port()
					  << std::endl;
				x::fdlistener::create(socketlist);
			});

		listener->start(x::http::fdserver::create(),
				x::ref<serverfactoryObj>::create(listener));
		listener->wait();

	} catch (const x::exception &e)
	{
		std::cerr << e << std::endl;
		exit(1);
	}
	return 0;
}
