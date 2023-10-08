// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GravityActor.generated.h"


// Forward Declarations
class ACPP_SimulationGameMode;


UCLASS()
class SPACEWARFARE_API ACPP_GravityActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_GravityActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called at a fixed DeltaTime to update physics
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

	void SetMass(double Mass);
	void SetSize(float Size);
	void SetLocation(FVector Location);
	void SetInitialVelocity(FVector InitialVelocity);

private:
	// Reference to the GameMode object
	ACPP_SimulationGameMode* SimulationGameMode;
	
	/*
	* Object that holds the representation of this actor in the physics simulation
	* 
	* This is necessary to get the correct values of the objects position in
	* the async physics tick
	*/
	FBodyInstanceAsyncPhysicsTickHandle RigidBody;

	float GM;

	// Get the vector representing the force of gravity pointing from this object
	// to the Other
	FVector GetGravityForce(ACPP_GravityActor* Other);
};
