namespace cpp swissguard
namespace java swissguard

enum BodyId {
	SUN = 0,
	MOON = 1,
	MERCURY = 2,
	VENUS = 3,
	MARS = 4,
	JUPITER = 5,
	SATURN = 6,
	URANUS = 7,
	NEPTUNE = 8,
	PLUTO = 9,
	MEAN_NODE = 10,
	TRUE_NODE = 11,
	MEAN_APOG = 12,
	OSCU_APOG = 13,
	EARTH = 14,
	CHIRON = 15,
	PHOLUS = 16,
	CERES = 17,
	PALLAS = 18,
	JUNO = 19,
	VESTA = 20,
	INTP_APOG = 21,
	INTP_PERG = 22,
	NPLANETS = 23,
	AST_OFFSET = 10000,
	VARUNA = 30000 /* AST_OFFSET + 20000 */,
	FICT_OFFSET = 40,
	FICT_OFFSET_1 = 39,
	FICT_MAX = 999,
	NFICT_ELEM = 15,
	COMET_OFFSET = 1000,
	NALL_NAT_POINTS = 38 /*(NPLANETS + NFICT_ELEM) */,
	CUPIDO = 40,
	HADES = 41,
	ZEUS = 42,
	KRONOS = 43,
	APOLLON = 44,
	ADMETOS = 45,
	VULKANUS = 46,
	POSEIDON = 47,
	ISIS = 48,
	NIBIRU = 49,
	HARRINGTON = 50,
	NEPTUNE_LEVERRIER = 51,
	NEPTUNE_ADAMS = 52,
	PLUTO_LOWELL = 53,
	PLUTO_PICKERING = 54,
	VULCAN = 55,
	WHITE_MOON = 56,
	PROSERPINA = 57,
	WALDEMATH = 58
}

enum Flags {
	JPLEPH = 1,
	SWIEPH = 2,
	MOSEPH = 4,
	HELCTR = 8,
	TRUEPOS = 16,
	J2000 = 32,
	NONUT = 64,
	SPEED3 = 128,
	SPEED = 256,
	NOGDEFL = 512,
	NOABERR = 1024,
	EQUATORIAL = 2048 /*(2*1024)*/,
	XYZ = 4096 /*(4*1024) */,
	RADIANS = 8192 /*(8*1024) */,
	BARYCTR = 16384 /*(16*1024) */,
	TOPOCTR = 32768 /*(32*1024) */,
	SIDEREAL = 65536 /*(64*1024)*/,
	ICRS = 131072 /*(128*1024)*/
}

/** Position of a body, either long, lat, distance (degrees) or x, y, z (AU) depending upon coordinate system */
struct Position {
	1: double x,
	2: double y,
	3: double z
}

/** Velocity of a body, either deg long/day, deg lat/day, distance/day, or rx, ry, rz (AU/sec) depending upon coordinate system */
struct Velocity {
	1: double rx,
	2: double ry,
	3: double rz
}

struct State {
	1: Position pos,
	2: Velocity vel
}

struct HouseCusps {
	1: list<double> cusps,
	2: double ascendant,
	3: double mc,
	4: double armc,
	5: double vertex,
	6: double equatorialAscendant,
	7: double coAscendantKoch,
	8: double coAscendantMunkasey,
	9: double polarAscendant
}

exception SwissEphemerisError {
	1: string message
}

typedef list<i32> BodyIds

typedef list<string> StarNames

typedef list<State> States

service SwissGuardService {
	string getVersion() throws (1: SwissEphemerisError see)

	State calculateBodyState(1: double jdEt, 2: i32 bodyId, 3: Flags flags) throws (1: SwissEphemerisError see)

	States calculateBodiesState(1: double jdEt, 2: BodyIds bodyIds, 3: Flags flags) throws (1: SwissEphemerisError see)

	State calculateBodyStateUt(1: double jdUt, 2: i32 bodyId, 3: Flags flags) throws (1: SwissEphemerisError see)

	States calculateBodiesStateUt(1: double jdUt, 2: BodyIds bodyIds, 3: Flags flags) throws (1: SwissEphemerisError see)

	State calculateFixedStarState(1: double jdEt, 2: string starName, 3: Flags flags) throws (1: SwissEphemerisError see)

	States calculateFixedStarsState(1: double jdEt, 2: StarNames starNames, 3: Flags flags) throws (1: SwissEphemerisError see)

	State calculateFixedStarStateUt(1: double jdUt, 2: string starName, 3: Flags flags) throws (1: SwissEphemerisError see)

	States calculateFixedStarsStateUt(1: double jdUt, 2: StarNames starNames, 3: Flags flags) throws (1: SwissEphemerisError see)

	HouseCusps computeHouses(1: double jdUt, 2: double geographicLatitude, 3: double geographicLongitude, 4: string houseSystem) throws (1: SwissEphemerisError see)

	HouseCusps computeHousesArmc(1: double armc, 2: double geographicLatitude, 3: double eclipticObliquityDegrees, 4: string houseSystem) throws (1: SwissEphemerisError see)

	HouseCusps computeHousesEx(1: double jdUt, 2: Flags flags, 3: double geographicLatitude, 4: double geographicLongitude, 5: string houseSystem) throws (1: SwissEphemerisError see)
}