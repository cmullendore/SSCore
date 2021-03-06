// SSUtilities.cpp
// SSCore
//
// Created by Tim DeBenedictis on 3/23/20.
// Copyright © 2020 Southern Stars. All rights reserved.
//
// A few useful string manipulation, angle conversion,
// and other utility functions that are not present in standard C++11.

#ifndef SSUtilities_hpp
#define SSUtilities_hpp

#include <cmath>
#include <string>
#include <vector>

#ifndef max
#define max(x,y) (x>y?x:y)
#endif

#ifndef min
#define min(x,y) (x<y?x:y)
#endif

// on Android, hijack fopen and route it through the android asset system
// so that we can pull things out of our package's APK. From:
// http://www.50ply.com/blog/2013/01/19/loading-compressed-android-assets-with-file-pointer/

#ifdef ANDROID
FILE *android_fopen ( const char *fname, const char *mode );
#define fopen(name,mode) android_fopen(name,mode)
#endif

using namespace std;

#define M_2PI ( 2 * M_PI )

string getcwd ( void );
bool fgetline ( FILE *infile, string &line );

string trim ( string str );
string format ( const char *fmt, ... );
vector<string> split ( string str, string delim );
vector<string> tokenize ( string str, string delim );

int strtoint ( string str );
int64_t strtoint64 ( string str );
float strtofloat ( string str );
double strtofloat64 ( string str );

double strtodeg ( string str );
double degtorad ( double deg );
double radtodeg ( double rad );
double sindeg ( double deg );
double cosdeg ( double deg );
double tandeg ( double deg );
double asindeg ( double y );
double acosdeg ( double x );
double atandeg ( double x );
double atan2pi ( double y, double x );
double atan2pideg ( double y, double x );
double modpi ( double rad );
double mod2pi ( double rad );
double mod180 ( double deg );
double mod360 ( double deg );
double mod24h ( double h );

#endif /* SSUtilities_hpp */
