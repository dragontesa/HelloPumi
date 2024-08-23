// Fill out your copyright notice in the Description page of Project Settings.
#include "PumiActor.h"
#include "PumiGenerateMesh.h"

// Sets default values
APumiActor::APumiActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#if UE_BUILD_SHIPPING
    smbFile = FPaths::ProjectDir() + "../data/cube0.smb";
#else
    smbFile = FPaths::ProjectDir() + "data/cube0.smb";
#endif	

	FString smbfile;
	if (FParse::Value(FCommandLine::Get(), TEXT("smb"), smbfile, true)) {
		UE_LOG(HelloPumiLog, Warning, TEXT("smb=%s"), *smbfile);
		smbFile = smbfile;
	}

	mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	// New in UE 4.17, multi-threaded PhysX cooking.
	mesh->bUseAsyncCooking = true;
	pumiGenerateMesh = NewObject<UPumiGenerateMesh>();
	pumiGenerateMesh->DelPointsDataLoadFinished.BindDynamic(this, &APumiActor::OnPumiPointsDataLoaded);
}

// Called when the game starts or when spawned
void APumiActor::BeginPlay()
{
	Super::BeginPlay();
	 if (!smbFile.IsEmpty()) {
	 	pumiGenerateMesh->OnReadSmbProc(*smbFile);
	}
}

// Called every frame
void APumiActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APumiActor::PostActorCreated()
{
	Super::PostActorCreated();
	// // Generate Mesh
	// if (!smbFile.IsEmpty()) {
	// 	pumiGenerateMesh->OnReadSmbProc(*smbFile);
	// }
}
// this will be callbacked even if open the map in editor but not Play
void APumiActor::PostLoad()
{
	Super::PostLoad();
	// // Generate Mesh
	// if (!smbFile.IsEmpty()) {
	// 	pumiGenerateMesh->OnReadSmbProc(*smbFile);
	// }
}

void APumiActor::OnPumiPointsDataLoaded(const TArray<FVector>& points, const TArray<FIntVector3>& indics)
{

}
