// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/PlatformFileManager.h"
#include "Math/MathFwd.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Generators/MeshShapeGenerator.h"
#include "Generators/SweepGenerator.h"
#include "Smb.h"
#include "Mds.h"
#include "PumiGenerateMesh.generated.h"

using namespace UE::Geometry;

DECLARE_LOG_CATEGORY_EXTERN(HelloPumiLog, Log, All);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPumiPointsDataLoadFinished, const TArray<FVector>&, points, const TArray<int32>&, indics);



/**
 * 
 */
UCLASS()
class HELLOPUMI_API UPumiGenerateMesh : public UObject
{
	GENERATED_BODY()
public:
     FPumiPointsDataLoadFinished DelPointsDataLoadFinished;
	void OnReadSmbProc(const TCHAR* smbFilePath);

    
protected:
    TSharedPtr<IFileHandle> openSbmFile(const TCHAR* smbFilePath, bool bAllowWrite);
    
    bool readSmbHeader(IFileHandle* hFile, SmbHeader& outHeader);
    bool readSmbMeshData(IFileHandle* hFile, SmbMeshData& outMeshData);
    bool createMdsData(MdsData& outMdsData, const SmbMeshData& smbMeshData);

    bool readSmbConn(IFileHandle* hFile, SmbConnect& outConn, MdsData& outMdsData, mds_id mdsCap[MDS_TYPES]);
	bool readSmbPoints(IFileHandle* hFile, MdsData& outMdsData, unsigned smbVerts, unsigned version);
    bool readSmbRemotes(IFileHandle* hFile, mds_links* outLinkes);

    void freeMdsData(const MdsData& mdsData);
};
