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

#include <boost/format.hpp>

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
using boost::format;

namespace swissguard {

class SwissGuardHandler : virtual public SwissGuardServiceIf
{
  /*
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

  virtual void computeBodyStates(BodyStates& _return, const Bodies& bodies, const PositionSystem position, const CoordinateSystem coords, const JulianDate& date) {
  	int flags = SEFLG_JPLEPH | SEFLG_SPEED;

  	//Build up the flags based on the parameters specified
  	switch (position) {
  		case PositionSystem::TrueGeocentric:
  			flags |= SEFLG_TRUEPOS;
  			break;

		case PositionSystem::Heliocentric:
			flags |= SEFLG_HELCTR;
			break;

		case PositionSystem::Barycentric:
			flags |= SEFLG_BARYCTR & SEFLG_ICRS & SEFLG_J2000 & SEFLG_NOABERR & SEFLG_NOGDEFL & SEFLG_NONUT;
			break;

		case PositionSystem::Astrometric:
			flags |= SEFLG_NOABERR & SEFLG_NOGDEFL;
			break;

		case PositionSystem::AstrometricJ2000:
			flags |= SEFLG_NOABERR & SEFLG_NOGDEFL & SEFLG_J2000 & SEFLG_NONUT;
			break;

		case PositionSystem::MeanDate:
			flags |= SEFLG_NONUT;
			break;

		case PositionSystem::MeanJ2000:
			flags |= SEFLG_J2000 & SEFLG_NONUT;
			break;

		case PositionSystem::TrueDate:
			//This is the default; leave flags as they are
			break;

		case PositionSystem::TrueJ2000:
			flags |= SEFLG_J2000;
			break;
  	}

  	switch (coords) {
  		case EclipticSpherical:
  			//This is the default; leave flags alone
  			break;

		case EclipticRectangular:
			flags |= SEFLG_XYZ;
			break;

		case EquatorialSpherical:
			flags |= SEFLG_EQUATORIAL;
			break;

		case EquatorialRectangular:
			flags |= SEFLG_EQUATORIAL | SEFLG_XYZ;
			break;
  	}

  	for (Bodies::const_iterator iter = bodies.begin(); iter != bodies.end(); ++iter) {
  		char errorMessage[256];
  		double state[6] = {0};
  		auto bodyNumber = *iter;

  		auto result = swe_calc_ut(date.jd1 + date.jd2, bodyNumber, flags, state, errorMessage);
  		if (result == ERR) {
  			cerr << "Got error getting state for body " << bodyNumber << " with flags " << flags << ": " << errorMessage << endl;

  			auto e = AstroThriftError();
  			e.message = errorMessage;

  			throw e;
  		}

  		State bodyState;

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
  */
  virtual void getVersion(std::string& _return) {
    char version[255] = {0};
    swe_version(version);

    _return = version;
  }

  virtual void calculateBodyState(State& _return, const double jdEt, const int32_t bodyId, const Flags flags) {
    double xx[6] = {0};
    char message[255] = {0};

    int ret = swe_calc(jdEt, bodyId, flags, xx, message);

    checkReturnFlag(flags, ret, "swe_calc", message);

    stateFromVector(xx, _return);
  }

  virtual void calculateBodiesState(States& _return, const double jdEt, const BodyIds& bodyIds, const Flags flags) {
    for (auto &id : bodyIds) {
      State state;
      calculateBodyState(state, jdEt, id, flags);
      _return.push_back(state);
    }
  }

  virtual void calculateBodyStateUt(State& _return, const double jdUt, const int32_t bodyId, const Flags flags) {
    double xx[6] = {0};
    char message[255] = {0};

    int ret = swe_calc_ut(jdUt, bodyId, flags, xx, message);

    checkReturnFlag(flags, ret, "swe_calc_ut", message);

    stateFromVector(xx, _return);
  }

  virtual void calculateBodiesStateUt(States& _return, const double jdUt, const BodyIds& bodyIds, const Flags flags) {
    for (auto &id : bodyIds) {
      State state;
      calculateBodyStateUt(state, jdUt, id, flags);
      _return.push_back(state);
    }
  }

  virtual void calculateFixedStarState(State& _return, const double jdEt, const std::string& starName, const Flags flags) {
    double xx[6] = {0};
    char message[255] = {0};

    int ret = swe_fixstar(const_cast<char*>(starName.c_str()), jdEt, flags, xx, message);

    checkReturnFlag(flags, ret, "swe_fixstar", message);

    stateFromVector(xx, _return);
  }

  virtual void calculateFixedStarsState(States& _return, const double jdEt, const StarNames& starNames, const Flags flags) {
    for (auto &name : starNames) {
      State state;
      calculateFixedStarState(state, jdEt, name, flags);
      _return.push_back(state);
    }
  }

  virtual void calculateFixedStarStateUt(State& _return, const double jdUt, const std::string& starName, const Flags flags) {
    double xx[6] = {0};
    char message[255] = {0};

    int ret = swe_fixstar(const_cast<char*>(starName.c_str()), jdUt, flags, xx, message);

    checkReturnFlag(flags, ret, "swe_fixstar_ut", message);

    stateFromVector(xx, _return);
  }

  virtual void calculateFixedStarsStateUt(States& _return, const double jdUt, const StarNames& starNames, const Flags flags) {
    for (auto &name : starNames) {
      State state;
      calculateFixedStarStateUt(state, jdUt, name, flags);
      _return.push_back(state);
    }
  }

  virtual void computeHouses(HouseCusps& _return, const double jdUt, const double geographicLatitude, const double geographicLongitude, const std::string& houseSystem) {
    double cusps[13] = {0};
    double ascmc[10] = {0};

    int ret = swe_houses(jdUt, geographicLatitude, geographicLongitude, static_cast<int>(houseSystem[0]), cusps, ascmc);

    checkReturnFlag(ret, "swe_houses", "<no message>");

    houseCuspsFromVectors(cusps, ascmc, _return);
  }

  virtual void computeHousesArmc(HouseCusps& _return, const double armc, const double geographicLatitude, const double eclipticObliquityDegrees, const std::string& houseSystem) {
    double cusps[13] = {0};
    double ascmc[10] = {0};

    int ret = swe_houses_armc(armc, geographicLatitude, eclipticObliquityDegrees, static_cast<int>(houseSystem[0]), cusps, ascmc);

    checkReturnFlag(ret, "swe_houses_armc", "<no message>");

    houseCuspsFromVectors(cusps, ascmc, _return);
  }

  virtual void computeHousesEx(HouseCusps& _return, const double jdUt, const Flags flags, const double geographicLatitude, const double geographicLongitude, const std::string& houseSystem) {
    double cusps[13] = {0};
    double ascmc[10] = {0};

    int ret = swe_houses_ex(jdUt, flags, geographicLatitude, geographicLongitude, static_cast<int>(houseSystem[0]), cusps, ascmc);

    checkReturnFlag(ret, "swe_houses_ex", "<no message>");

    houseCuspsFromVectors(cusps, ascmc, _return);
  }

private:
  void checkReturnFlag(int32_t flags, int32_t returnFlags, const char* methodName, const char* message) {
    //Check for errors, and throw an exception
    checkReturnFlag(returnFlags, methodName, message);

    if (returnFlags != flags) {
      //A soft error; means one of the flag bits couldn't be applied to the calculation for some reason.  For our purposes that's an error
      auto e = SwissEphemerisError();

      e.message = str(format("%s error: input flags 0x%08x, return flags 0x%08x") % methodName % flags % returnFlags);

      throw e;
    }
  }

  void checkReturnFlag(int32_t returnFlags, const char* methodName, const char* message) {
    //Check for errors, and throw an exception
    if (returnFlags < 0) {
      //A straight-up error case:
      auto e = SwissEphemerisError();

      e.message = str(format("%s error: %s") % methodName % message);

      throw e;
    } 
  }

  void stateFromVector(const double* vector, State& state) {
    state.pos.x = vector[0];
    state.pos.y = vector[1];
    state.pos.z = vector[2];

    state.vel.rx = vector[3];
    state.vel.ry = vector[4];
    state.vel.rz = vector[5];
  }

  void houseCuspsFromVectors(const double* cusps, const double* ascmc, HouseCusps& houseCusps) {
    //Note for some stupid reason, swisseph uses 1-based indexing for house cusps, with element 0 always set to 0.0
    for (auto i = 1; i < 13; i++) {
      houseCusps.cusps.push_back(cusps[i]);
    }

    houseCusps.ascendant = ascmc[SE_ASC];
    houseCusps.mc = ascmc[SE_MC];
    houseCusps.armc = ascmc[SE_ARMC];
    houseCusps.vertex = ascmc[SE_VERTEX];
    houseCusps.equatorialAscendant = ascmc[SE_EQUASC];
    houseCusps.coAscendantKoch = ascmc[SE_COASC1];
    houseCusps.coAscendantMunkasey = ascmc[SE_COASC2];
    houseCusps.polarAscendant = ascmc[SE_POLASC];
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