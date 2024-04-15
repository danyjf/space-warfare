// Fill out your copyright notice in the Description page of Project Settings.

#include "Universe.h"
#include "CPP_Planet.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"

FOrbitalState UUniverse::ConvertOrbitalElementsToOrbitalState(const FOrbitalElements& OrbitalElements, double GM)
{
	float e = OrbitalElements.Eccentricity;
	float a = OrbitalElements.SemiMajorAxis;
	float i = UKismetMathLibrary::DegreesToRadians(OrbitalElements.Inclination);
	float O = UKismetMathLibrary::DegreesToRadians(OrbitalElements.LongitudeOfAscendingNode);
	float w = UKismetMathLibrary::DegreesToRadians(OrbitalElements.ArgumentOfPeriapsis);
	float M = UKismetMathLibrary::DegreesToRadians(OrbitalElements.MeanAnomaly);

	// Solve Keplers Equation for the eccentric anomaly using Newton's method
    float E = 0.0f;
    float F = 0.0f;
	float Delta = 0.000001;
    if (e < 1.0f)
    {
	    E = M;
	    F = E - e * sin(E) - M;
	    for (int j = 0; j < 30; j++)
	    {
	    	E = E - F / (1 - e * cos(E));
	    	F = E - e * sin(E) - M;

	    	if (abs(F) < Delta)
	    	{
	    		break;
	    	}
	    }
    }
    else if (e > 1.0f)
    {
	    E = M;
	    F = e * sinh(E) - E - M;
	    for (int j = 0; j < 30; j++)
	    {
	    	E = E - F / (e * cosh(E) - 1);
	    	F = e * sinh(E) - E - M;

	    	if (abs(F) < Delta)
	    	{
	    		break;
	    	}
	    }
    }

	// Obtain the true anomaly
    float v = 0.0f;
    if (e < 1.0f)
    {
	    v = 2 * atan2(sqrt(1 + e) * sin(E / 2), sqrt(1 - e) * cos(E / 2));
    }
    else if (e > 1.0f)
    {
	    v = 2 * atan2(sqrt(e + 1) * sinh(E / 2), sqrt(e - 1) * cosh(E / 2));
    }

	// Use the eccentric anomaly to get the distance to the central body
	float rc = a * (1 - e * cos(E));

	// Obtain the position and velocity vectors in the orbital frame
	FVector OrbitalPosition(rc * cos(v), rc * sin(v), 0);
	FVector OrbitalVelocity(-sin(E), sqrt(1 - e * e) * cos(E), 0);
	OrbitalVelocity = (sqrt(GM * a) / rc) * OrbitalVelocity;

	// Transform the position and velocity vectors to the inertial frame
	FOrbitalState OrbitalState;

	OrbitalState.Location.X = OrbitalPosition.X * (cos(w) * cos(O) - sin(w) * cos(i) * sin(O)) -
		OrbitalPosition.Y * (sin(w) * cos(O) + cos(w) * cos(i) * sin(O));
	OrbitalState.Location.Y = OrbitalPosition.X * (cos(w) * sin(O) + sin(w) * cos(i) * cos(O)) +
		OrbitalPosition.Y * (cos(w) * cos(i) * cos(O) - sin(w) * sin(O));
	OrbitalState.Location.Z = OrbitalPosition.X * (sin(w) * sin(i)) + 
		OrbitalPosition.Y * (cos(w) * sin(i));

	OrbitalState.Velocity.X = OrbitalVelocity.X * (cos(w) * cos(O) - sin(w) * cos(i) * sin(O)) -
		OrbitalVelocity.Y * (sin(w) * cos(O) + cos(w) * cos(i) * sin(O));
	OrbitalState.Velocity.Y = OrbitalVelocity.X * (cos(w) * sin(O) + sin(w) * cos(i) * cos(O)) +
		OrbitalVelocity.Y * (cos(w) * cos(i) * cos(O) - sin(w) * sin(O));
	OrbitalState.Velocity.Z = OrbitalVelocity.X * (sin(w) * sin(i)) +
		OrbitalVelocity.Y * (cos(w) * sin(i));

    OrbitalState.Location = ToLeftHandSystem(OrbitalState.Location);
    OrbitalState.Velocity = ToLeftHandSystem(OrbitalState.Velocity);

	return OrbitalState;
}

FOrbitalElements UUniverse::ConvertOrbitalStateToOrbitalElements(const FOrbitalState& OrbitalState, double GM)
{
    FOrbitalElements OrbitalElements;

    FVector r = ToRightHandSystem(OrbitalState.Location);
    FVector v = ToRightHandSystem(OrbitalState.Velocity);

    FVector h = FVector::CrossProduct(r, v);                // Angular momentum
    FVector n = FVector::CrossProduct(FVector(0, 0, 1), h); // Node vector

    FVector ev = ((v.SizeSquared() - GM / r.Size()) * r - FVector::DotProduct(r, v) * v) / GM;  // Eccentricity vector

    float E = v.SizeSquared() / 2.0f - GM / r.Size();       // Specific orbital energy

    OrbitalElements.SemiMajorAxis = -GM / (2.0f * E);
    OrbitalElements.Eccentricity = ev.Size();

    // Inclination is the angle between the angular momentum vector and its z component
    OrbitalElements.Inclination = acos(h.Z / h.Size());

    if (FMath::IsNearlyZero(OrbitalElements.Inclination))
    {
        // For non-inclined orbits, the ascending node is undefined; set to zero by convention
        OrbitalElements.LongitudeOfAscendingNode = 0.0f;
        if (FMath::IsNearlyZero(OrbitalElements.Eccentricity))
        {
            // For circular orbits, place periapsis at ascending node by convention
            OrbitalElements.ArgumentOfPeriapsis = 0.0f;
        }
        else
        {
            // Argument of periapsis is the angle between eccentricity vector and its x component.
            OrbitalElements.ArgumentOfPeriapsis = acos(ev.X / ev.Size());
        }
    }
    else
    {
        // Right ascension of ascending node is the angle between the node vector and its x component.
        OrbitalElements.LongitudeOfAscendingNode = acos(n.X / n.Size());
        if (n.Y < 0.0f)
        {
            OrbitalElements.LongitudeOfAscendingNode = 2 * PI - OrbitalElements.LongitudeOfAscendingNode;
        }

        // Argument of periapsis is angle between node and eccentricity vectors.
        OrbitalElements.ArgumentOfPeriapsis = acos(FVector::DotProduct(n, ev) / (n.Size() * ev.Size()));
    }

    float f = 0.0f;
    if (FMath::IsNearlyZero(OrbitalElements.Eccentricity))
    {
        if (FMath::IsNearlyZero(OrbitalElements.Inclination))
        {
            // True anomaly is angle between position vector and its x component.
            f = acos(r.X / r.Size());
            if (v.X > 0.0f)
            {
                f = 2.0f * PI - f;
            }
        }
        else
        {
            // True anomaly is angle between node vector and position vector.
            f = acos(FVector::DotProduct(n, r) / (n.Size() * r.Size()));
            if (FVector::DotProduct(n, v) > 0.0f)
            {
                f = 2.0f * PI - f;
            }
        }
    }
    else
    {
        if (ev.Z < 0.0f)
        {
            OrbitalElements.ArgumentOfPeriapsis = 2.0f * PI - OrbitalElements.ArgumentOfPeriapsis;
        }

        // True anomaly is angle between eccentricity vector and position vector.
        f = acos(FVector::DotProduct(ev, r) / (ev.Size() * r.Size()));

        if (FVector::DotProduct(r, v) < 0.0f)
        {
            f = 2.0f * PI - f;
        }
    }

    OrbitalElements.MeanAnomaly = GetMeanAnomaly(OrbitalElements.Eccentricity, f);

    OrbitalElements.Inclination = UKismetMathLibrary::RadiansToDegrees(OrbitalElements.Inclination);
    OrbitalElements.LongitudeOfAscendingNode = UKismetMathLibrary::RadiansToDegrees(OrbitalElements.LongitudeOfAscendingNode);
    OrbitalElements.ArgumentOfPeriapsis = UKismetMathLibrary::RadiansToDegrees(OrbitalElements.ArgumentOfPeriapsis);
    OrbitalElements.MeanAnomaly = UKismetMathLibrary::RadiansToDegrees(OrbitalElements.MeanAnomaly);

    return OrbitalElements;
}

FGeographicCoordinates UUniverse::ConvertECILocationToGeographicCoordinates(ACPP_Planet* Planet, FVector Location)
{
	FGeographicCoordinates GeographicCoordinates;
	
    Location = ToRightHandSystem(Location);

    // Calculate latitude
	GeographicCoordinates.Latitude = atan(Location.Z / sqrt(pow(Location.X, 2) + pow(Location.Y, 2)));
	GeographicCoordinates.Latitude = UKismetMathLibrary::RadiansToDegrees(GeographicCoordinates.Latitude);
	
    // Calculate longitude
	float EarthRotationAngle = -UKismetMathLibrary::DegreesToRadians(Planet->GetActorRotation().Yaw);
	if (EarthRotationAngle < 0)
	{
		EarthRotationAngle += 2 * PI;
	}
	
	GeographicCoordinates.Longitude = atan2(Location.Y, Location.X) - EarthRotationAngle;
	GeographicCoordinates.Longitude = FRotator::NormalizeAxis(UKismetMathLibrary::RadiansToDegrees(GeographicCoordinates.Longitude));

    // Calculate altitude
	GeographicCoordinates.Altitude = Location.Length() - (Planet->GetActorScale().X / 2);

	return GeographicCoordinates;
}

FVector UUniverse::ConvertGeographicCoordinatesToECILocation(ACPP_Planet* Planet, const FGeographicCoordinates& GeographicCoordinates)
{
    FVector ECIPosition;

	float EarthRotationAngle = -UKismetMathLibrary::DegreesToRadians(Planet->GetActorRotation().Yaw);
	if (EarthRotationAngle < 0)
	{
		EarthRotationAngle += 2 * PI;
	}

    float Radius = Planet->GetActorScale().X / 2;
    float Theta = UKismetMathLibrary::DegreesToRadians(GeographicCoordinates.Longitude) + EarthRotationAngle;
    float R = (GeographicCoordinates.Altitude + Radius) * cos(UKismetMathLibrary::DegreesToRadians(GeographicCoordinates.Latitude));
    ECIPosition.X = R * cos(Theta);
    ECIPosition.Y = R * sin(Theta);
    ECIPosition.Z = (GeographicCoordinates.Altitude + Radius) * sin(UKismetMathLibrary::DegreesToRadians(GeographicCoordinates.Latitude));

    return ECIPosition;
}

double UUniverse::GetEarthRotationAngle(double JulianDay)
{
	// Is negative because of unreal's left handed system
	return -UKismetMathLibrary::RadiansToDegrees(2 * PI * (0.7790572732640 + 1.00273781191135448 * (JulianDay - 2451545.0)));
}

FVector UUniverse::ToRightHandSystem(const FVector& Vector)
{
    return FVector(Vector.X, -Vector.Y, Vector.Z);
}

FVector UUniverse::ToLeftHandSystem(const FVector& Vector)
{
    return FVector(Vector.X, -Vector.Y, Vector.Z);
}

float UUniverse::GetMeanAnomaly(float Eccentricity, float TrueAnomaly)
{
    return atan2(-sqrt(1 - pow(Eccentricity, 2.0f)) * sin(TrueAnomaly), -Eccentricity - cos(TrueAnomaly)) + PI - Eccentricity * (sqrt(1 - pow(Eccentricity, 2.0f)) * sin(TrueAnomaly)) / (1 + Eccentricity * cos(TrueAnomaly));
}

FString UUniverse::OrbitalElementsToString(const FOrbitalElements& OrbitalElements)
{
    return FString::Printf(TEXT("Eccentricity: %f; SemiMajorAxis: %f; Inclination: %f; LongitudeOfAscendingNode: %f; ArgumentOfPeriapsis: %f; MeanAnomaly: %f"), OrbitalElements.Eccentricity, OrbitalElements.SemiMajorAxis, OrbitalElements.Inclination, OrbitalElements.LongitudeOfAscendingNode, OrbitalElements.ArgumentOfPeriapsis, OrbitalElements.MeanAnomaly);
}

FString UUniverse::OrbitalStateToString(const FOrbitalState& OrbitalState)
{
    return FString::Printf(TEXT("Location: %s; Velocity: %s"), *OrbitalState.Location.ToString(), *OrbitalState.Velocity.ToString());
}
