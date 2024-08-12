#ifndef BB8CC391_33DF_4946_8A98_86E4C47328BD
#define BB8CC391_33DF_4946_8A98_86E4C47328BD

//typedef MDS_ID_TYPE mds_id; // MDS_ID_TYPE will be defined in CMakeLists at build time
typedef int MDS_ID_TYPE;
typedef MDS_ID_TYPE mds_id;

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

#endif /* BB8CC391_33DF_4946_8A98_86E4C47328BD */
