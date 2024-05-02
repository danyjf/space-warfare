// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CPP_PlayerController.generated.h"

UENUM(BlueprintType)
enum class EPlayerStatus : uint8 
{
    WAITING = 0                     UMETA(DisplayName = "Waiting"),
    PLACING_SATELLITE_LAUNCHER = 1  UMETA(DisplayName = "PlacingSatelliteLauncher"),
    PLACING_GROUND_STATIONS = 2     UMETA(DisplayName = "PlacingGroundStations"),
    GROUND_STATION_CONTROL = 3      UMETA(DisplayName = "ControllingSatellites")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCurrencyUpdated, int, Currency);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAllPlayersReady);

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    int PlayerID;

    UPROPERTY(EditAnywhere)
    float CameraRotationSpeed;

    UPROPERTY(ReplicatedUsing=OnRep_Currency, EditAnywhere, BlueprintReadWrite)
    int Currency;

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
    EPlayerStatus PlayerStatus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bMouseInputEnabled;

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

    UFUNCTION(BlueprintCallable)
    void SetOrbitingActor(AActor* ActorToOrbit);

    UFUNCTION(BlueprintCallable)
    AActor* GetOrbitingActor() {return OrbitingActor;}

	// Sets default values for this actor's properties
	ACPP_PlayerController();

    virtual void SetupInputComponent() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
    UPROPERTY()
    AActor* OrbitingActor;
};
