// Fill out your copyright notice in the Description page of Project Settings.
#include "PumiGenerateMesh.h"

DEFINE_LOG_CATEGORY(HelloPumiLog);

TSharedPtr<IFileHandle> UPumiGenerateMesh::openSbmFile(const TCHAR* smbFilePath, bool bAllowWrite)
{
    FPlatformFileManager& pfm = FPlatformFileManager::Get();
    IPlatformFile* pf = pfm.GetPlatformFile(TEXT("MyPlatformFile"));
    if (pf == nullptr) {
        UE_LOG(HelloPumiLog, Log, TEXT("Failure to create platform file"));
        return TSharedPtr<IFileHandle>(nullptr);
    }

    IFileHandle* fh = pf->OpenRead(smbFilePath,bAllowWrite);
    if (fh == nullptr) {
        UE_LOG(HelloPumiLog, Log, TEXT("Failure to open read file"));
        return TSharedPtr<IFileHandle>(nullptr);
    }

    TSharedPtr<IFileHandle> fhptr(fh);
    return fhptr;
}

void UPumiGenerateMesh::OnReadSmbProc(const TCHAR* smbFilePath)
{
    // 1. Open SmbFile
    TSharedPtr<IFileHandle> shf = openSbmFile(smbFilePath,true);
    if (shf == nullptr)
    {
        return;
    }

    IFileHandle* hFile = shf.Get();
    // 2. Read Header
    SmbHeader smbHead = readSmbHeader(hFile);

    // 3. Read SMB Mesh Data
    unsigned smbMesh[SMB_TYPES];
    bool ok = readSmbMesh(hFile, smbMesh);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb mesh data"));
        return;
    }
    
    // 4. Read Link Connection Data
    // 5. Read Vertex Positions and Parameter Coordinates
    // 6. 
}

SmbHeader UPumiGenerateMesh::readSmbHeader(IFileHandle* hFile)
{
    return SmbHeader();
}

bool UPumiGenerateMesh::readSmbMesh(IFileHandle* hFile, unsigned outMeshData[SMB_TYPES])
{
    return true;
}