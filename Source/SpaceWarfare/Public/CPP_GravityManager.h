// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GravityManager.generated.h"


class UCPP_GravityComponent;


UCLASS()
class SPACEWARFARE_API ACPP_GravityManager : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TimeScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<UCPP_GravityComponent*> GravityComponents;

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
    void CalculateGravityForces();
    void SemiImplicitEulerIntegrator(float DeltaTime);
    void LeapFrogIntegrator(float DeltaTime);
};