// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GravityManager.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_GravityManager : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<class UCPP_GravityComponent*> GravityComponents;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    double GravitationalConstant;

	// Sets default values for this actor's properties
	ACPP_GravityManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called at a fixed DeltaTime to update physics
	virtual void AsyncPhysicsTickActor(float DeltaTime, float SimTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_SimulationGameMode* SimulationGameMode;
    void CalculateGravityForces();
    void SemiImplicitEulerIntegrator(float DeltaTime);
    void LeapFrogIntegrator(float DeltaTime);
};
