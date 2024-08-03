// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "S1GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class S1_API US1GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BluePrintCallable)
	void ConnectToGameServer();

	UFUNCTION(BluePrintCallable)
	void DisconnectFromGameServer();
	
public:
	// Game Server
	class FSocket* Socket;
	FString IpAddress = TEXT("127.0.0.1");
	int16 Port = 7777;
};
