// Fill out your copyright notice in the Description page of Project Settings.


#include "Gravity.h"
#include "CPP_GravityActor.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"


FVector UGravity::CalculateGravityForce(ACPP_GravityActor* OnActor, ACPP_GravityActor* ByActor, double GravitationalConstant)
{
	FVector MyLocation = OnActor->RigidBody->X();
	FVector OtherLocation = ByActor->RigidBody->X();
	float MyMass = OnActor->RigidBody->M();
	float OtherMass = ByActor->RigidBody->M();

	FVector Direction = OtherLocation - MyLocation;

	double Force = (GravitationalConstant * MyMass * OtherMass) / Direction.SquaredLength();

	Direction.Normalize();

	return Force * Direction;
}

void UGravity::SemiImplicitEulerIntegrator(ACPP_GravityActor* GravityActor, float DeltaTime)
{
	GravityActor->UpdateVelocity(DeltaTime);
	GravityActor->UpdatePosition(DeltaTime);
	GravityActor->ResetForces();
}

FOrbitalState UGravity::ConvertOrbitalElementsToOrbitalState(FOrbitalElements OE, double GM)
{
	// Solve Kepler’s Equation for the eccentric anomaly using Newton's method
	float E = OE.M;
	float F = E - OE.e * sin(E) - OE.M;
	float Delta = 0.000001;
	for (int i = 0; i < 30; i++)
	{
		E = E - F / (1 - OE.e * cos(E));
		F = E - OE.e * sin(E) - OE.M;

		if (abs(F) < Delta)
		{
			break;
		}
	}

	// Obtain the true anomaly
	float v = 2 * atan2(sqrt(1 + OE.e) * sin(E / 2), sqrt(1 - OE.e) * cos(E / 2));

	// Use the eccentric anomaly to get the distance to the central body
	float rc = OE.a * (1 - OE.e * cos(E));
	UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), rc);

	// Obtain the position and velocity vectors in the orbital frame
	FVector OrbitalPosition(rc * cos(v), rc * sin(v), 0);
	FVector OrbitalVelocity(-sin(E), sqrt(1 - OE.e * OE.e) * cos(E), 0);
	OrbitalVelocity = (sqrt(GM * OE.a) / rc) * OrbitalVelocity;
	UE_LOG(LogTemp, Warning, TEXT("Orbital Location: %s"), *OrbitalPosition.ToString());

	// Transform the position and velocity vectors to the inertial frame
	FOrbitalState OrbitalState;

	OrbitalState.Location.X = OrbitalPosition.X * (cos(OE.w) * cos(OE.O) - sin(OE.w) * cos(OE.i) * sin(OE.O)) -
		OrbitalPosition.Y * (sin(OE.w) * cos(OE.O) + cos(OE.w) * cos(OE.i) * sin(OE.O));
	OrbitalState.Location.Y = OrbitalPosition.X * (cos(OE.w) * sin(OE.O) + sin(OE.w) * cos(OE.i) * cos(OE.O)) +
		OrbitalPosition.Y * (cos(OE.w) * cos(OE.i) * cos(OE.O) - sin(OE.w) * sin(OE.O));
	OrbitalState.Location.Z = OrbitalPosition.X * (sin(OE.w) * sin(OE.i)) + 
		OrbitalPosition.Y * (cos(OE.w) * sin(OE.i));

	OrbitalState.Velocity.X = OrbitalVelocity.X * (cos(OE.w) * cos(OE.O) - sin(OE.w) * cos(OE.i) * sin(OE.O)) -
		OrbitalVelocity.Y * (sin(OE.w) * cos(OE.O) + cos(OE.w) * cos(OE.i) * sin(OE.O));
	OrbitalState.Velocity.Y = OrbitalVelocity.X * (cos(OE.w) * sin(OE.O) + sin(OE.w) * cos(OE.i) * cos(OE.O)) +
		OrbitalVelocity.Y * (cos(OE.w) * cos(OE.i) * cos(OE.O) - sin(OE.w) * sin(OE.O));
	OrbitalState.Velocity.Z = OrbitalVelocity.X * (sin(OE.w) * sin(OE.i)) +
		OrbitalVelocity.Y * (cos(OE.w) * sin(OE.i));

	return OrbitalState;
}
