// Fill out your copyright notice in the Description page of Project Settings.


#include "Universe.h"
#include "CPP_Planet.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Kismet/KismetMathLibrary.h"


FOrbitalState UUniverse::ConvertOrbitalElementsToOrbitalState(FOrbitalElements OrbitalElements, double GM)
{
	float e = OrbitalElements.Eccentricity;
	float a = OrbitalElements.SemiMajorAxis;
	float i = UKismetMathLibrary::DegreesToRadians(OrbitalElements.Inclination);
	float O = UKismetMathLibrary::DegreesToRadians(OrbitalElements.LongitudeOfAscendingNode);
	float w = UKismetMathLibrary::DegreesToRadians(OrbitalElements.ArgumentOfPeriapsis);
	float M = UKismetMathLibrary::DegreesToRadians(OrbitalElements.MeanAnomaly);

	// Solve Kepler’s Equation for the eccentric anomaly using Newton's method
	float E = M;
	float F = E - e * sin(E) - M;
	float Delta = 0.000001;
	for (int j = 0; j < 30; j++)
	{
		E = E - F / (1 - e * cos(E));
		F = E - e * sin(E) - M;

		if (abs(F) < Delta)
		{
			break;
		}
	}

	// Obtain the true anomaly
	float v = 2 * atan2(sqrt(1 + e) * sin(E / 2), sqrt(1 - e) * cos(E / 2));

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

	// Transform to left hand coordinate system
	OrbitalState.Location.Y *= -1;
	OrbitalState.Velocity.Y *= -1;

	return OrbitalState;
}

FGeographicCoordinates UUniverse::ConvertECILocationToGeographicCoordinates(ACPP_Planet* Planet, FVector Location)
{
	FGeographicCoordinates GeographicCoordinates;
	
	Location.Y = -Location.Y;	// transform location to right hand coordinate system

    // calculate latitude
	GeographicCoordinates.Latitude = atan(Location.Z / sqrt(pow(Location.X, 2) + pow(Location.Y, 2)));
	GeographicCoordinates.Latitude = UKismetMathLibrary::RadiansToDegrees(GeographicCoordinates.Latitude);
	
    // calculate longitude
	float EarthRotationAngle = -UKismetMathLibrary::DegreesToRadians(Planet->GetActorRotation().Yaw);
	if (EarthRotationAngle < 0)
	{
		EarthRotationAngle += 2 * PI;
	}
	
	GeographicCoordinates.Longitude = atan2(Location.Y, Location.X) - EarthRotationAngle;
	GeographicCoordinates.Longitude = FRotator::NormalizeAxis(UKismetMathLibrary::RadiansToDegrees(GeographicCoordinates.Longitude));

    // calculate altitude
	GeographicCoordinates.Altitude = Location.Length() - (Planet->GetActorScale().X / 2);

	return GeographicCoordinates;
}

FVector UUniverse::ConvertGeographicCoordinatesToECILocation(ACPP_Planet* Planet, const FGeographicCoordinates& GeographicCoordinates)
{
    FVector ECIPosition;

    //ECIPosition.Set(0, 0, Planet->GetActorScale().X / 2);
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
	// is negative because of unreal's left handed system
	return -UKismetMathLibrary::RadiansToDegrees(2 * PI * (0.7790572732640 + 1.00273781191135448 * (JulianDay - 2451545.0)));
}
