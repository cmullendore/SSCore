//  SSStar.cpp
//  SSCore
//
//  Created by Tim DeBenedictis on 3/15/20.
//  Copyright © 2020 Southern Stars. All rights reserved.

#include <algorithm>
#include <map>

#include "SSDynamics.hpp"
#include "SSStar.hpp"

// Constructs single star with a specific object type code.
// All fields except type code are set to empty strings or infinity,
// signifying unknown/undefined values.

SSStar::SSStar ( SSObjectType type ) : SSObject ( type )
{
    _names = vector<string> ( 0 );
    _idents = vector<SSIdentifier> ( 0 );

    _parallax = 0.0;
    _radvel = HUGE_VAL;
    _position = _velocity = SSVector ( HUGE_VAL, HUGE_VAL, HUGE_VAL );
    _Vmag = HUGE_VAL;
    _Bmag = HUGE_VAL;
    
    _spectrum = "";
}

// Constructs single star with type code set to indicate "single star".
// All other fields set to values, signifying unknown/undefined.

SSStar::SSStar ( void ) : SSStar ( kTypeStar )
{

}

// Constructs variable star with all fields except type code
// set to empty strings or infinity, signifying "unknown".

SSVariableStar::SSVariableStar ( void ) : SSStar ( kTypeVariableStar )
{
    _varType = "";
    _varMaxMag = HUGE_VAL;
    _varMinMag = HUGE_VAL;
    _varPeriod = HUGE_VAL;
    _varEpoch = HUGE_VAL;
}

// Constructs double star with all fields except type code
// set to empty strings or infinity, signifying "unknown".

SSDoubleStar::SSDoubleStar ( void ) : SSStar ( kTypeDoubleStar )
{
    _comps = "";
    _magDelta = HUGE_VAL;
    _sep = HUGE_VAL;
    _PA = HUGE_VAL;
    _PAyr = HUGE_VAL;
}

// Constructs double variable star with all fields except type code
// set to empty strings or infinity, signifying "unknown".

SSDoubleVariableStar::SSDoubleVariableStar ( void ) : SSDoubleStar(), SSVariableStar()
{
    _type = kTypeDoubleVariableStar;
}

// Constructs deep sky object with the specified type code;
// all other fields are set to unknown/undefined values.

SSDeepSky::SSDeepSky ( SSObjectType type ) : SSStar ( type )
{
    _majAxis = HUGE_VAL;
    _minAxis = HUGE_VAL;
    _PA = HUGE_VAL;
}

// Returns this star's identifier in a specific catalog.
// If not present, returns null identifier (i.e. zero).

SSIdentifier SSStar::getIdentifier ( SSCatalog cat )
{
    for ( int i = 0; i < _idents.size(); i++ )
        if ( _idents[i].catalog() == cat )
            return _idents[i];
    
    return SSIdentifier();
}

bool SSStar::addIdentifier ( SSIdentifier ident )
{
    return SSAddIdentifier ( ident, _idents );
}

void SSStar::sortIdentifiers ( void )
{
    sort ( _idents.begin(), _idents.end(), compareSSIdentifiers );
}

void SSStar::computeEphemeris ( SSDynamics &dyn )
{
    if ( _parallax > 0.0 )
    {
        _direction = _position + _velocity * ( dyn.jde - SSTime::kJ2000 );
        _distance = _direction.magnitude();
        _direction /= _distance;
        _magnitude = _Vmag + 5.0 * log10 ( _distance * _parallax );
    }
    else
    {
        _direction = _position;
        _distance = HUGE_VAL;
        _magnitude = _Vmag;
    }
}

// Sets this star's spherical coordinates in the fundamental frame,
// i.e. the star's mean equatorial J2000 coordinates at epoch 2000.
// The star's RA (coords.lon) and Dec (coords.lat) are in radians.
// The star's distance in light years (coords.rad) may be infinite if unknown.

void SSStar::setFundamentalCoords ( SSSpherical coords )
{
    _parallax = isinf ( coords.rad ) ? 0.0 : SSDynamics::kLYPerParsec / coords.rad;

    if ( _parallax <= 0.0 || isinf ( coords.rad ) )
        coords.rad = 1.0;
    
    _position = coords.toVectorPosition();
}

// Sets this star's spherical coordinates and proper motion in the fundamental frame
// i.e. the star's mean equatorial J2000 coordinates and proper motion at epoch 2000.
// The star's RA (coords.lon) and Dec (coords.lat) are in radians.
// The stars proper motion in RA (motion.ra) and dec (motion.dec) are in radians per Julian year.
// The star's distance in light years (coords.rad) may be infinite if unknown.
// The star's radial velocity in light years per year (motion.rad) may be infinite if unknown.
// Mathematically, both coordinates and motion are required to compute the star's rectangular
// heliocentric position and motion; practically, if you have its motion you'll also have its position,
// so we pass them both here.  You can extract them separately (see below).

void SSStar::setFundamentalMotion ( SSSpherical coords, SSSpherical motion )
{
    _parallax = isinf ( coords.rad ) ? 0.0 : SSDynamics::kLYPerParsec / coords.rad;
    _radvel = motion.rad;

    if ( _parallax <= 0.0 )
    {
        coords.rad = 1.0;
        motion.rad = 0.0;
    }
    
    if ( isinf ( motion.rad ) )
        motion.rad = 0.0;
    
    _position = coords.toVectorPosition();
    _velocity = coords.toVectorVelocity ( motion );
}

// Returns this star's heliocentric spherical coordinates in the fundamental
// J2000 mean equatorial frame at epoch J2000.  The star's RA (coords.lon)
// and Dec (coords.lat) are in radians.  Its distance in light years (coords.rad)
// will be infinite if unknown.

SSSpherical SSStar::getFundamentalCoords ( void )
{
    SSSpherical coords = _position.toSpherical();
    coords.rad = ( isinf ( _parallax ) || _parallax == 0.0 ) ? HUGE_VAL : SSDynamics::kLYPerParsec / _parallax;
    return coords;
}

// Returns this star's heliocentric proper motion in the fundamental J2000
// mean equatorial frame at epoch J2000.  The proper motion in RA (motion.lon)
// and Dec (motion.lat) are both in radians per year.  Its radial velocity
// (motion.rad) is in light years per year and will be infinite if unknown.

SSSpherical SSStar::getFundamentalMotion ( void )
{
    SSSpherical motion = _position.toSphericalVelocity ( _velocity );
    motion.rad = _radvel;
    return motion;
}

// Returns CSV string from base data (excluding names and identifiers).

string SSStar::toCSV1 ( void )
{
    SSSpherical coords = getFundamentalCoords();
    SSSpherical motion = getFundamentalMotion();
    
    SSHourMinSec ra = coords.lon;
    SSDegMinSec dec = coords.lat;
    double distance = coords.rad;
    
    string csv = SSObject::typeToCode ( _type ) + ",";
    
    csv += ra.toString() + ",";
    csv += dec.toString() + ",";
    
    csv += isnan ( motion.lon ) ? "," : format ( "%+.5f,", ( motion.lon / 15.0 ).toArcsec() );
    csv += isnan ( motion.lat ) ? "," : format ( "%+.4f,", motion.lat.toArcsec() );
    
    csv += isinf ( _Vmag ) ? "," : format ( "%+.2f,", _Vmag );
    csv += isinf ( _Bmag ) ? "," : format ( "%+.2f,", _Bmag );
    
    csv += isinf ( distance ) ? "," : format ( "%.3E,", distance * SSDynamics::kParsecPerLY );
    csv += isinf ( _radvel ) ? "," : format ( "%+.1f,", _radvel * SSDynamics::kLightKmPerSec );
    
    // If spectrum contains a comma, put it in quotes.
    
    csv += _spectrum.find ( "," ) == string::npos ? _spectrum + "," : "\"" + _spectrum + "\",";
    
    return csv;
}

// Returns CSV string from identifiers and names (excluding base data).

string SSStar::toCSV2 ( void )
{
    string csv = "";
    
    for ( int i = 0; i < _idents.size(); i++ )
        csv += _idents[i].toString() + ",";
    
    for ( int i = 0; i < _names.size(); i++ )
        csv += _names[i] + ",";

    return csv;
}

// Returns CSV string including base star data plus names and identifiers.

string SSStar::toCSV ( void )
{
    return toCSV1() + toCSV2();
}

// Returns CSV string from double-star data (but not SStar base class).

string SSDoubleStar::toCSVD ( void )
{
    string csv = "";
    
    csv += _comps + ",";
    csv += isinf ( _magDelta ) ? "," : format ( "%+.2f,", _magDelta );
    csv += isinf ( _sep ) ? "," : format ( "%.1f,", _sep * SSAngle::kArcsecPerRad );
    csv += isinf ( _PA ) ? "," : format ( "%.1f,", _PA * SSAngle::kDegPerRad );
    csv += isinf ( _PAyr ) ? "," : format ( "%.2f,", _PAyr );

    return csv;
}

// Returns CSV string including base star data, double-star data,
// plus names and identifiers. Overrides SSStar::toCSV().

string SSDoubleStar::toCSV ( void )
{
    return toCSV1() + toCSVD() + toCSV2();
}

// Returns CSV string from variable-star data (but not SStar base class).

string SSVariableStar::toCSVV ( void )
{
    string csv = "";
    
    csv += _varType + ",";
    csv += isinf ( _varMinMag ) ? "," : format ( "%+.2f,", _varMinMag );
    csv += isinf ( _varMaxMag ) ? "," : format ( "%+.2f,", _varMaxMag );
    csv += isinf ( _varPeriod ) ? "," : format ( "%.2f,", _varPeriod );
    csv += isinf ( _varEpoch )  ? "," : format ( "%.2f,", _varEpoch );

    return csv;
}

// Returns CSV string including base star data, variable-star data, plus names and identifiers.
// Overrides SSStar::toCSV().

string SSVariableStar::toCSV ( void )
{
    return toCSV1() + toCSVV() + toCSV2();
}

// Returns CSV string including base star data, double-star data, variable-star data,
// plus names and identifiers.  Overrides SSStar::toCSV().

string SSDoubleVariableStar::toCSV ( void )
{
    return toCSV1() + toCSVD() + toCSVV() + toCSV2();
}

// Returns CSV string from deep sky object data (but not SStar base class).

string SSDeepSky::toCSVDS ( void )
{
    string csv = "";

    csv += isinf ( _majAxis ) ? "," : format ( "%.2f,", _majAxis * SSAngle::kArcminPerRad );
    csv += isinf ( _minAxis ) ? "," : format ( "%.2f,", _minAxis * SSAngle::kArcminPerRad );
    csv += isinf ( _PA ) ? "," : format ( "%.1f,", _PA * SSAngle::kDegPerRad );

    return csv;
}

// Returns CSV string including base star data, double-star data,
// plus names and identifiers. Overrides SSStar::toCSV().

string SSDeepSky::toCSV ( void )
{
    return toCSV1() + toCSVDS() + toCSV2();
}

// Allocates a new SSStar and initializes it from a CSV-formatted string.
// Returns nullptr on error (invalid CSV string, heap allocation failure, etc.)

SSObjectPtr SSStar::fromCSV ( string csv )
{
    // split string into comma-delimited fields,
    // remove leading & trailing whitespace from each field.
    
    vector<string> fields = split ( csv, "," );
    for ( int i = 0; i < fields.size(); i++ )
        fields[i] = trim ( fields[i] );
    
    SSObjectType type = SSObject::codeToType ( fields[0] );
    if ( type < kTypeStar || type > kTypeGalaxy )
        return nullptr;
    
    // Set expected field index for first identifier based on object type.
    // Verify that we have the required number if fiels and return if not.
    
    int fid = 0;
    if ( type == kTypeStar )
        fid = 10;
    else if ( type == kTypeDoubleStar )
        fid = 15;
    else if ( type == kTypeVariableStar )
        fid = 15;
    else if ( type == kTypeDoubleVariableStar )
        fid = 20;
    else
        fid = 13;
    
    if ( fields.size() < fid )
        return nullptr;
    
    SSHourMinSec ra ( fields[1] );
    SSDegMinSec dec ( fields[2] );
    
    double pmRA = fields[3].empty() ? HUGE_VAL : SSAngle::kRadPerArcsec * strtofloat64 ( fields[3] ) * 15.0;
    double pmDec = fields[4].empty() ? HUGE_VAL : SSAngle::kRadPerArcsec * strtofloat64 ( fields[4] );
    
    float vmag = fields[5].empty() ? HUGE_VAL : strtofloat ( fields[5] );
    float bmag = fields[6].empty() ? HUGE_VAL : strtofloat ( fields[6] );
    
    float dist = fields[7].empty() ? HUGE_VAL : strtofloat ( fields[7] ) * SSDynamics::kLYPerParsec;
    float radvel = fields[8].empty() ? HUGE_VAL : strtofloat ( fields[8] ) / SSDynamics::kLightKmPerSec;
    string spec = trim ( fields[9] );
    
    // For remaining fields, attempt to parse an identifier.
    // If we succeed, add it to the identifier vector; otherwise add it to the name vector.
    
    vector<string> names;
    vector<SSIdentifier> idents;
    
    for ( int i = fid; i < fields.size(); i++ )
    {
        if ( fields[i].empty() )
            continue;
        
        SSIdentifier ident = SSIdentifier::fromString ( fields[i] );
        if ( ident )
            idents.push_back ( ident );
        else
            names.push_back ( fields[i] );
    }
    
    SSObjectPtr pObject = SSNewObject ( type );
    SSStarPtr pStar = SSGetStarPtr ( pObject );
    SSDoubleStarPtr pDoubleStar = SSGetDoubleStarPtr ( pObject );
    SSVariableStarPtr pVariableStar = SSGetVariableStarPtr ( pObject );
    SSDeepSkyPtr pDeepSkyObject = SSGetDeepSkyPtr ( pObject );

    if ( pStar == nullptr )
        return nullptr;

    SSSpherical coords ( ra, dec, dist );
    SSSpherical motion ( pmRA, pmDec, radvel );
    
    pStar->setFundamentalMotion ( coords, motion );
    pStar->setVMagnitude ( vmag );
    pStar->setBMagnitude ( bmag );
    pStar->setSpectralType ( spec );
    pStar->setIdentifiers( idents );
    pStar->setNames ( names );
    
    if ( pDoubleStar )
    {
        string comps = fields[10];
        float dmag = fields[11].empty() ? HUGE_VAL : strtofloat ( fields[11] );
        float sep = fields[12].empty() ? HUGE_VAL : strtofloat ( fields[12] ) / SSAngle::kArcsecPerRad;
        float pa = fields[13].empty() ? HUGE_VAL : strtofloat ( fields[13] ) / SSAngle::kDegPerRad;
        float year = fields[14].empty() ? HUGE_VAL : strtofloat ( fields[14] );

        pDoubleStar->setComponents ( comps );
        pDoubleStar->setMagnitudeDelta( dmag );
        pDoubleStar->setSeparation ( sep );
        pDoubleStar->setPositionAngle ( pa );
        pDoubleStar->setPositionAngleYear ( year );
    }

    if ( pVariableStar )
    {
        int fv = ( type == kTypeVariableStar ) ? 10 : 15;
            
        string vtype = fields[fv];
        float vmin = fields[fv+1].empty() ? HUGE_VAL : strtofloat ( fields[fv+1] );
        float vmax = fields[fv+2].empty() ? HUGE_VAL : strtofloat ( fields[fv+2] );
        float vper = fields[fv+3].empty() ? HUGE_VAL : strtofloat ( fields[fv+3] );
        double vep = fields[fv+4].empty() ? HUGE_VAL : strtofloat64 ( fields[fv+4] );
        
        pVariableStar->setVariableType ( vtype );
        pVariableStar->setMaximumMagnitude ( vmax );
        pVariableStar->setMinimumMagnitude ( vmin );
        pVariableStar->setPeriod ( vper );
        pVariableStar->setEpoch ( vep );
    }
    
    if ( pDeepSkyObject )
    {
        float major = fields[10].empty() ? HUGE_VAL : strtofloat ( fields[10] ) / SSAngle::kArcminPerRad;
        float minor = fields[11].empty() ? HUGE_VAL : strtofloat ( fields[11] ) / SSAngle::kArcminPerRad;
        float pa = fields[12].empty() ? HUGE_VAL : strtofloat ( fields[12] ) / SSAngle::kDegPerRad;
        
        pDeepSkyObject->setMajorAxis ( major );
        pDeepSkyObject->setMinorAxis ( minor );
        pDeepSkyObject->setPositionAngle ( pa );
    }
    
    return ( pObject );
}

// Downcasts generic SSObject pointer to SSStar pointer.
// Returns nullptr if pointer is not an instance of SSStar!

SSStarPtr SSGetStarPtr ( SSObjectPtr ptr )
{
    return dynamic_cast<SSStarPtr> ( ptr.get() );
}

// Downcasts generic SSObject pointer to SSDoubleStar pointer.
// Returns nullptr if pointer is not an instance of SSDoubleStar
// or SSDoubleVariableStar!

SSDoubleStarPtr SSGetDoubleStarPtr ( SSObjectPtr ptr )
{
    return dynamic_cast<SSDoubleStarPtr> ( ptr.get() );
}

// Downcasts generic SSObject pointer to SSVariableStar pointer.
// Returns nullptr if pointer is not an instance of SSVariableStar
// or SSDoubleVariableStar!

SSVariableStarPtr SSGetVariableStarPtr ( SSObjectPtr ptr )
{
    return dynamic_cast<SSVariableStarPtr> ( ptr.get() );
}

// Downcasts generic SSObject pointer to SSDeepSkyStar pointer.
// Returns nullptr if pointer is not an instance of SSDeepSky!

SSDeepSkyPtr SSGetDeepSkyPtr ( SSObjectPtr ptr )
{
    return dynamic_cast<SSDeepSkyPtr> ( ptr.get() );
}
