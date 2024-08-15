// Fill out your copyright notice in the Description page of Project Settings.
#include "PumiActor.h"
#include "PumiGenerateMesh.h"

// Sets default values
APumiActor::APumiActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FString smbfile;
	if (FParse::Value(FCommandLine::Get(), TEXT("smb"), smbfile, true)) {
		UE_LOG(HelloPumiLog, Warning, TEXT("smb=%s"), *smbfile);
		smbFile = smbfile;
	}

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	pumiGenerateMesh = NewObject<UPumiGenerateMesh>();
}

// Called when the game starts or when spawned
void APumiActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APumiActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APumiActor::PostActorCreated()
{
	Super::PostActorCreated();
	// Generate Mesh
	if (!smbFile.IsEmpty()) {
		pumiGenerateMesh->OnReadSmbProc(*smbFile);
	}

}


void APumiActor::PostLoad()
{

}