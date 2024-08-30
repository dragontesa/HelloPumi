// Fill out your copyright notice in the Description page of Project Settings.
#include "PumiActor.h"
#include "PumiGenerateMesh.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetProceduralMeshLibrary.h"
#include "DynamicMesh/MeshNormals.h"
#include "DynamicMesh/DynamicMeshAttributeSet.h"
#include "DynamicMesh/MeshAttributeUtil.h"
#include "UDynamicMesh.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshNormalsFunctions.h"

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

	pmc = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = pmc;
	// New in UE 4.17, multi-threaded PhysX cooking.
	
	pmc->bUseAsyncCooking = false;
	pmc->bUseComplexAsSimpleCollision = true;
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

void APumiActor::OnPumiPointsDataLoaded(const TArray<FVector>& vertics, const TArray<int32>& indics)
{
	GenerateProceduralMesh300(vertics, indics);
}


void APumiActor::GenerateProceduralMesh100(const TArray<FVector>& vertics, const TArray<int32>& indics)
{
	if (vertics.Num() < 2) {
		UE_LOG(HelloPumiLog, Log, TEXT("invalid vertics number"));
		return;
	}

    pmc->ClearAllMeshSections();

    int32 NumTris    = indics.Num()/3;
	int32 NumVertices = vertics.Num(); // same as NumTris * 3;

	TArray<FVector> normals;
	normals.SetNumUninitialized(vertics.Num());
	for (int i=0;i<NumTris-2;i+=3) {
		FVector one = vertics[indics[i*3+1]] - vertics[indics[i*3]];
		FVector two = vertics[indics[i*3+2]] - vertics[indics[i*3]];
		// FVector n = UKismetMathLibrary::Cross_VectorVector(one,two);
		FVector n = FVector::CrossProduct(one,two);
		n.Normalize();
		normals.Add(n);
	}

	if (NumVertices%2 == 1)
	    NumVertices -= 1;

    const float scaleWeight = 10.0f;
	float uvDistance = 0.0f;
	float vrtDistanceScale = FVector3d::Distance(vertics[0],vertics[1]);
	TArray<FVector2D> UVs;
	UVs.Add(FVector2D(0,1));
	UVs.Add(FVector2D(0,0));

	for(int i=2;i<NumVertices-1;i+=2)
	{
		uvDistance += FVector3d::Distance(vertics[i], vertics[i-2]) / vrtDistanceScale / scaleWeight;
		UVs.Add(FVector2D(uvDistance, 1));
		UVs.Add(FVector2D(uvDistance, 0));
	}

	uvDistance += FVector3d::Distance(vertics[vertics.Num()-1], vertics[vertics.Num()-2]) / vrtDistanceScale / scaleWeight;
	UVs.Add(FVector2D(uvDistance, 1));

	TArray<FVector> normals2;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;
	vertexColors.Add(FLinearColor(0.70, 0.70, 0.85, 1.0));
	vertexColors.Add(FLinearColor(0.70, 0.70, 0.85, 1.0));
	vertexColors.Add(FLinearColor(0.70, 0.70, 0.85, 1.0));

//    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertics,indics,UVs,normals, tangents);

    UE_LOG(HelloPumiLog, Log, TEXT("Verts num=%d"), vertics.Num());
	UE_LOG(HelloPumiLog, Log, TEXT("Index num=%d"), indics.Num()/3);
	UE_LOG(HelloPumiLog, Log, TEXT("Normals num=%d"), normals.Num());
    UE_LOG(HelloPumiLog, Log, TEXT("UVs num=%d"), UVs.Num());

#if 1
	pmc->CreateMeshSection_LinearColor(0, vertics, indics, normals, UVs, vertexColors, tangents, true);
#else
	pmc->CreateMeshSection(0, vertics, indics, normals, UVs,  TArray<FColor>(), TArray<FProcMeshTangent>(), false);
#endif
	// pmc->ContainsPhysicsTriMeshData(true);
}


void APumiActor::GenerateProceduralMesh200(const TArray<FVector>& vertics, const TArray<int32>& indics)
{
	const int32 NumTris    = indics.Num()/3;
	const int32 NumVertices = vertics.Num(); // same as NumTris * 3;

    // Reset dynamic mesh
	FDynamicMesh3& OutMesh = DynMesh3;
	OutMesh = FDynamicMesh3();

    // Setup Dynamic Mesh Vertics
	for (const FVector& v: vertics)
	{
		OutMesh.AppendVertex(FVector3d(v.X,v.Y,v.Z));
	}

    // Set Mesh Triangle
	for (int i=0;i<NumTris-2;++i)
	{
		UE::Geometry::FIndex3i idx(indics[3*i],indics[3*i+1],indics[3*i+2]);
	    OutMesh.AppendTriangle(idx);
	}

    GenerateProceduralMeshWithDynamicMesh(OutMesh);
}

void APumiActor::GenerateProceduralMesh300(const TArray<FVector>& vertics, const TArray<int32>& indics)
{
    // Reset dynamic mesh
	// FDynamicMesh3& OutMesh = DynMesh3;
	// OutMesh = FDynamicMesh3();
	UDynamicMesh* GeoMesh = NewObject<UDynamicMesh>();
	GeoMesh->Reset();

	FPolygon2d polygon;
	for (const FVector& v: vertics) {
		polygon.AppendVertex(FVector2d(v));
	}
	
#if 0
	if (polygon.IsClockwise()) {
		polygon.Reverse();
	}
#endif

   UGeometryScriptLibrary_MeshPrimitiveFunctions::AppendSimpleExtrudePolygon(
	GeoMesh
	, FGeometryScriptPrimitiveOptions()
	, FTransform()
	, polygon.GetVertices()
	, 100
	, 5);

	UGeometryScriptLibrary_MeshNormalsFunctions::ComputeSplitNormals(
		GeoMesh
		,FGeometryScriptSplitNormalsOptions()
		,FGeometryScriptCalculateNormalsOptions());

		FDynamicMesh3& OutMesh = GeoMesh->GetMeshRef();

		GenerateProceduralMeshWithDynamicMesh(OutMesh);
}

void APumiActor::GenerateProceduralMeshWithDynamicMesh(FDynamicMesh3& dynMesh)
{
	pmc->ClearAllMeshSections();
	//UE::Geometry::FPolygon2d polyg = FPoloygon3D(vertics);
	bool bVertexNormals = false;
	bool bFaceNormals = true;
	bool bSplitNormals = false;
	bool bTexCoords = true;
	bool bVertexColors = false;
    
	FDynamicMesh3& OutMesh = dynMesh;
	// Compute Normals
	TArray<FVector> Vertices, Normals;
	int32 NumMeshTri = OutMesh.TriangleCount();
	int32 NumMeshVert = NumMeshTri*3;
	Vertices.SetNumUninitialized(NumMeshVert);
	Normals.SetNumUninitialized(NumMeshVert);

	const FDynamicMeshNormalOverlay* NormalOverlay = nullptr;
	if (bVertexNormals) {
		OutMesh.EnableAttributes();
		FDynamicMeshNormalOverlay* normalOver = OutMesh.Attributes()->PrimaryNormals();
		FMeshNormals::InitializeOverlayToPerVertexNormals(normalOver, false);
		NormalOverlay = normalOver;
	}

	if (bFaceNormals) {
		OutMesh.EnableAttributes();
		FDynamicMeshNormalOverlay* normalOver = OutMesh.Attributes()->PrimaryNormals();
	    FMeshNormals::InitializeOverlayToPerTriangleNormals(normalOver);
		NormalOverlay = normalOver;
	}

	FMeshNormals vertNormals(&OutMesh);
	if (OutMesh.HasAttributes() == false && bFaceNormals == false) {
		vertNormals.ComputeVertexNormals();
		bVertexNormals = true;
	}
	

    // Mapping UV for all vertics
	const FDynamicMeshUVOverlay* UVOverlay = nullptr;
	TArray<FVector2D> UV0;
	if (bTexCoords){
		OutMesh.EnableAttributes();
		UV0.SetNum(NumMeshVert);
		FDynamicMeshUVOverlay* uvOver = OutMesh.Attributes()->PrimaryUV();
		UVOverlay = uvOver;
	    OutMesh.EnableVertexUVs(FVector2f::Zero());
		UE::Geometry::CopyVertexUVsToOverlay(OutMesh,*uvOver);
	}

	// Vertex Colors
	TArray<FLinearColor> VtxColors;
	if (bVertexColors && OutMesh.HasVertexColors())
	{
		VtxColors.SetNum(NumMeshVert);
	}

    // Tangents
	TArray<FProcMeshTangent> Tangents;		// not supporting this for now


    TArray<int32> Triangles;
	Triangles.SetNumUninitialized(NumMeshTri*3);

	FVector3d Position[3];
	FVector3f Normal[3];
	FVector2f UV[3];
	int32 BufferIndex = 0;
	for (int32 tid : OutMesh.TriangleIndicesItr())
	{
		int32 k = 3 * (BufferIndex++);

		FIndex3i TriVerts = OutMesh.GetTriangle(tid);

		OutMesh.GetTriVertices(tid, Position[0], Position[1], Position[2]);
		Vertices[k] = (FVector)Position[0];
		Vertices[k+1] = (FVector)Position[1];
		Vertices[k+2] = (FVector)Position[2];


		if (bVertexNormals)
		{
			Normals[k] = (FVector)vertNormals[TriVerts.A];
			Normals[k+1] = (FVector)vertNormals[TriVerts.B];
			Normals[k+2] = (FVector)vertNormals[TriVerts.C];
		}
		else if (NormalOverlay != nullptr && bFaceNormals == false)
		{
			NormalOverlay->GetTriElements(tid, Normal[0], Normal[1], Normal[2]);
			Normals[k] = (FVector)Normal[0];
			Normals[k+1] = (FVector)Normal[1];
			Normals[k+2] = (FVector)Normal[2];
		}
		else
		{
			FVector3d TriNormal = OutMesh.GetTriNormal(tid);
			Normals[k] = (FVector)TriNormal;
			Normals[k+1] = (FVector)TriNormal;
			Normals[k+2] = (FVector)TriNormal;
		}

		if (UVOverlay != nullptr && UVOverlay->IsSetTriangle(tid))
		{
			UVOverlay->GetTriElements(tid, UV[0], UV[1], UV[2]);
			UV0[k] = (FVector2D)UV[0];
			UV0[k+1] = (FVector2D)UV[1];
			UV0[k+2] = (FVector2D)UV[2];
		}

		if (bVertexColors)
		{
			VtxColors[k] = (FLinearColor)OutMesh.GetVertexColor(TriVerts.A);
			VtxColors[k+1] = (FLinearColor)OutMesh.GetVertexColor(TriVerts.B);
			VtxColors[k+2] = (FLinearColor)OutMesh.GetVertexColor(TriVerts.C);
		}

		Triangles[k] = k;
		Triangles[k+1] = k+1;
		Triangles[k+2] = k+2;
	}

    UE_LOG(HelloPumiLog, Log, TEXT("Verts num=%d"), Vertices.Num());
	UE_LOG(HelloPumiLog, Log, TEXT("Index num=%dd"), Triangles.Num()/3);
	UE_LOG(HelloPumiLog, Log, TEXT("Normals num=%d"), Normals.Num());
	UE_LOG(HelloPumiLog, Log, TEXT("UVs num=%d"), UV0.Num());
    // Generate Mesh Component
    pmc->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VtxColors, Tangents, true);
}


void APumiActor::UpdateMeshComponent()
{
	pmc->ClearAllMeshSections();

	int32 numTriangles = DynMesh3.TriangleCount();
	int32 numVertics   = numTriangles*3;
	//TArray<FVector> 
}