// Fill out your copyright notice in the Description page of Project Settings.


#include "Gravity.h"
#include "CPP_GravityActor.h"
#include "CPP_Planet.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Kismet/KismetMathLibrary.h"


FVector UGravity::CalculateGravityForce(ACPP_GravityActor* OnActor, ACPP_GravityActor* ByActor, double GravitationalConstant)
{
	// TODO: to avoid all the comparisons and casting, maybe calculate all 
	// forces with the planet first on a different function and then do the 
	// forces between everything else

	FVector MyLocation = OnActor->RigidBody->X();
	FVector OtherLocation = ByActor->RigidBody->X();
	float MyMass = OnActor->RigidBody->M();
	float OtherMass = ByActor->RigidBody->M();

	FVector Direction = OtherLocation - MyLocation;

	double Force;
	if (OnActor->ActorHasTag("Earth"))
	{
		ACPP_Planet* Planet = Cast<ACPP_Planet>(OnActor);
		Force = (Planet->GM * OtherMass) / Direction.SquaredLength();
	}
	else if (ByActor->ActorHasTag("Earth"))
	{
		ACPP_Planet* Planet = Cast<ACPP_Planet>(ByActor);
		Force = (Planet->GM * MyMass) / Direction.SquaredLength();
	}
	else
	{
		Force = (GravitationalConstant * MyMass * OtherMass) / Direction.SquaredLength();
	}

	Direction.Normalize();

	return Force * Direction;
}

void UGravity::SemiImplicitEulerIntegrator(ACPP_GravityActor* GravityActor, float DeltaTime)
{
	GravityActor->UpdateVelocity(DeltaTime);
	GravityActor->UpdatePosition(DeltaTime);
	GravityActor->ResetForces();
}

FOrbitalState UGravity::ConvertOrbitalElementsToOrbitalState(FOrbitalElements OrbitalElements, double GM)
{
	float e = OrbitalElements.Eccentricity;
	float a = OrbitalElements.SemiMajorAxis;
	float i = UKismetMathLibrary::DegreesToRadians(OrbitalElements.Inclination);
	float O = UKismetMathLibrary::DegreesToRadians(OrbitalElements.LongitudeOfAscendingNode);
	float w = UKismetMathLibrary::DegreesToRadians(OrbitalElements.ArgumentOfPeriapsis);
	float M = UKismetMathLibrary::DegreesToRadians(OrbitalElements.MeanAnomaly);

	// Solve Kepler�s Equation for the eccentric anomaly using Newton's method
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
