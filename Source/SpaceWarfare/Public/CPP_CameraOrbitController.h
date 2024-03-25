// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPP_CameraOrbitController.generated.h"

UENUM(BlueprintType)
enum class EPlayerStatus : uint8 
{
    WAITING = 0                 UMETA(DisplayName = "Waiting"),
    PLACING_GROUND_STATIONS = 1 UMETA(DisplayName = "PlacingGroundStations"),
    GROUND_STATION_CONTROL = 2  UMETA(DisplayName = "ControllingSatellites")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrencyUpdated, int, Currency);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAllPlayersReady);

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

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    EPlayerStatus PlayerStatus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasNecessaryReplicatedVariables;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bFinishedPlacingGroundStations;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    class UInputMappingContext* InputMapping;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    class UInputAction* SelectAction;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    class UInputAction* DragAction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* OrbitingActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    APawn* PlayerPawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UCPP_CameraOrbitableComponent* CameraOrbitableComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USpringArmComponent* SpringArmComponent;

    UPROPERTY(BlueprintAssignable)
    FCurrencyUpdated OnCurrencyUpdated;

    UPROPERTY(BlueprintAssignable)
    FAllPlayersReady OnAllPlayersFinishedPlacingGroundStations;

    UFUNCTION()
    void OnRep_Currency();

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerPlayerFinishedJoiningSession();

    UFUNCTION(BlueprintCallable, Server, Reliable)
    void ServerPlayerFinishedPlacingGroundStations(bool bFinished);

    UFUNCTION(BlueprintCallable, Client, Reliable)
    void ClientAllPlayersFinishedPlacingGroundStations();

    UFUNCTION(BlueprintCallable)
    void SpendCurrency(int Amount);
    
    UFUNCTION(BlueprintCallable)
    void MouseSelect(const FInputActionValue& Value);

    UFUNCTION(BlueprintCallable)
    void MouseDrag(const FInputActionValue& Value);

	// Sets default values for this actor's properties
	ACPP_CameraOrbitController();

    virtual void SetupInputComponent() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
