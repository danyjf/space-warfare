// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPP_CameraOrbitController.generated.h"

UENUM(BlueprintType)
enum class EInputMode : uint8 
{
    GROUNDSTATIONINPUT = 0  UMETA(DisplayName = "GroundStationInput"),
    GODMODEINPUT = 1        UMETA(DisplayName = "GodModeInput")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrencyUpdated, int, Currency);

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_CameraOrbitController : public APlayerController
{
	GENERATED_BODY()
	
public:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    int PlayerID;

    UPROPERTY(ReplicatedUsing=OnRep_Currency, EditAnywhere, BlueprintReadWrite)
    int Currency;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool Ready;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* OrbitingActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    APawn* PlayerPawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UCPP_CameraOrbitableComponent* CameraOrbitableComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USpringArmComponent* SpringArmComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EInputMode InputMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ClickTimer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ClickThreshold;

    UPROPERTY(BlueprintAssignable)
    FCurrencyUpdated OnCurrencyUpdated;

    UFUNCTION()
    void OnRep_Currency();

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerPlayerReady();

    UFUNCTION(BlueprintCallable)
    void SpendCurrency(int Amount);

    UFUNCTION(BlueprintCallable)
    void HandleLeftMouseButtonPress();

    UFUNCTION(BlueprintCallable)
    void HandleLeftMouseButtonRelease();

	// Sets default values for this actor's properties
	ACPP_CameraOrbitController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
