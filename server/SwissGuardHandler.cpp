#include "gen-cpp/SwissGuardService.h"

#include <stdint.h>
#include <cmath>
#include <protocol/TBinaryProtocol.h>
#include <server/TSimpleServer.h>
#include <transport/TServerSocket.h>
#include <transport/TBufferTransports.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/server/TThreadedServer.h>

#include "swephexp.h"

#define SECONDS_PER_DAY 86400.0


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::apache::thrift::concurrency;
using boost::shared_ptr;
using std::cout;
using std::cerr;
using std::endl;

namespace swissguard {

class SwissGuardHandler : virtual public SwissGuardServiceIf
{
  virtual void setJplFile(const std::string& jplFilePath) {
  	//Of course could use boost for this but running boost under OS X right now is a royal PITA
  	int beginIdx = jplFilePath.rfind('/');
  	auto fileName = jplFilePath.substr(beginIdx + 1);
  	auto directory = jplFilePath.substr(0, beginIdx);

  	swe_set_ephe_path(const_cast<char*>(directory.c_str()));
  	swe_set_jpl_file(const_cast<char*>(fileName.c_str()));

  	cout << "swisseph path set to " << directory << endl;
  	cout << "swisseph JPL file set to " << fileName << endl;
  }

  virtual void computeBodyStates(astrothrift::BodyStates& _return, const astrothrift::Bodies& bodies, const astrothrift::PositionSystem position, const astrothrift::CoordinateSystem coords, const astrothrift::JulianDate& date) {
  	int flags = SEFLG_JPLEPH | SEFLG_SPEED;

  	//Build up the flags based on the parameters specified
  	switch (position) {
  		case astrothrift::PositionSystem::TrueGeocentric:
  			flags |= SEFLG_TRUEPOS;
  			break;

		case astrothrift::PositionSystem::Heliocentric:
			flags |= SEFLG_HELCTR;
			break;

		case astrothrift::PositionSystem::Barycentric:
			flags |= SEFLG_BARYCTR & SEFLG_ICRS & SEFLG_J2000 & SEFLG_NOABERR & SEFLG_NOGDEFL & SEFLG_NONUT;
			break;

		case astrothrift::PositionSystem::Astrometric:
			flags |= SEFLG_NOABERR & SEFLG_NOGDEFL;
			break;

		case astrothrift::PositionSystem::AstrometricJ2000:
			flags |= SEFLG_NOABERR & SEFLG_NOGDEFL & SEFLG_J2000 & SEFLG_NONUT;
			break;

		case astrothrift::PositionSystem::MeanDate:
			flags |= SEFLG_NONUT;
			break;

		case astrothrift::PositionSystem::MeanJ2000:
			flags |= SEFLG_J2000 & SEFLG_NONUT;
			break;

		case astrothrift::PositionSystem::TrueDate:
			//This is the default; leave flags as they are
			break;

		case astrothrift::PositionSystem::TrueJ2000:
			flags |= SEFLG_J2000;
			break;
  	}

  	switch (coords) {
  		case astrothrift::EclipticSpherical:
  			//This is the default; leave flags alone
  			break;

		case astrothrift::EclipticRectangular:
			flags |= SEFLG_XYZ;
			break;

		case astrothrift::EquatorialSpherical:
			flags |= SEFLG_EQUATORIAL;
			break;

		case astrothrift::EquatorialRectangular:
			flags |= SEFLG_EQUATORIAL | SEFLG_XYZ;
			break;
  	}

  	for (astrothrift::Bodies::const_iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
  		char errorMessage[256];
  		double state[6] = {0};
  		auto bodyNumber = *iter;

  		auto result = swe_calc_ut(date.jd1 + date.jd2, bodyNumber, flags, state, errorMessage);
  		if (result == ERR) {
  			cerr << "Got error getting state for body " << bodyNumber << " with flags " << flags << ": " << errorMessage << endl;

  			auto e = astrothrift::AstroThriftError();
  			e.message = errorMessage;

  			throw e;
  		}

  		astrothrift::State bodyState;

  		bodyState.pos.x = state[0];
  		bodyState.pos.y = state[1];
  		bodyState.pos.z = state[2];
  		bodyState.vel.rx = state[3];
  		bodyState.vel.ry = state[4];
  		bodyState.vel.rz = state[5];

      //When dealing with cartesian coordinates, speed is returned in AU/day.  It should be AU/sec per astrothrift
      if (flags & SEFLG_XYZ) {
        bodyState.vel.rx /= SECONDS_PER_DAY;
        bodyState.vel.ry /= SECONDS_PER_DAY;
        bodyState.vel.rz /= SECONDS_PER_DAY;
      }

  		_return[bodyNumber] = bodyState;
  	}
  }
};

}

int main (int argc, char **argv)
{
  int port = 9090;
  shared_ptr<swissguard::SwissGuardHandler> handler(new swissguard::SwissGuardHandler());
  shared_ptr<TProcessor> processor(new swissguard::SwissGuardServiceProcessor(handler));
  shared_ptr<TServerTransport> serverTransport(new TServerSocket(port));
  shared_ptr<TTransportFactory> transportFactory(new TFramedTransportFactory());
  shared_ptr<TProtocolFactory> protocolFactory(new TBinaryProtocolFactory());
  shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager (1);
  shared_ptr<PosixThreadFactory> threadFactory    = shared_ptr<PosixThreadFactory> (new PosixThreadFactory ());
  threadManager -> threadFactory (threadFactory);
  threadManager -> start ();

 cout << "Listening on port " << port << " for Thrift connections" << endl;
 
 /* This time we'll try using a TThreadedServer, a better server than the TSimpleServer in the last tutorial */
 TThreadedServer server(processor, serverTransport, transportFactory, protocolFactory);
 server.serve();
 
 return 0;
}