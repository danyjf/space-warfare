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
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    float StartOrbitDistance;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    float MaxOrbitDistance;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    float MinOrbitDistance;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    float ZoomSpeed;

	// Sets default values for this component's properties
	UCPP_CameraOrbitableComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
