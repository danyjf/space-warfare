// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPP_GravityComponent.generated.h"


UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACEWARFARE_API UCPP_GravityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FBodyInstanceAsyncPhysicsTickHandle RigidBody;
    
    UPROPERTY(Replicated, BlueprintReadWrite)
    double GravitationalParameter;  // gravitational constant multiplied by mass

    UFUNCTION(BlueprintCallable)
    void AddGravityForce(const FVector& Force);

    UFUNCTION(BlueprintCallable)
    void ClearGravityForce();

    UFUNCTION(BlueprintCallable)
    double GetMass() { return Mass; }
    UFUNCTION(BlueprintCallable)
    void SetMass(double Value);

    UFUNCTION(BlueprintCallable)
    double GetGravitationalParameter() { return GravitationalParameter; }
    UFUNCTION(BlueprintCallable)
    void SetGravitationalParameter(double Value) { GravitationalParameter = Value; }

    UFUNCTION(BlueprintCallable)
    const FVector& GetGravityForce() const { return GravityForce; }

    UFUNCTION(BlueprintCallable)
    const FVector& GetVelocity() const { return Velocity; }
    UFUNCTION(BlueprintCallable)
    void SetVelocity(const FVector& Value) { Velocity = Value; }

	// Sets default values for this component's properties
	UCPP_GravityComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
    double Mass;                    // in kilograms
    FVector GravityForce;           // sum of gravitational forces exerted on this actor
    FVector Velocity;               // current velocity of this actor
};
