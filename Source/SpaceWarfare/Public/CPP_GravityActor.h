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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	/*
	* Object that holds the representation of this actor in the physics simulation
	*
	* This is necessary to get the correct values of the objects position in
	* the async physics tick
	*/
	FBodyInstanceAsyncPhysicsTickHandle RigidBody;
	FVector TotalForces;
	FVector Velocity;

	// Sets default values for this actor's properties
	ACPP_GravityActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(FString Name, double Mass, float Size, FVector Location, FVector InitialVelocity);

	void AddForce(FVector Force);
	//void ResetForces();
	//void UpdateVelocity(float DeltaTime);
	//void UpdateLocation(float DeltaTime);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Reference to the GameMode object
	ACPP_SimulationGameMode* SimulationGameMode;
};
