// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_SimulationGameMode.h"

void ACPP_SimulationGameMode::ApplyGravity(TArray<AActor*> actors, TArray<float> masses, float gravitationalConstant, float deltaTime)
{
	for (int i = 0; i < actors.Num(); i++)
	{
		for (int j = 0; j < actors.Num(); j++)
		{
			if (i == j)
				continue;

			FVector force = GravitationalForce(actors[i]->GetActorLocation(), actors[j]->GetActorLocation(), masses[i], masses[j], gravitationalConstant);

			actors[i]->GetComponentByClass<UStaticMeshComponent>()->AddForce(force * deltaTime);
		}
	}
}

FVector ACPP_SimulationGameMode::GravitationalForce(FVector location1, FVector location2, float mass1, float mass2, float gravitationalConstant)
{
	FVector direction = location2 - location1;

	float force = gravitationalConstant * mass1 * mass2 / direction.SquaredLength();

	direction.Normalize();

	return force * direction;
}
