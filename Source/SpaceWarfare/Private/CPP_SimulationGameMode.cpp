// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"
#include "CPP_GravityActor.h"
#include "Kismet/KismetSystemLibrary.h"

void ACPP_SimulationGameMode::ApplyGravity(TArray<AActor*> Actors)
{
	for (int i = 0; i < Actors.Num(); i++)
	{
		for (int j = 0; j < Actors.Num(); j++)
		{
			if (i == j)
				continue;

			FVector Force = GravitationalForce(
				Actors[i]->GetActorLocation(), 
				Actors[j]->GetActorLocation(), 
				Actors[i]->GetComponentByClass<UStaticMeshComponent>()->GetMass(),
				Actors[j]->GetComponentByClass<UStaticMeshComponent>()->GetMass()
			);

			Actors[i]->GetComponentByClass<UStaticMeshComponent>()->AddForce(Force);
		}
	}
}

FVector ACPP_SimulationGameMode::GravitationalForce(FVector Location1, FVector Location2, float Mass1, float Mass2)
{
	FVector Direction = Location2 - Location1;

	/*UKismetSystemLibrary::PrintString(
		this, 
		FString("G: ")
		.Append(FString::SanitizeFloat(GravitationalConstant))
		.Append(" m1: ")
		.Append(FString::SanitizeFloat(Mass1))
		.Append(" m2: ")
		.Append(FString::SanitizeFloat(Mass2))
		.Append(" r: ")
		.Append(FString::SanitizeFloat(Direction.Length()))
	);*/
	double Force = (GravitationalConstant * Mass1 * Mass2) / Direction.SquaredLength();
	UKismetSystemLibrary::PrintString(this, FString::SanitizeFloat(Force));

	Direction.Normalize();

	return Force * Direction;
}
