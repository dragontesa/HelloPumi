// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PumiActor.generated.h"

UCLASS(Blueprintable, meta = (DisplayName = "Hello Pumi Actor"), ClassGroup = (Custom))
class HELLOPUMI_API APumiActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APumiActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hello Pumi Actor")
	FString smbFile;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	private:
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent * mesh;
	class UPumiGenerateMesh* pumiGenerateMesh;
};
