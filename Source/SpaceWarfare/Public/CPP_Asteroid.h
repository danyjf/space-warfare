// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Asteroid.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_Asteroid : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	class ACPP_Planet* OrbitingPlanet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_OrbitSpline> OrbitSplineBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int Currency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_OrbitSpline* OrbitSpline;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    UStaticMeshComponent* StaticMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    class UCPP_GravityComponent* GravityComponent;

	// Sets default values for this actor's properties
	ACPP_Asteroid();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;

    /** 
     * Variable used to replicate the state of the asteroid to the clients 
     * so that the orbit display can be updated
    */
    UPROPERTY(Replicated)
    FOrbitalState OrbitalState;
};
