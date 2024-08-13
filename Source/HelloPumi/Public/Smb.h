#ifndef SMB_BDD1F6D2_5042_4FFF_B3C5_F820345E4EBE
#define SMB_BDD1F6D2_5042_4FFF_B3C5_F820345E4EBE

enum { SMB_VERSION = 6 };

enum {
  SMB_VERT,
  SMB_EDGE,
  SMB_TRI,
  SMB_QUAD,
  SMB_HEX,
  SMB_PRIS,
  SMB_PYR,
  SMB_TET,
  SMB_TYPES
};

enum {
  SMB_INT,
  SMB_DBL
};

struct SmbHeader
{
    unsigned magic;
    unsigned version;
    unsigned dim;
    unsigned np;
};

struct SmbMeshData
{
   unsigned n[SMB_TYPES];
};

struct SmbVerts
{
  double (*point)[3];
  double (*param)[2];
};

struct SmbConnect
{
  unsigned (*conn)[SMB_TYPES]; // unsigned [SMB_TYPES][cap]
};


#endif /* SMB_BDD1F6D2_5042_4FFF_B3C5_F820345E4EBE */
