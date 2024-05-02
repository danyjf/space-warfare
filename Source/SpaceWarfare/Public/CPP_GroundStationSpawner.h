// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationSpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGroundStationRepresentationUpdate);

UCLASS()
class SPACEWARFARE_API ACPP_GroundStationSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int OwnerPlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_Planet* Planet;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsChoosingLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACPP_GroundStation> GroundStationBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ACPP_GroundStationRepresentation> GroundStationRepresentationBlueprint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class ACPP_GroundStationRepresentation* GroundStationRepresentation;

    UPROPERTY(BlueprintAssignable)
    FGroundStationRepresentationUpdate OnUpdateGroundStationRepresentation;

    UFUNCTION()
    int GetGroundStationID() { return CurrentGroundStationNumber++; }

    UFUNCTION()
    void DestroyGroundStationRepresentation();

    UFUNCTION(BlueprintCallable)
    void SpawnGroundStationRepresentation(FVector Location);

    UFUNCTION(BlueprintCallable)
    void UpdateGroundStationRepresentationLocation(FVector Location);

    UFUNCTION(BlueprintCallable)
    void SpawnGroundStationAtRepresentationLocation();

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerSpawnGroundStation(FVector Location);

	// Sets default values for this actor's properties
	ACPP_GroundStationSpawner();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_PlayerController* CameraOrbitController;

    int CurrentGroundStationNumber;
};
