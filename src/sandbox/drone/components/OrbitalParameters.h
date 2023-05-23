#pragma once
#include <cmath>
#include <gmtl/gmtl.h>

struct OrbitalParameters
{
    // orbital parameters
    double eccentricity;
    double semimajor_axis;
    double incliniation;
    double longAN; ///> longitude of ascending node
    double longPA; ///> longitude of periapsis, basically the orientation of the ellipse
    /// time specific parameters
    double meanLongitude{}; /** Mean longitude is the ecliptic longitude at which an orbiting body could be found if its orbit were circular and free of perturbations. While nominally a simple longitude, in practice the mean longitude does not correspond to any one physical angle.**/
};

struct EulerCoordinates
{
    double phi;
    double theta;
    double psi;
    double radius;

    gmtl::Vec3d toCartesian() const
    {
        double x = radius * (cos(phi) * cos(theta) - sin(phi) * cos(psi) * sin(theta));
        double y = radius * (sin(phi) * cos(theta) * cos(psi) + cos(phi) * sin(theta));
        double z = radius * (sin(phi) * sin(psi));
        return gmtl::Vec3d{x, y, z};
    }
   
};


class OrbitalMechanics
{
private:
    static double getEccentricAnomaly(double mean_anomaly, double eccentricity)
    {
        double e         = 0.0;
        double lastDelta = 2.0;
        for (double delta = 1.0, e_ = mean_anomaly; delta < lastDelta;)
        {
            e         = eccentricity * std::sin(e_) + mean_anomaly;
            lastDelta = delta;
            delta     = std::abs(e_ - e);
            e_        = e;
        }
        return e;
    }
    /**/     
    static double getTrueAnomaly(double eccentricAnomaly, double eccentricity)
    {
        return 2.0 * std::atan2(                                                     //
                   std::sqrt(1.0 + eccentricity) * std::sin(eccentricAnomaly / 2.0), //
                   std::sqrt(1.0 - eccentricity) * std::cos(eccentricAnomaly / 2.0)  //
               );
    }

    static double getRadius(const OrbitalParameters& op, double eccentricAnomaly)
    {
        return op.semimajor_axis * (1.0 - op.eccentricity * std::cos(eccentricAnomaly));
    }

    static double getArgumentOfPeriapsis(const OrbitalParameters& op)
    {
        return op.longPA - op.longAN;
    }
    static double getMeanAnomaly(const OrbitalParameters& op)
    {
        return op.meanLongitude - op.longPA;
    };

public:
    static EulerCoordinates getEulerAnglesFromEccentricAnomaly(const OrbitalParameters& op, double eccentricAnomaly)
    {
        double trueAnomaly         = getTrueAnomaly(eccentricAnomaly, op.eccentricity);
        double argumentOfPeriapsis = getArgumentOfPeriapsis(op);

        double psi = trueAnomaly + argumentOfPeriapsis;

        double r = getRadius(op, eccentricAnomaly);

        return EulerCoordinates{
            psi,
            op.longAN,
            op.incliniation,
            r};
    }
    //TODO: this is a simplification over a orbit derived from an object "falling" around a central mass and having its speed derived from 
    static auto getEulerAngelsAtJulianDay(const OrbitalParameters& op, double day)
    {
        // orbital period T = 2PI*sqrt(a^3/GM), a == semi-major axis
        auto   siderealOrbitPeriod = 365.0;
        double meanMotion          = 2.0 * std::atan(1.0) *4.0 / siderealOrbitPeriod;
        double currentMeanAnomaly  = getMeanAnomaly(op) + meanMotion * day; //(day == jd - m_epoch;)

        double eccentricAnomaly = getEccentricAnomaly(currentMeanAnomaly, op.eccentricity);


        return getEulerAnglesFromEccentricAnomaly(op, eccentricAnomaly);
    }
    /**
     * Get the angle on the orbit at a given percentage of the orbital period 
     */

    static auto getEulerAngelsAtFraction(const OrbitalParameters& op, double period_fraction)
    {
        // orbital period T = 2PI*sqrt(a^3/GM), a == semi-major axis
        
        double meanMotion          = 2.0 * std::atan(1.0) * 4.0 ;
        double currentMeanAnomaly = getMeanAnomaly(op) + meanMotion * period_fraction;

        double eccentricAnomaly = getEccentricAnomaly(currentMeanAnomaly, op.eccentricity);


        return getEulerAnglesFromEccentricAnomaly(op, eccentricAnomaly);
    }
};
