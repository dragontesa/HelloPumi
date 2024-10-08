// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "Math/MathFwd.h"
#include "PumiActor.generated.h"


UCLASS(Blueprintable, meta = (DisplayName = "Hello Pumi Actor"), ClassGroup = (Custom))
class HELLOPUMI_API APumiActor : public AActor
{
	using FDynamicMesh3 = UE::Geometry::FDynamicMesh3;
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	APumiActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hello Pumi Actor")
	FString smbFile;


    // callbacked from PumiGenerateMesh
	UFUNCTION()
    void OnPumiPointsDataLoaded(const TArray<FVector>& vertics, const TArray<int32>& indics);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

private:
    void GenerateProceduralMesh100(const TArray<FVector>& vertics, const TArray<int32>& indics);
	void GenerateProceduralMesh200(const TArray<FVector>& vertics, const TArray<int32>& indics);
	void GenerateProceduralMesh300(const TArray<FVector>& vertics, const TArray<int32>& indics);
	void GenerateProceduralMeshWithDynamicMesh(FDynamicMesh3& dynMesh);
	void UpdateMeshComponent();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProceduralMeshComponent> pmc;
	
	private:
	FDynamicMesh3 DynMesh3;
	class UPumiGenerateMesh* pumiGenerateMesh;
};
