// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GravityActor.h"
#include "CPP_SimulationGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" 

// Sets default values
ACPP_GravityActor::ACPP_GravityActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACPP_GravityActor::BeginPlay()
{
	Super::BeginPlay();

	SimulationGameMode = Cast<ACPP_SimulationGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (this->GetName() == "BP_Earth_C_0") 
	{
		GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), 5.972 * pow(10, 24));
	}
	else if (this->GetName() == "BP_Satellite_C_0")
	{
		GetComponentByClass<UStaticMeshComponent>()->SetMassOverrideInKg(FName(NAME_None), 2415);
	}
}

// Called every frame
void ACPP_GravityActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < SimulationGameMode->GravityActors.Num(); i++)
	{
		if (this == SimulationGameMode->GravityActors[i])
			continue;

		FVector Force = Gravity(SimulationGameMode->GravityActors[i]);

		GetComponentByClass<UStaticMeshComponent>()->AddForce(Force * 60);
	}
}

FVector ACPP_GravityActor::Gravity(ACPP_GravityActor* Other)
{
	FVector MyLocation = GetActorLocation();
	FVector OtherLocation = Other->GetActorLocation();
	float MyMass = GetComponentByClass<UStaticMeshComponent>()->GetMass();
	float OtherMass = Other->GetComponentByClass<UStaticMeshComponent>()->GetMass();

	FVector Direction = OtherLocation - MyLocation;

	double Force = (G * MyMass * OtherMass) / Direction.SquaredLength();
	UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(Force));

	Direction.Normalize();

	return Force * Direction;
}
