#ifndef config_h
#define config_h

#include <string>


// This variable isn't used outside this file, but makes specifying the other
// paths simpler.

// WINDOWS Path:
const std::string DATA_DIR = "C:\\Users\\Long Truong\\Documents\\Comp465\\universe-planets-tri-long\\data\\";
const std::string DATA_DIR_EQ = "C:\\Users\\Long Truong\\Documents\\Comp465\\universe-planets-tri-long\\data\\data\\";


// MAC path

// const std::string DATA_DIR = "/Users/buidinhtri/Desktop/COMP465/universe-planets-tri-long/data/";
// const std::string DATA_DIR_EQ = DATA_DIR;


// The file to load for the earth texture.
const std::string EARTH_PATH = DATA_DIR + "earth.jpg";
const std::string VENUS_PATH = DATA_DIR + "venus.jpg";
const std::string MERCURY_PATH = DATA_DIR + "mercury.jpg";
const std::string MOON_PATH = DATA_DIR + "moon.jpg";
const std::string MARS_PATH = DATA_DIR + "mars.jpg";
const std::string JUPITER_PATH = DATA_DIR + "jupiter.jpg";
const std::string SATURN_PATH = DATA_DIR + "saturn.jpg";
const std::string URANUS_PATH = DATA_DIR + "uranus.jpg";
const std::string NEPTUNE_PATH = DATA_DIR + "neptune.jpg";
const std::string PLUTO_PATH = DATA_DIR + "pluto.jpg";
const std::string SUN_PATH = DATA_DIR + "sun.jpg";

// The earthquake database file
const std::string DATA_PATH = DATA_DIR_EQ + "earthquakes.txt";


#endif
