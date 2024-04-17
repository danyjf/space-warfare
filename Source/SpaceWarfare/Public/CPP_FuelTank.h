// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_FuelTank.generated.h"

UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEWARFARE_API UCPP_FuelTank : public UActorComponent
{
	GENERATED_BODY()

public:	
    /** Maximum amount of fuel for the tank in liters */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxFuel;

    UFUNCTION(BlueprintCallable)
    bool SpendFuel(float Value);

    /** Get current amount of fuel in the tank in liters */
    UFUNCTION(BlueprintCallable)
    float GetFuelLevel();

	// Sets default values for this component's properties
	UCPP_FuelTank();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
    /** Current amount of fuel in the tank in liters */
    float FuelLevel;
    class ACPP_MultiplayerGameMode* MultiplayerGameMode;
    class ACPP_Satellite* Satellite;
};
