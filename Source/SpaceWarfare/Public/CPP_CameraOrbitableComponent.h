// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_CameraOrbitableComponent.generated.h"


UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEWARFARE_API UCPP_CameraOrbitableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StartOrbitDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxOrbitDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinOrbitDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ZoomSpeed;

	// Sets default values for this component's properties
	UCPP_CameraOrbitableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
