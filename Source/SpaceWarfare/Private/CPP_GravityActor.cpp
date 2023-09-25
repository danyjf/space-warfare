// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GravityActor.h"
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

}
