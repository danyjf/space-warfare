// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_CheatManager.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_GameHUD.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


void UCPP_CheatManager::InputModeGod()
{
    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    CameraOrbitController->InputMode = EInputMode::GODMODEINPUT;

    ACPP_GameHUD* GameHUD = Cast<ACPP_GameHUD>(CameraOrbitController->GetHUD());
    GameHUD->GameWidget->SetVisibility(ESlateVisibility::Hidden);
    GameHUD->GameWidget->SetIsEnabled(false);
}

void UCPP_CheatManager::InputModeGroundStation()
{
    ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    CameraOrbitController->InputMode = EInputMode::GROUNDSTATIONINPUT;

    ACPP_GameHUD* GameHUD = Cast<ACPP_GameHUD>(CameraOrbitController->GetHUD());
    GameHUD->GameWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    GameHUD->GameWidget->SetIsEnabled(true);
}
