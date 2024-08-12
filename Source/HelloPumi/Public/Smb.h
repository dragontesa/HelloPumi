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



#endif /* SMB_BDD1F6D2_5042_4FFF_B3C5_F820345E4EBE */
