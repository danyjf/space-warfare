// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Universe.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_GroundStationBase.h"
#include "CPP_GroundStation.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_GroundStation : public ACPP_GroundStationBase
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadWrite)
    class ACPP_GroundStationManager* GroundStationManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int OwnerPlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionFieldOfView;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionHeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float DetectionVisualizationHeight;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    UStaticMeshComponent* DetectionCone;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    UStaticMeshComponent* DetectionConeVisualization;

    UFUNCTION()
    void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

    UFUNCTION()
    void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_GroundStation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;
};
