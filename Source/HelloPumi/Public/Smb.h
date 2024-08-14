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

struct SmbConnect
{
  unsigned* conn[SMB_TYPES]; // unsigned [cap][SMB_TYPES]
  SmbConnect() {
    for (int i=0;i<SMB_TYPES;++i,conn[i]=nullptr;);
  }
  ~SmbConnect() {
    for (int i=0;i<SMB_TYPES; ++i) {
      if (conn[i] != nullptr)
         delete conn[i];
    }
  }
};


#endif /* SMB_BDD1F6D2_5042_4FFF_B3C5_F820345E4EBE */
