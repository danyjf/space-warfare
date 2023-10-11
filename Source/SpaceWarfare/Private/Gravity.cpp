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
