// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HttpServerRequest.h"
#include "HttpResultCallback.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPP_HttpServer.generated.h"

UCLASS()
class SPACEWARFARE_API ACPP_HttpServer : public AActor
{
	GENERATED_BODY()
	
public:	
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	FString HttpPathGET = TEXT("/get-test");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	FString HttpPathPOST = TEXT("/post-test");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	FString HttpPathPUT = TEXT("/put-test");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Http")
	int32 ServerPort = 8080;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Sets default values for this actor's properties
	ACPP_HttpServer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void StartServer();
	void StopServer();

	// Callbacks for HttpRequests
	bool RequestGET(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool RequestPOST(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);
	bool RequestPUT(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete);

	// Just print request for example
	void RequestPrint(const FHttpServerRequest& Request, bool PrintBody = true);
};
