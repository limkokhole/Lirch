#include <vector>
#include <iostream>
#include <csignal>
#include <string>

#include <thread>

#include <QCoreApplication>

#include "message.h"
#include "message_pipe.h"
#include "core_messages.h"
#include "lirch_constants.h"
#include "lirch_non_qt.h"

using namespace std;

extern message_pipe in_pipe;
extern bool verbose;

void run_core(const vector<message> &vm);

void handle_sigint(int)
{
	//Quit nicely when we get a ctrl-c, but not if we get it twice.  We
	//need to unregister before we write because we don't want to lock the
	//mutex twice.  This should only be a problem under heavy load (which
	//is exactly when we don't want to block SIGINT).
	signal(SIGINT, SIG_DFL);
	in_pipe.write(core_quit_message::create());
}

int main(int argc, char *argv[])
{
	if (signal(SIGINT, handle_sigint)==SIG_IGN)
		signal(SIGINT, SIG_IGN);
	QCoreApplication app(argc, argv);
	vector<message> vm;
	bool preload=true;
	char **arg=argv; //Skip the program name
	if (argc!=0)
	{
		for (arg=argv+1; *arg; ++arg)
		{
			if (*arg==string("-h") || *arg==string("--help"))
			{
				cout << "Usage: " << argv[0] << " [options] [-- [plugins]]\n"
				        " -h --help         Print help and exit\n"
				        " -l --list-preload List the preloaded plugins and exit\n"
				        " -n --no-preload   Don't load the preloaded plugins\n"
				        " -v --verbose      Make the core generate debug output\n"
				        "                   Otherwise, the next 2 arguments are recognized as\n"
				        "                   a 'plugin name' 'filename' pair\n";
				return 1;
			}
			else if (*arg==string("-l") || *arg==string("--list-preload"))
			{
				for (auto &p : preloads)
				{
					cout << p.name << ": " << p.filename << endl;
				}
				return 2;
			}
			else if (*arg==string("-v") || *arg==string("--verbose"))
				verbose=true;
			else if (*arg==string("-n") || *arg==string("--no-preload"))
				preload=false;
			else if (*arg==string("--"))
			{
				++arg;
				break;
			}
			else if (**arg=='-')
			{
				cout << "Unrecognized option: " << *arg << endl;
				return 3;
			}
			else
			{
				if (arg[1])
				{
					vm.push_back(plugin_adder::create(arg[0],arg[1]));
					++arg;
				}
			}
		}
	}
	// Preload a variety of plugins specified in build (see lirch_constants.h)
	vector<message> pp;
	if (preload)
	{
		for (auto &p : preloads)
		{
			pp.push_back(plugin_adder::create(string(p.name), string(p.filename)));
		}
		//Append the command-line plugins to the preloaded plugins
	}
	pp.insert(pp.end(), vm.begin(), vm.end());
	// Load the rest of the plugins specified on command line
	while (arg[0] && arg[1])
	{
		pp.push_back(plugin_adder::create(arg[0],arg[1]));
		arg+=2;
	}

	thread t([&app, pp](){
		run_core(pp);
		core_notifier notifier;
		QObject::connect(&notifier, SIGNAL(quit()), &app, SLOT(quit()));
		notifier.emitQuit();
	});
	// Wait until core shutdown
	core_waiter waiter(t);
	QObject::connect(&app, SIGNAL(aboutToQuit()), &waiter, SLOT(onQuit()));
	app.exec();
}
