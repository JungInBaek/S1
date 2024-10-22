// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathFinder.generated.h"

UCLASS()
class S1_API APathFinder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathFinder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere)
	float LevelBoundary = 0.f;

	UPROPERTY(EditAnywhere)
	float _GridSize = 0.f;

	TArray<FVector> GenerateNodes(UWorld* World, float GridSize);
	bool IsLocationNavigable(UWorld* World, FVector Location);
	TMap<FVector, TArray<FVector>> GenerateEdges(const TArray<FVector>& Nodes, float GridSize);

	const TArray<FVector> NeighborOffsets =
	{
		FVector(1, 0, 0), FVector(-1, 0, 0),
		FVector(0, 1, 0), FVector(0, -1, 0)
	};

	TArray<FVector> NodeArr;
	TMap<FVector, TArray<FVector>> EdgesMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = LevelFileIO)
	class UFileIO* FileIO;
};
