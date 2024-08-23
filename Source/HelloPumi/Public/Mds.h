#ifndef BB8CC391_33DF_4946_8A98_86E4C47328BD
#define BB8CC391_33DF_4946_8A98_86E4C47328BD

//typedef MDS_ID_TYPE mds_id; // MDS_ID_TYPE will be defined in CMakeLists at build time
enum {
  MDS_VERTEX,
  MDS_EDGE,
  MDS_TRIANGLE,
  MDS_QUADRILATERAL,
  MDS_WEDGE,
  MDS_PYRAMID,
  MDS_TETRAHEDRON,
  MDS_HEXAHEDRON,
  MDS_TYPES
};

typedef unsigned MDS_ID_TYPE;
typedef MDS_ID_TYPE mds_id;

#define MDS_NONE -1
#define MDS_LIVE -2

#define ID(t,i) ((i)*MDS_TYPES + (t))
#define TYPE(id) ((id) % MDS_TYPES)
#define INDEX(id) ((id) / MDS_TYPES)

struct mds {
  int d;
  mds_id n[MDS_TYPES];
  mds_id cap[MDS_TYPES];
  mds_id end[MDS_TYPES];
  int mrm[4][4];
  mds_id* down[4][MDS_TYPES];
  mds_id* up[4][MDS_TYPES];
  mds_id* first_up[4][MDS_TYPES];
  mds_id* free[MDS_TYPES];
  mds_id first_free[MDS_TYPES];
};

#define MDS_SET_MAX 256

struct mds_set {
  int n;
  mds_id e[MDS_SET_MAX];
};

struct mds_links {
  unsigned np;
  unsigned* n;
  unsigned* p;
  unsigned** l;
};
#define MDS_LINKS_INIT {0,0,0,0}


extern int const mds_dim[MDS_TYPES];
extern int const mds_degree[MDS_TYPES][4];
extern int const* mds_types[MDS_TYPES][4];

extern int smb2mds(int smb_type);
extern int mds2smb(int mds_type);
extern int down_degree(int t);
extern mds_id mds_identify(int type, mds_id idx);

// custom
struct MdsData
{
  mds_id cap[MDS_TYPES];
  double (*point)[3];
  double (*param)[2];
  int    (*indics)[3];
  struct mds_links links;
  mds_set down;

  ~MdsData() {
    if (point != nullptr)
       delete [] point;
    if (param != nullptr)
       delete [] param;

    if (links.p != nullptr)
       delete [] links.p;
    if (links.n != nullptr)
       delete [] links.n;
    if (links.l != nullptr) {
      for (unsigned i=0;i<links.np;++i)
        delete [] links.l[i];
    }
  }
};

#endif /* BB8CC391_33DF_4946_8A98_86E4C47328BD */
