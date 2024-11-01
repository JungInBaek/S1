// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PathFinder.h"
#include "FileIO.h"

// Sets default values
APathFinder::APathFinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FileIO = CreateDefaultSubobject<UFileIO>(TEXT("FileIO"));
}

// Called when the game starts or when spawned
void APathFinder::BeginPlay()
{
	Super::BeginPlay();

	GenerateNodes(GetWorld(), _GridSize);
	UE_LOG(LogTemp, Warning, TEXT("Debug Test"));
}

// Called every frame
void APathFinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TMap<FVector, int32> APathFinder::GenerateNodes(UWorld* World, float GridSize)
{
	TMap<FVector, int32> Nodes;

	// 레벨의 경계를 기준으로 노드를 생성
	for (float x = -LevelBoundary; x <= LevelBoundary; x += GridSize)
	{
		for (float y = -LevelBoundary; y <= LevelBoundary; y += GridSize)
		{
			FVector Location(x, y, 0);
			Nodes.Add(Location, 0);
			if (abs(Location.X) == LevelBoundary || abs(Location.Y) == LevelBoundary)
			{
				Nodes[Location] = 0;
			}
			else if (IsLocationNavigable(World, Location))
			{
				Nodes[Location] = 1;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Obstacle: %f, %f, %f"), Location.X, Location.Y, Location.Z);
				Nodes[Location] = -1;
			}
		}
	}

	NodeMap = Nodes;
	FileIO->OutputLevelInfo(NodeMap);

	return Nodes;
}

bool APathFinder::IsLocationNavigable(UWorld* World, FVector Location)
{
	// 레이캐스트 등을 사용하여 Location이 이동 가능한지 확인
	FHitResult Hit;
	FVector Start = Location + FVector(0, 0, 100);
	FVector End = Location - FVector(0, 0, 0);
	FCollisionQueryParams Params;

	bool Result = !World->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic, Params);

	if (Result)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 10.f);
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 10.f);
	}

	return Result;
}

TMap<FVector, TArray<FVector>> APathFinder::GenerateEdges(const TArray<FVector>& Nodes, float GridSize)
{
	TMap<FVector, TArray<FVector>> Edges;

	for (const FVector& Node : Nodes)
	{
		TArray<FVector> Neighbors;
		for (const FVector& Offset : NeighborOffsets)
		{
			FVector Neighbor = Node + Offset * GridSize;
			if (Nodes.Contains(Neighbor))
			{
				Neighbors.Add(Neighbor);
			}
		}
		Edges.Add(Node, Neighbors);
	}

	//EdgesMap = Edges;
	//FileIO->OutputLevelInfo(NodeMap);

	return Edges;
}
