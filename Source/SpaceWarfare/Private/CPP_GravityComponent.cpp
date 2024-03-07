// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_GravityComponent.h"
#include "CPP_GravityManager.h"

#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UCPP_GravityComponent::UCPP_GravityComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UCPP_GravityComponent::BeginPlay()
{
	Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        StaticMeshComponent = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
	    RigidBody = StaticMeshComponent->GetBodyInstanceAsyncPhysicsTickHandle();

        GravityManager = Cast<ACPP_GravityManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACPP_GravityManager::StaticClass()));
        GravityManager->GravityComponents.Add(this);
    }
}

// Called every frame
void UCPP_GravityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCPP_GravityComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);

    if (GetOwner()->HasAuthority())
    {
        GravityManager->GravityComponents.Remove(this);
    }
}

void UCPP_GravityComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UCPP_GravityComponent, GravitationalParameter);
}

double UCPP_GravityComponent::GetMass()
{
    return StaticMeshComponent->GetMass();
}

void UCPP_GravityComponent::SetMass(double Value)
{
    if (!StaticMeshComponent)
    {
        StaticMeshComponent = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
    }
	StaticMeshComponent->SetMassOverrideInKg(FName(NAME_None), Value);
}

void UCPP_GravityComponent::SetVelocity(const FVector& Value)
{
    if (!StaticMeshComponent)
    {
        StaticMeshComponent = GetOwner()->GetComponentByClass<UStaticMeshComponent>();
    }
	StaticMeshComponent->SetPhysicsLinearVelocity(Value);
}

void UCPP_GravityComponent::AddGravityForce(const FVector& Force)
{
    GravityForce += Force;
}

void UCPP_GravityComponent::ClearGravityForce()
{
    GravityForce.Set(0.0, 0.0, 0.0);
}
