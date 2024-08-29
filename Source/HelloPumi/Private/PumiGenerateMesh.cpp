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

static const uint16_t endian_value = 1;
#define ENDIANNESS ((*((uint8_t*)(&endian_value)))==1)
#define CORE_BIG_ENDIAN 0
#define ENCODED_ENDIAN CORE_BIG_ENDIAN //consistent with network byte order

#define CORE_BIG_ENDIAN 0
#define ENCODED_ENDIAN CORE_BIG_ENDIAN //consistent with network byte order
#define READ_UNSIGNEDS(f,p) read_unsigneds(f,&(p), 1)

const char* SMB_TYPE_NAMES[] = {
    "SMB_VERT (Vertex)",
    "SMB_EDGE (Edge)",
    "SMB_TRI  (Triangle)",
    "SMB_QUAD (Quadrilateral)",
    "SMB_HEX  (Hexagon)",
    "SMB_PRIS (Prism)",
    "SMB_PYR  (Pyramid)",
    "SMB_TET  (Tetrahedron)"
};

const char* MDS_TYPE_NAMES[] = {
    "MDS_VERTEX",
    "MDS_EDGE",
    "MDS_TRIANGLE",
    "MDS_QUADRILATERAL",
    "MDS_WEDGE",
    "MDS_PYRAMID",
    "MDS_TETRAHEDRON",
    "MDS_HEXAHEDRON"
};

static void swap_32(uint32_t* p)
{
  uint32_t a = *p;
#if defined(__GNUC__)
  a = __builtin_bswap32(a);
#elif defined(_MSC_VER)
  a = _byteswap_ulong(a);
#else
  a = ((a & 0x000000FF) << 24) | ((a & 0x0000FF00) << 8) |
      ((a & 0x00FF0000) >> 8) | ((a & 0xFF000000) >> 24);
#endif
  *p = a;
}

static void swap_64(uint64_t* p)
{
  uint64_t a = *p;
#if defined(__GNUC__) && !defined(__CUDA_ARCH__)
  a = __builtin_bswap64(a);
#elif defined(_MSC_VER) && !defined(__CUDA_ARCH__)
  a = _byteswap_uint64(a);
#else
  a = ((a & 0x00000000000000FFULL) << 56) |
      ((a & 0x000000000000FF00ULL) << 40) |
      ((a & 0x0000000000FF0000ULL) << 24) | ((a & 0x00000000FF000000ULL) << 8) |
      ((a & 0x000000FF00000000ULL) >> 8) | ((a & 0x0000FF0000000000ULL) >> 24) |
      ((a & 0x00FF000000000000ULL) >> 40) | ((a & 0xFF00000000000000ULL) >> 56);
#endif
  *p = a;
}

void swap_unsigneds(unsigned* p, size_t n)
{
  check(sizeof(unsigned)==4);
  for (size_t i=0; i < n; ++i)
    swap_32(p++);
}

void swap_doubles(double* p, size_t n)
{
  check(sizeof(double)==8);
  for (size_t i=0; i < n; ++i)
    swap_64((uint64_t*)(p++));
}


bool read_unsigneds(IFileHandle* hFile, unsigned* outUnsign, size_t n)
{
    bool ok = hFile->Read((uint8*)outUnsign,n*sizeof(unsigned));
    if (ENDIANNESS != ENCODED_ENDIAN)
      swap_unsigneds(outUnsign,n);
    return ok;
}


bool read_doubles(IFileHandle* hFile, double* outDouble, size_t n)
{
    bool ok = hFile->Read((uint8*)outDouble, n*sizeof(double));
    if (ENDIANNESS != ENCODED_ENDIAN)
       swap_doubles(outDouble, n);
    return ok;
}

TSharedPtr<IFileHandle> UPumiGenerateMesh::openSbmFile(const TCHAR* smbFilePath, bool bAllowWrite)
{
    IPlatformFile& pf = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fh = pf.OpenRead(smbFilePath,bAllowWrite);
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

    // unsigned test0 = 1000;
    // int test = test0;
    // UE_LOG(HelloPumiLog, Log, TEXT("unsigned = %u, int = %d, %u"),test0,test,test);

    IFileHandle* hFile = shf.Get();
    // int64 pos = hFile->Tell();
    // UE_LOG(HelloPumiLog, Log, TEXT("pos = %ld"),pos);

    // 2. Read Header
    SmbHeader smbHead;
    bool ok = readSmbHeader(hFile,smbHead);

    // pos = hFile->Tell();
    // UE_LOG(HelloPumiLog, Log, TEXT("pos = %ld"),pos);

    // 3. Read SMB Mesh Data
    SmbMeshData smbMesh;
    ok = readSmbMeshData(hFile, smbMesh);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb mesh data"));
        return;
    }
    
    // pos = hFile->Tell();
    // UE_LOG(HelloPumiLog, Log, TEXT("pos = %ld"),pos);

    // 4. create mds data with smbMesh
    MdsData mdsData;
    ok = createMdsData(mdsData, smbMesh);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to create mds data"));
        return;
    }
    // pos = hFile->Tell();
    // UE_LOG(HelloPumiLog, Log, TEXT("pos = %ld"),pos);

    // 5. Read Link Connection Data
    SmbConnect smbConn;
    ok = readSmbConn(hFile, smbConn, mdsData, mdsData.cap);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb connect data"));
        return;
    }

    // 5. Read Vertex Positions and Parameter Coordinates and Save into mds data
    ok = readSmbPoints(hFile, mdsData, smbMesh.n[SMB_VERT], smbHead.version);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb points"));
        return;
    }

    // 6. Read Links
    ok = readSmbRemotes(hFile, &mdsData.links);
    if (!ok) {
        UE_LOG(HelloPumiLog, Log, TEXT("failed to read smb remotes"));
        //return;
    }


    // 7. Copy mds points and tri buffer to delegate
    double x,y,z;
    TArray<FVector> mdsPoints;
    TArray<int32> mdsTris;
    for (unsigned i=0;i<mdsData.cap[MDS_VERTEX];++i)
    {
        x = mdsData.point[i][0] * 10.f;
        y = mdsData.point[i][1] * 10.f;
        z = mdsData.point[i][2] * 10.f;
        mdsPoints.Add(FVector(x,y,z));

#if 0 // for debug
    double u,v;
        if (smbHead.version >= 2) {
            u = mdsData.param[i][0];
            v = mdsData.param[i][1];
           mdsParams.Add(FVector2D(u,v));
        }
#endif
    }

    for (unsigned j=0;j<mdsData.cap[MDS_TRIANGLE];++j)
    {
      mdsTris.Add(mdsData.tri[j][0]);
      mdsTris.Add(mdsData.tri[j][1]);
      mdsTris.Add(mdsData.tri[j][2]);
    }

    // 8. Post delegation
    DelPointsDataLoadFinished.ExecuteIfBound(mdsPoints, mdsTris);
}

bool UPumiGenerateMesh::readSmbHeader(IFileHandle* hFile, SmbHeader& outHeader)
{
    READ_UNSIGNEDS(hFile, outHeader.magic);
    READ_UNSIGNEDS(hFile, outHeader.version);
    READ_UNSIGNEDS(hFile, outHeader.dim);
    READ_UNSIGNEDS(hFile, outHeader.np);

    UE_LOG(HelloPumiLog, Log, TEXT("----- smb header ----\n magic=%u, ver=%u, dim=%u, np=%u")
    , outHeader.magic
    , outHeader.version
    , outHeader.dim
    , outHeader.np);

    return true;
}

bool UPumiGenerateMesh::readSmbMeshData(IFileHandle* hFile, SmbMeshData& outMeshData)
{
    unsigned * n = outMeshData.n;
    read_unsigneds(hFile, n, SMB_TYPES);
    UE_LOG(HelloPumiLog, Log, TEXT("----- smb mesh data ----"));
    #if 0
    for (int i=0;i<SMB_TYPES;++i) 
    {
      UE_LOG(HelloPumiLog, Log, TEXT("[%hs]\t\t = %u"), SMB_TYPE_NAMES[i], n[i]);
    }
    #endif
    return true;
}

bool UPumiGenerateMesh::createMdsData(MdsData& outMdsData, const SmbMeshData& smbMeshData)
{
    UE_LOG(HelloPumiLog, Log, TEXT("----- mds mesh data ----"));
    MdsData* mds = &outMdsData;
    for (int i=0; i<MDS_TYPES; ++i)
    {
        unsigned tmp = smbMeshData.n[mds2smb(i)];
        if (sizeof(mds_id) == 4)
          check(tmp < MAX_ENTITIES);

        mds->cap[i] = tmp;
        UE_LOG(HelloPumiLog, Log, TEXT("[%hs] = %u"), MDS_TYPE_NAMES[i], tmp);
    }

    mds->point = new double[mds->cap[MDS_VERTEX]][3];
    mds->param = new double[mds->cap[MDS_VERTEX]][2];

    return true;
}

bool UPumiGenerateMesh::readSmbConn(
    IFileHandle* hFile
    , SmbConnect& outConn
    , MdsData& outMdsData
    , mds_id mdsCap[MDS_TYPES])
{
    UE_LOG(HelloPumiLog, Log, TEXT("----- smb connectivity ----"));
    unsigned** outSmbConn = outConn.conn;
    unsigned* conn;
    struct mds_set& down = outMdsData.down;
    int const* dt;
    mds_id cap;
    size_t size;
    int type_mds;
    int i;
    int mds_index;

    for (i=1;i<SMB_TYPES;++i) {
        type_mds = smb2mds(i);
        down.n = down_degree(type_mds);
        cap = mdsCap[type_mds];
        dt = mds_types[type_mds][mds_dim[type_mds] - 1];
        size = down.n * cap;
        conn = new unsigned[size];
        outSmbConn[i] = conn;
        read_unsigneds(hFile,conn, size);

        UE_LOG(HelloPumiLog, Log, TEXT("entity %hs"), MDS_TYPE_NAMES[type_mds]);
        for (mds_id j = 0; j < cap; ++j) {
            FString logIds = FString::Printf(TEXT("\tID %d :"),j);
            FString logConnecty;
          for (int k = 0; k < down.n; ++k) {
            mds_index = conn[j * down.n + k];
            down.e[k] = mds_identify(dt[k], mds_index);
            logConnecty += FString::Printf(TEXT("%2d th connect %2d\t"), k, mds_index);
          }
          UE_LOG(HelloPumiLog, Log, TEXT("%s %s"), *logIds, *logConnecty);
        }
    }

    // copy index data
    cap = mdsCap[MDS_EDGE];
    conn = outSmbConn[SMB_EDGE];
    int down_n = down_degree(MDS_EDGE);
    unsigned (*mdsEdge)[2] = new unsigned[cap][2];
    UE_LOG(HelloPumiLog, Log, TEXT("mdsEdge=%u, down_n=%d"),cap,down_n);
    for (mds_id j=0;j<cap;++j) {
      for (int k = 0; k < down_n; ++k) {
          mdsEdge[j][k] = conn[j * down_n + k]; // a edge has two vertexs
      }
    }

    cap = mdsCap[MDS_TRIANGLE];
    conn = outSmbConn[SMB_TRI];
    down_n = down_degree(MDS_TRIANGLE);
    int edgeIndex[3];
    outMdsData.tri = new int[cap][3];
    int (*tri)[3] = outMdsData.tri;
    UE_LOG(HelloPumiLog, Log, TEXT("mdsTri=%u, down_n=%d"),cap,down_n);
    for (mds_id j = 0;j<cap;++j) {
      for (int k = 0; k < down_n; k+=3) {
          edgeIndex[0] = conn[j * down_n + k];
          edgeIndex[1] = conn[j * down_n + k+1];
          // UE_LOG(HelloPumiLog, Log, TEXT("edgeIndex0=%d, edgeIndex1=%d"),edgeIndex[0],edgeIndex[1]);
          tri[j][0] = mdsEdge[edgeIndex[0]][0];
          tri[j][1] = mdsEdge[edgeIndex[0]][1];
          if (mdsEdge[edgeIndex[0]][0] != mdsEdge[edgeIndex[1]][0]
          && mdsEdge[edgeIndex[0]][1] != mdsEdge[edgeIndex[1]][0])
            tri[j][2] = mdsEdge[edgeIndex[1]][0];
          else
            tri[j][2] = mdsEdge[edgeIndex[1]][1];
      }
    }

    delete [] mdsEdge;

    return true;
}

bool UPumiGenerateMesh::readSmbPoints(
    IFileHandle* hFile
    , MdsData& outMdsData
    , unsigned smbVerts
    , unsigned version)
{
    UE_LOG(HelloPumiLog, Log, TEXT("----- smb points  ----"));
    double *point = &outMdsData.point[0][0];
    double *param = &outMdsData.param[0][0];
    read_doubles(hFile, point, 3 * smbVerts);
    if (version >= 2)
        read_doubles(hFile, param, 2 * smbVerts);
    else {
        // unsigned pi, pj;
        for (unsigned pi = 0; pi < smbVerts; ++pi) {
          // for (pj = 0; pj < 2; ++pj) outMdsData.param[pi][pj] = 0.0;
          outMdsData.param[pi][0] = 0.0;
          outMdsData.param[pi][1] = 0.0;
        }
    }

    #if 0 // for debug
    // print vertex points
    double (*verts)[3] = reinterpret_cast<double(*)[3]>(point);
    for (unsigned i=0;i<smbVerts;++i) {
        UE_LOG(HelloPumiLog, Log, TEXT("%u: %.6f, %.6f, %.6f"), i, verts[i][0], verts[i][1], verts[i][2]);
    }
    #endif

    return true;
}


bool UPumiGenerateMesh::readSmbRemotes(IFileHandle* hFile, mds_links* outLinkes)
{
    mds_links* links = outLinkes;
    READ_UNSIGNEDS(hFile, links->np);
    if (!links->np) {
        UE_LOG(HelloPumiLog, Log, TEXT("not found remote info from smb"));
        return false;
    }

  links->p = new unsigned[links->np];
  read_unsigneds(hFile, links->p, links->np);
  links->n = new unsigned[links->np];
  links->l = new unsigned*[links->np];
  read_unsigneds(hFile, links->n, links->np);
  for (unsigned i=0;i<links->np;++i)
  {
    if (sizeof(mds_id) == 4) check(links->n[i] < MAX_ENTITIES);
    links->l[i] = new unsigned[links->n[i]];
    read_unsigneds(hFile, links->l[i], links->n[i]);
  }
  return true;
}

void UPumiGenerateMesh::freeMdsData(const MdsData& mdsData)
{
    
}