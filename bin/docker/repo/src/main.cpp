// media.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <Ice/Ice.h>
#include "streamI.h"
#include "glog/logging.h"

class MediaServer : public Ice::Application
{
  public:
	MediaServer();
	~MediaServer();
	virtual int run(int, char *[]);
	virtual void interruptCallback(int);

  private:
};

MediaServer::MediaServer()
{
}

MediaServer ::~MediaServer()
{
}
int MediaServer::run(int argc, char *[])
{
	if (argc > 1)
	{
		std::cerr << appName() << ": too many arguments" << std::endl;
		return EXIT_FAILURE;
	}

	try
	{

#ifdef STANDALONE
		Ice::Identity id = Ice::stringToIdentity("Stream");
#else
		Ice::PropertiesPtr properties = communicator()->getProperties();
		LOG(INFO) << "Property is : " << properties->getProperty("Identity") << std::endl;
		Ice::Identity id = Ice::stringToIdentity(properties->getProperty("Identity"));
#endif // !STANDALONE

		Ice::ObjectAdapterPtr adapter = communicator()->createObjectAdapter("Stream");

		adapter->add(std::make_shared<StreamI>(), id);
		adapter->activate();
	}
	catch (const Ice::Exception &e)
	{
		//initiated_ = false;
		LOG(ERROR) << e.what();
		return EXIT_SUCCESS;
	}
	catch (const std::exception &e)
	{
		LOG(ERROR) << e.what();
		return EXIT_SUCCESS;
	}

	communicator()->waitForShutdown();
	return EXIT_SUCCESS;
}

void MediaServer::interruptCallback(int)
{
	LOG(ERROR) << "Shutting down ICE ";
	communicator()->shutdown();
}

void run(int argc, char *argv[])
{
	int status = 0;

	try
	{
		//
		// CtrlCHandler must be created before the communicator or any other threads are started
		//
		Ice::CtrlCHandler ctrlCHandler;

		//
		// CommunicatorHolder's ctor initializes an Ice communicator,
		// and its dtor destroys this communicator.
		//
		Ice::CommunicatorHolder ich(argc, argv, "config.mediaserver");
		auto communicator = ich.communicator();

		ctrlCHandler.setCallback(
			[communicator](int) {
				std::cout << "shutting down" << std::endl;
				communicator->destroy();
			});

		//
		// The communicator initialization removes all Ice-related arguments from argc/argv
		//
		if (argc > 1)
		{
			std::cerr << argv[0] << ": too many arguments" << std::endl;
			status = 1;
		}
		else
		{
			// auto adapter = communicator->createObjectAdapter("Hello");
			// adapter->add(make_shared<HelloI>(_workQueue), Ice::stringToIdentity("hello"));

			// adapter->activate();
			Ice::Identity id = Ice::stringToIdentity("Stream");
			Ice::ObjectAdapterPtr adapter = communicator->createObjectAdapter("Stream");
			adapter->add(std::make_shared<StreamI>(), id);
			adapter->activate();

			communicator->waitForShutdown();
		}
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		status = 1;
	}

	//return status;
}
int main(int argc, char *argv[])
{
	google::InitGoogleLogging(argv[0]);
	FLAGS_log_dir = "./log";
	FLAGS_stderrthreshold = 0;
	FLAGS_logtostderr = 0;

	//MediaServer app;
#ifdef STANDALONE
	int status = 0;
	//int status = app.main(argc, argv, "config.mediaserver");
	run(argc, argv);
#else
	int status = app.main(argc, argv);
#endif // STANDALONE

	return status;
}
