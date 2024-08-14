// Fill out your copyright notice in the Description page of Project Settings.
#include "PumiGenerateMesh.h"
#include "Misc/AssertionMacros.h"

DEFINE_LOG_CATEGORY(HelloPumiLog);

/* these limits are just for sanity checking
   of the input file contents.
   they do not reflect hard limitations anywhere
   else in the MDS source code,
   so feel free to increase them slightly if you
   have a strange application. */
#define MAX_ENTITIES (100*1000*1000)
#define MAX_PEERS (10*1000)
#define MAX_TAGS (100)

#define CORE_BIG_ENDIAN 0
#define ENCODED_ENDIAN CORE_BIG_ENDIAN //consistent with network byte order
#define READ_UNSIGNEDS(f,p) read_unsigneds(f,&(p), 1)

bool read_unsigneds(IFileHandle* hFile, unsigned* outUnsign, size_t bytesToRead)
{
    bool ok = hFile->Read((uint8*)outUnsign,bytesToRead);
    return ok;
}


bool read_doubles(IFileHandle* hFile, double* outDouble, size_t bytesToRead)
{
    bool ok = hFile->Read((uint8*)outDouble, bytesToRead);
    return ok;
}

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
    SmbHeader smbHead;
    bool ok = readSmbHeader(hFile,smbHead);

    // 3. Read SMB Mesh Data
    SmbMeshData smbMesh;
    ok = readSmbMeshData(hFile, smbMesh);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb mesh data"));
        return;
    }
    
    // 4. create mds data with smbMesh
    MdsData mdsData;
    ok = createMdsData(mdsData, smbMesh);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to create mds data"));
        return;
    }

    // 5. Read Link Connection Data
    SmbConnect smbConn;
    ok = readSmbConn(hFile, smbConn, mdsData.cap);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb connect data"));
        return;
    }

    // 5. Read Vertex Positions and Parameter Coordinates and Save into mds data
    ok = readSmbPointsToMds(hFile, mdsData, smbMesh.n[SMB_VERT], smbHead.version);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb points"));
        return;
    }

    // 6. Read Links

}

bool UPumiGenerateMesh::readSmbHeader(IFileHandle* hFile, SmbHeader& outHeader)
{
    READ_UNSIGNEDS(hFile, outHeader.magic);
    READ_UNSIGNEDS(hFile, outHeader.version);
    READ_UNSIGNEDS(hFile, outHeader.dim);
    READ_UNSIGNEDS(hFile, outHeader.np);
    return true;
}

bool UPumiGenerateMesh::readSmbMeshData(IFileHandle* hFile, SmbMeshData& outMeshData)
{
    read_unsigneds(hFile, outMeshData.n, SMB_TYPES);
    return true;
}

bool UPumiGenerateMesh::createMdsData(MdsData& outMdsData, const SmbMeshData& smbMeshData)
{
    MdsData* mds = &outMdsData;
    for (int i=0; i<MDS_TYPES; ++i)
    {
        unsigned tmp = smbMeshData.n[mds2smb(i)];
        if (sizeof(mds_id) == 4)
          check(tmp < MAX_ENTITIES);
        mds->cap[i] = tmp;
    }

    mds->point = new double[mds->cap[MDS_VERTEX]][3];
    mds->param = new double[mds->cap[MDS_VERTEX]][2];

    return true;
}

bool UPumiGenerateMesh::readSmbConn(
    IFileHandle* hFile
    , SmbConnect& outConn
    , mds_id mdsCap[MDS_TYPES])
{
    unsigned** outSmbConn = outConn.conn;
    unsigned* conn;
    struct mds_set down;
    mds_id cap;
    size_t size;
    int type_mds;
    int i;
    for (i=0;i<SMB_TYPES;++i) {
        type_mds = smb2mds(i);
        down.n = down_degree(type_mds);
        cap = mdsCap[type_mds];
        size = down.n * cap;
        conn = new unsigned[size*sizeof(*conn)];
        outSmbConn[i] = conn;
        read_unsigneds(hFile,conn, size);
    }

    return true;
}

bool UPumiGenerateMesh::readSmbPointsToMds(
    IFileHandle* hFile
    , MdsData& outMdsData
    , unsigned smbVerts
    , unsigned version)
{
    double *point = &outMdsData.point[0][0];
    double *param = &outMdsData.param[0][0];
    read_unsigneds(hFile, point, 3 * smbVerts);
    if (version >= 2)
        read_doubles(hFile, param, 2 * smbVerts);
    
    return true;
}


bool UPumiGenerateMesh::readSmbRemotesToMds(IFileHandle* hFile, mds_links& outMdsRemotes)
{
    READ_UNSIGNEDS(hFile, outMdsRemotes.np);
    if (!outMdsRemotes.np) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb points"));
        return false;
    }
}

void UPumiGenerateMesh::freeMdsData(const MdsData& mdsData)
{

}