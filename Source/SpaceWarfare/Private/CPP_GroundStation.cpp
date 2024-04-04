// Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_GroundStation.h"
#include "CPP_GroundStationManager.h"
#include "CPP_MultiplayerGameMode.h"
#include "CPP_Satellite.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACPP_GroundStation::ACPP_GroundStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACPP_GroundStation::BeginPlay()
{
	Super::BeginPlay();

    if (HasAuthority())
    {
	    MultiplayerGameMode = Cast<ACPP_MultiplayerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

        // Find the ground station manager responsible for this ground station
        for(ACPP_GroundStationManager* Manager : MultiplayerGameMode->GetGroundStationManagers())
        {
            if (Manager->OwnerPlayerID == OwnerPlayerID)
            {
                GroundStationManager = Manager;
            }
        }

        DetectionCone->OnComponentBeginOverlap.AddDynamic(this, &ACPP_GroundStation::OnComponentBeginOverlap);
        DetectionCone->OnComponentEndOverlap.AddDynamic(this, &ACPP_GroundStation::OnComponentEndOverlap);

        // Hide enemy ground stations on listen server
        if (UGameplayStatics::GetPlayerController(GetWorld(), 0) != GetOwner())
        {
            SetActorHiddenInGame(true);
        }
    }
    else if (!HasAuthority())
    {
        SetActorHiddenInGame(false);
    }
}

// Called every frame
void ACPP_GroundStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_GroundStation::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
    if (HasAuthority())
    {
        ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(OtherActor);
        if (Satellite && GroundStationManager)
        {
            UKismetSystemLibrary::PrintString(GetWorld(), FString::FromInt(GroundStationManager->OwnerPlayerID));
            GroundStationManager->SatelliteEnteredOverpassArea(Satellite);
        }
    }
}

void ACPP_GroundStation::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (HasAuthority())
    {
        ACPP_Satellite* Satellite = Cast<ACPP_Satellite>(OtherActor);
        if (Satellite && GroundStationManager)
        {
            GroundStationManager->SatelliteExitedOverpassArea(Satellite);
        }
    }
}
