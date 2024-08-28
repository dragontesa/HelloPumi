// Fill out your copyright notice in the Description page of Project Settings.
#include "PumiActor.h"
#include "PumiGenerateMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetProceduralMeshLibrary.h"

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
	RootComponent = mesh;
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

void APumiActor::OnPumiPointsDataLoaded(const TArray<FVector>& points, const TArray<int32>& indics)
{
	GenerateProceduralMesh100(points, indics);
}


void APumiActor::GenerateProceduralMesh100(const TArray<FVector>& points, const TArray<int32>& indics)
{
	if (points.Num() < 2) {
		UE_LOG(HelloPumiLog, Log, TEXT("invalid vertics number"));
		return;
	}

	TArray<FVector> normals;
	for (int i=0;i<indics.Num()/3-2;i+=3) {
		FVector one = points[indics[i*3+1]] - points[indics[i*3]];
		FVector two = points[indics[i*3+1]] - points[indics[i*3+2]];
		FVector norm = UKismetMathLibrary::Cross_VectorVector(one,two);
		Normalize(norm);
		normals.Add(norm);
	}

	UE_LOG(HelloPumiLog, Log, TEXT("Verts num=%d"), points.Num());
	UE_LOG(HelloPumiLog, Log, TEXT("Index num=%d"), indics.Num()/3);
	UE_LOG(HelloPumiLog, Log, TEXT("Normals num=%d"), normals.Num());

    int vertCap = points.Num();
	if (vertCap%2 == 1)
	    vertCap -= 1;

    const float scaleWeight = 100.0f;
	float uvDistance = 0.0f;
	float vrtDistanceScale = FVector3d::Distance(points[0],points[1]) / scaleWeight;
	TArray<FVector2D> UVs;
	UVs.Add(FVector2D(0,1));
	UVs.Add(FVector2D(0,0));

	for(int i=2;i<vertCap-1;i+=2)
	{
		uvDistance += FVector3d::Distance(points[i], points[i-2]) / scaleWeight / vrtDistanceScale;
		UVs.Add(FVector2D(uvDistance, 1));
		UVs.Add(FVector2D(uvDistance, 0));
	}

    TArray<FVector> normals2;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;
	vertexColors.Add(FLinearColor(0.65, 0.65, 0.65, 1.0));
	vertexColors.Add(FLinearColor(0.65, 0.65, 0.65, 1.0));
	vertexColors.Add(FLinearColor(0.65, 0.65, 0.65, 1.0));

    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(points,indics,UVs,normals2, tangents);

#if 0
	mesh->CreateMeshSection_LinearColor(0, points, indics, normals, UVs, vertexColors, tangents, true);
#else
	mesh->CreateMeshSection(0, points, indics, normals, UVs,  TArray<FColor>(), TArray<FProcMeshTangent>(), false);
#endif
	// mesh->ContainsPhysicsTriMeshData(true);
}


void APumiActor::GenerateProceduralMesh200(const TArray<FVector>& points, const TArray<int32>& indics)
{
	//UE::Geometry::FPolygon2d polyg = FPoloygon3D(points);
}