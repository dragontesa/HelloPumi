// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HAL/PlatformFileManager.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Smb.h"
#include "Mds.h"
#include "PumiGenerateMesh.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(HelloPumiLog, Log, All);

/**
 * 
 */
UCLASS()
class HELLOPUMI_API UPumiGenerateMesh : public UObject
{
	GENERATED_BODY()

    TSharedPtr<IFileHandle> openSbmFile(const TCHAR* smbFilePath, bool bAllowWrite);
	void OnReadSmbProc(const TCHAR* smbFilePath);
    SmbHeader readSmbHeader(IFileHandle* hFile);
    bool readSmbMesh(IFileHandle* hFile, unsigned outMeshData[SMB_TYPES]);
};
