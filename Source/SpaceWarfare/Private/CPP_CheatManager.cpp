// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_CheatManager.h"
#include "CPP_CameraOrbitController.h"
#include "CPP_GameHUD.h"
#include "CPP_Satellite.h"

#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"


void UCPP_CheatManager::ServerView()
{
    //ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    //CameraOrbitController->InputMode = EInputMode::GODMODEINPUT;

    //ACPP_GameHUD* GameHUD = Cast<ACPP_GameHUD>(CameraOrbitController->GetHUD());
    //GameHUD->GameWidget->SetVisibility(ESlateVisibility::Hidden);
    //GameHUD->GameWidget->SetIsEnabled(false);

    //TArray<AActor*> Satellites;
    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    //for (AActor* Satellite : Satellites)
    //{
    //    Satellite->SetActorHiddenInGame(false);
    //}
}

void UCPP_CheatManager::ClientView()
{
    //ACPP_CameraOrbitController* CameraOrbitController = Cast<ACPP_CameraOrbitController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    //CameraOrbitController->InputMode = EInputMode::GROUNDSTATIONINPUT;

    //ACPP_GameHUD* GameHUD = Cast<ACPP_GameHUD>(CameraOrbitController->GetHUD());
    //GameHUD->GameWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    //GameHUD->GameWidget->SetIsEnabled(true);

    //TArray<AActor*> Satellites;
    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPP_Satellite::StaticClass(), Satellites);
    //for (AActor* Satellite : Satellites)
    //{
    //    Satellite->SetActorHiddenInGame(true);
    //}
}
