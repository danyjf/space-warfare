// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CPP_GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API ACPP_GameHUD : public AHUD
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UUserWidget* GameScreenWidget;
};
