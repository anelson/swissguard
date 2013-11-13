/** Defines the service and types exposed by swissguard to wrap the swisseph library */
namespace cpp swissguard
namespace java swissguard

enum Body {
  Sun = 0,
  Moon = 1,
  Mercury = 2,
  Venus = 3,
  Mars = 4,
  Jupiter = 5,
  Saturn = 6,
  Uranus = 7,
  Neptune = 8,
  Pluto = 9,
  MeanNode = 10,
  TrueNode = 11,
  MeanApogee = 12,
  OsculatingApogee = 13,
  Earth = 14,
  Chiron = 15,
  Pholus = 16,
  Ceres = 17,
  Pallas = 18,
  Juno = 19,
  Vesta = 20
}

enum PositionSystem {
	/* SEFLG_TRUEPOS (no corrections for light time ) */
	TrueGeocentric = 2,

	/* SEFLG_HELCTR */
	Heliocentric = 3,

	/* SEFLG_BARYCTR */
	Barycentric = 4,

	/* Position corrected for light time but not for annual aberration or gravitational deflection of light by the sun.  SEFLG_NOABERR and SEFLG_NOGDEFL */
	Astrometric = 9,

	/* Asme as Astrometric, but but with SEFLG_J2000 and SEFLG_NONUT as well */
	AstrometricJ2000 = 10,

	/* SEFLG_NONUT */
	MeanDate = 5,

	/* SEFLG_NONUT and SEFLG_J2000 */
	MeanJ2000 = 6,

	/* No flags */
	TrueDate = 7,

	/* SEFLG_J2000 */
	TrueJ2000 = 8
}

enum CoordinateSystem {
	/* No additional flags */
	EclipticSpherical = 1,

	/** SEFLG_XYZ */
	EclipticRectangular = 2,

	/* SEFLG_EQUATORIAL */
	EquatorialSpherical = 3,

	/* SEFLG_EQUATORIAL | SEFLG_XYZ */
	EquatorialRectangular = 4
}

/** Position of a body, either long, lat, distance or x, y, z depending upon coordinate system */
struct Position {
	1: double x,
	2: double y,
	3: double z
}

/** Velocity of a body, either deg long/day, deg lat/day, distance/day, or rx, ry, rz depending upon coordinate system */
struct Velocity {
	1: double rx,
	2: double ry,
	3: double rz
}

struct State {
	1: Position pos,
	2: Velocity vel
}

typedef list<Body> Bodies

typedef map<Body, State> BodyStates

/** Sun and all the planets; for some reason thrift throws an error if the symbolic names of the enum values are used so make do with
repeating the same int literals at before */
const Bodies AllSolarBodies = [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 14]

/* Julian date, expressed in Universal Time (UT) */
struct JulianDate {
	1: double jd1,
	2: double jd2
}

exception SwissEphemerisError {
	1: string message
}

service SwissGuard {
	/** Sets the path of the JPL ephemeris file to use.  This must be called first and refer to a JPL ephemeris file that swisseph
	knows how to read, or attempts to get body state wil fail */
	void setJplFile(1: string jplFilePath),

	BodyStates computeBodyStates(1: Bodies bodies, 2: PositionSystem position, 3: CoordinateSystem coords, 4: JulianDate date)
}

