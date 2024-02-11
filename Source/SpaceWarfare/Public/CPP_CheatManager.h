// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "CPP_CheatManager.generated.h"

/**
 * 
 */
UCLASS()
class SPACEWARFARE_API UCPP_CheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
    UFUNCTION(exec)
    void ServerView();

    UFUNCTION(exec)
    void ClientView();
};
