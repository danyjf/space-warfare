// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_Asteroid.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_Asteroid : public AActor
{
	GENERATED_BODY()
	
public:	
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
};
