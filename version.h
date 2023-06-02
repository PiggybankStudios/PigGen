/*
File:   version.h
Author: Taylor Robbins
Date:   05\09\2023
Description:
	** Defines the macros that contain the values for the current piggen version number
	** The build number is incremented automatically by a python script that runs before each build of the platform layer
*/

#ifndef _VERSION_H
#define _VERSION_H

#define PIGGEN_VERSION_MAJOR 0
#define PIGGEN_VERSION_MINOR 1

//NOTE: Auto-incremented by a python script before each build
#define PIGGEN_VERSION_BUILD 295

#endif //  _VERSION_H
