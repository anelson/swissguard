include "astro.thrift"

/** Defines the service and types exposed by swissguard to wrap the swisseph library */
namespace cpp swissguard
namespace java swissguard

exception SwissEphemerisError {
	1: string message
}

service SwissGuardService extends astro.AstroThriftService {
	/** Sets the path of the JPL ephemeris file to use.  This must be called first and refer to a JPL ephemeris file that swisseph
	knows how to read, or attempts to get body state wil fail */
	void setJplFile(1: string jplFilePath) throws (1: SwissEphemerisError see)
}

