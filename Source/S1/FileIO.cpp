// Fill out your copyright notice in the Description page of Project Settings.


#include "FileIO.h"

// Sets default values for this component's properties
UFileIO::UFileIO()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFileIO::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UFileIO::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFileIO::OutputLevelInfo(const TMap<FVector, TArray<FVector>>& OutputEdgesMap)
{
	FString Info;
	for (auto Item : OutputEdgesMap)
	{
		const FVector& Key = Item.Key;
		Info += FString::Printf(TEXT("Key: (%.6f, %.6f, %.6f)\n"), Key.X, Key.Y, Key.Z);
		for (const FVector& Value : Item.Value)
		{
			Info += FString::Printf(TEXT("\tValue: (%.6f, %.6f, %.6f)\n"), Value.X, Value.Y, Value.Z);
		}
	}

	FString path = FPaths::GameDevelopersDir() + TEXT("/info");
	UE_LOG(LogTemp, Warning, TEXT("맵 정보 파일 출력!"));
	FFileHelper::SaveStringToFile(Info, *path);
}