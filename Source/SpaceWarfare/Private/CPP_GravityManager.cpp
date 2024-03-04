// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GravityManager.h"
#include "CPP_GravityComponent.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

// Sets default values
ACPP_GravityManager::ACPP_GravityManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ACPP_GravityManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACPP_GravityManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACPP_GravityManager::AsyncPhysicsTickActor(float DeltaTime, float SimTime)
{
	Super::AsyncPhysicsTickActor(DeltaTime, SimTime);

    //NativeEngineIntegrator(DeltaTime * TimeScale);
    //SemiImplicitEulerIntegrator(DeltaTime * TimeScale);
    LeapFrogIntegrator(DeltaTime * TimeScale);
}

void ACPP_GravityManager::CalculateGravityForces()
{
    for (int i = 0; i < GravityComponents.Num(); i++)
    {
        for (int j = i + 1; j < GravityComponents.Num(); j++)
        {
            double ForceMagnitude = 0.0;
            FVector Direction = GravityComponents[j]->RigidBody->X() - GravityComponents[i]->RigidBody->X();

            if (GravityComponents[i]->GetGravitationalParameter() > GravityComponents[j]->GetGravitationalParameter())
            {
                ForceMagnitude = GravityComponents[i]->GetGravitationalParameter() * GravityComponents[j]->GetMass() / Direction.SquaredLength();
            }
            else
            {
                ForceMagnitude = GravityComponents[j]->GetGravitationalParameter() * GravityComponents[i]->GetMass() / Direction.SquaredLength();
            }

            Direction.Normalize();
            FVector GravityForce = ForceMagnitude * Direction;

            GravityComponents[i]->AddGravityForce(GravityForce);
            GravityComponents[j]->AddGravityForce(-GravityForce);
        }
    }
}

void ACPP_GravityManager::SemiImplicitEulerIntegrator(float DeltaTime)
{
    CalculateGravityForces();

    for (UCPP_GravityComponent* GravityComponent : GravityComponents)
    {
	    // Update velocity
        FVector Acceleration = GravityComponent->GetGravityForce() / GravityComponent->GetMass();
        GravityComponent->SetVelocity(GravityComponent->GetVelocity() + Acceleration * DeltaTime);

	    // Update position
	    GravityComponent->RigidBody->SetX(GravityComponent->RigidBody->X() + GravityComponent->GetVelocity() * DeltaTime);
	
	    // Set forces back to zero
	    GravityComponent->ClearGravityForce();
    }
}

void ACPP_GravityManager::LeapFrogIntegrator(float DeltaTime)
{
    CalculateGravityForces();

    for (UCPP_GravityComponent* GravityComponent : GravityComponents)
    {
        //FVector Acceleration = GravityComponent->GetGravityForce() / GravityComponent->GetMass();
        FVector Acceleration = GravityComponent->RigidBody->Acceleration() + GravityComponent->GetGravityForce() / GravityComponent->GetMass();
        //GravityComponent->RigidBody->SetAcceleration(GravityComponent->RigidBody->Acceleration() + GravityComponent->GetGravityForce() / GravityComponent->GetMass());
        
        //GravityComponent->SetVelocity(GravityComponent->GetVelocity() + Acceleration * 0.5 * DeltaTime);
        //GravityComponent->RigidBody->SetV(GravityComponent->RigidBody->V() + GravityComponent->RigidBody->Acceleration() * 0.5 * DeltaTime); 
        GravityComponent->RigidBody->SetV(GravityComponent->RigidBody->V() + Acceleration * 0.5 * DeltaTime); 

	    //GravityComponent->RigidBody->SetX(GravityComponent->RigidBody->X() + GravityComponent->GetVelocity() * DeltaTime);
        GravityComponent->RigidBody->SetX(GravityComponent->RigidBody->X() + GravityComponent->RigidBody->V() * DeltaTime);

        GravityComponent->ClearGravityForce();
    }

    CalculateGravityForces();

    for (UCPP_GravityComponent* GravityComponent : GravityComponents)
    {
        //FVector Acceleration = GravityComponent->GetGravityForce() / GravityComponent->GetMass();
        GravityComponent->RigidBody->SetAcceleration(GravityComponent->RigidBody->Acceleration() + GravityComponent->GetGravityForce() / GravityComponent->GetMass());

        //GravityComponent->SetVelocity(GravityComponent->GetVelocity() + Acceleration * 0.5 * DeltaTime);
        GravityComponent->RigidBody->SetV(GravityComponent->RigidBody->V() + GravityComponent->RigidBody->Acceleration() * 0.5 * DeltaTime); 

        GravityComponent->ClearGravityForce();
    }
}

void ACPP_GravityManager::NativeEngineIntegrator(float DeltaTime)
{
    CalculateGravityForces();

    for (UCPP_GravityComponent* GravityComponent : GravityComponents)
    {
        //GravityComponent->RigidBody->AddForce(GravityComponent->GetGravityForce());

        GravityComponent->RigidBody->SetAcceleration(GravityComponent->RigidBody->Acceleration() + GravityComponent->GetGravityForce() / GravityComponent->RigidBody->M());
        //FVector Acceleration = GravityComponent->GetGravityForce() / GravityComponent->GetMass();
        GravityComponent->RigidBody->SetV(GravityComponent->RigidBody->V() + GravityComponent->RigidBody->Acceleration() * DeltaTime); 
        //GravityComponent->RigidBody->SetV(GravityComponent->RigidBody->V() + Acceleration * DeltaTime); 
        GravityComponent->RigidBody->SetX(GravityComponent->RigidBody->X() + GravityComponent->RigidBody->V() * DeltaTime);

        GravityComponent->ClearGravityForce();
    }
}
