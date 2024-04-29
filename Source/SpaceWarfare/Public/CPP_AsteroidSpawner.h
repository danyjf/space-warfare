// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_AsteroidSpawner.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_AsteroidSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class ACPP_Asteroid> AsteroidBlueprintClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DelayBetweenAsteroidSpawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int MaxNumberOfAsteroids;

    int AsteroidCount;

    UFUNCTION(BlueprintCallable)
    void StartSpawning();

    UFUNCTION(BlueprintCallable)
    void SpawnAsteroidAtRandomOrbit();

	// Sets default values for this actor's properties
	ACPP_AsteroidSpawner();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;
    FTimerHandle SpawnAsteroidsTimerHandle;
};
