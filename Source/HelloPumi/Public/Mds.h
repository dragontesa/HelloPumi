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

typedef int MDS_ID_TYPE;
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

extern int const mds_dim[MDS_TYPES];
extern int const mds_degree[MDS_TYPES][4];
extern int const* mds_types[MDS_TYPES][4];

extern int smb2mds(int smb_type);
extern int mds2smb(int mds_type);
extern int down_degree(int t);

// custom
struct MdsData
{
  mds_id cap[MDS_TYPES];
  double (*point)[3];
  double (*param)[2];
};

#endif /* BB8CC391_33DF_4946_8A98_86E4C47328BD */
