#include "Mds.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
#include "Smb.h"


void reel_fail(const char* format, ...)
{
}
//  __attribute__((noreturn,format(printf,1,2)));

void reel_protect(void)
{

}

static void* mds_realloc(void* p, size_t n)
{
  if ((!p)&&(!n))
    return NULL;
  if (n)
    p = realloc(p,n);
  else {
    free(p);
    p = NULL;
  }
  if ((!p) && (n))
    reel_fail("MDS ran out of memory!\n");
  return p;
}

#define REALLOC(r,p,n) ((p)=(r)mds_realloc(p,(n)*sizeof(*(p))))
#define ZERO(o) memset(&(o),0,sizeof(o))


int const mds_dim[MDS_TYPES] =
{0 /* MDS_VERTEX */
,1 /* MDS_EDGE */
,2 /* MDS_TRIANGLE */
,2 /* MDS_QUADRILATERAL */
,3 /* MDS_WEDGE */
,3 /* MDS_PYRAMID */
,3 /* MDS_TETRAHEDRON */
,3 /* MDS_HEXAHEDRON */
};

int const mds_degree[MDS_TYPES][4] =
{{1, 0,0,0} /* MDS_VERTEX */
,{2, 1,0,0} /* MDS_EDGE */
,{3, 3,1,0} /* MDS_TRIANGLE */
,{4, 4,1,0} /* MDS_QUADRILATERAL */
,{6, 9,5,1} /* MDS_WEDGE */
,{5, 8,5,1} /* MDS_PYRAMID */
,{4, 6,4,1} /* MDS_TETRAHEDRON */
,{8,12,6,1} /* MDS_HEXAHEDRON */
};

static int const e0[] = {MDS_VERTEX,MDS_VERTEX};

static int const t0[] = {MDS_VERTEX,MDS_VERTEX,MDS_VERTEX};
static int const t1[] = {MDS_EDGE,MDS_EDGE,MDS_EDGE};

static int const t01[] =     {0,1,1,2,2,0};
static int const t10[] = {2,0,0,1,1,2};

static int const q0[] = {MDS_VERTEX,MDS_VERTEX,MDS_VERTEX,MDS_VERTEX};
static int const q1[] = {MDS_EDGE,MDS_EDGE,MDS_EDGE,MDS_EDGE};

static int const q01[] =     {0,1,1,2,2,3,3,0};
static int const q10[] = {3,0,0,1,1,2,2,3};

static int const T0[] = {MDS_VERTEX,MDS_VERTEX,MDS_VERTEX,MDS_VERTEX};
static int const T1[] = {MDS_EDGE,MDS_EDGE,MDS_EDGE
                        ,MDS_EDGE,MDS_EDGE,MDS_EDGE};
static int const T2[] = {MDS_TRIANGLE,MDS_TRIANGLE,MDS_TRIANGLE,MDS_TRIANGLE};

static int const T01[] = {0,1,1,2,2,0
                         ,0,3,1,3,2,3};
static int const T10[] = {2,0,0,1,1,2,3,4};
static int const T12[] = {0,1,2
                         ,0,4,3
                         ,1,5,4
                         ,2,3,5};
static int const T21[] = {0,1
                         ,0,2
                         ,0,3
                         ,1,3
                         ,1,2
                         ,2,3};

static int const W0[] = {MDS_VERTEX,MDS_VERTEX,MDS_VERTEX
                        ,MDS_VERTEX,MDS_VERTEX,MDS_VERTEX};
static int const W1[] = {MDS_EDGE,MDS_EDGE,MDS_EDGE
                        ,MDS_EDGE,MDS_EDGE,MDS_EDGE
                        ,MDS_EDGE,MDS_EDGE,MDS_EDGE};
static int const W2[] = {MDS_TRIANGLE
                        ,MDS_QUADRILATERAL,MDS_QUADRILATERAL,MDS_QUADRILATERAL
                        ,MDS_TRIANGLE};

static int const W01[] = {0,1,1,2,2,0
                         ,0,3,1,4,2,5
                         ,3,4,4,5,5,3};
static int const W10[] = {0,2,0,1,1,2
                         ,6,8,6,7,7,8};
static int const W12[] = {0,1,2
                         ,0,4,6,3
                         ,1,4,7,5
                         ,2,3,8,5
                         ,6,7,8};
static int const W21[] = {0,1,0,2,0,3
                         ,1,3,1,2,2,3
                         ,1,4,2,4,3,4};

static int const P0[] = {MDS_VERTEX,MDS_VERTEX,MDS_VERTEX,MDS_VERTEX
                        ,MDS_VERTEX};
static int const P1[] = {MDS_EDGE,MDS_EDGE,MDS_EDGE,MDS_EDGE
                        ,MDS_EDGE,MDS_EDGE,MDS_EDGE,MDS_EDGE};
static int const P2[] = {MDS_QUADRILATERAL
                        ,MDS_TRIANGLE,MDS_TRIANGLE,MDS_TRIANGLE,MDS_TRIANGLE};

static int const P01[] = {0,1,1,2
                         ,2,3,3,0
                         ,0,4,1,4
                         ,2,4,3,4};
static int const P10[] = {0,3,0,1
                         ,1,2,2,3
                         ,4,5};
static int const P12[] = {0,1,2,3
                         ,0,5,4
                         ,1,6,5
                         ,2,7,6
                         ,3,4,7};
static int const P21[] = {0,1,0,2,0,3
                         ,0,4,1,4,1,2
                         ,2,3,3,4};

static int const H0[] = {MDS_VERTEX,MDS_VERTEX,MDS_VERTEX,MDS_VERTEX
                        ,MDS_VERTEX,MDS_VERTEX,MDS_VERTEX,MDS_VERTEX};
static int const H1[] = {MDS_EDGE,MDS_EDGE,MDS_EDGE,MDS_EDGE
                        ,MDS_EDGE,MDS_EDGE,MDS_EDGE,MDS_EDGE
                        ,MDS_EDGE,MDS_EDGE,MDS_EDGE,MDS_EDGE};
static int const H2[] = {MDS_QUADRILATERAL,MDS_QUADRILATERAL,MDS_QUADRILATERAL
                        ,MDS_QUADRILATERAL,MDS_QUADRILATERAL,MDS_QUADRILATERAL};

static int const H01[] = {0,1,1,2,2,3,3,0
                         ,0,4,1,5,2,6,3,7
                         ,4,5,5,6,6,7,7,4};
static int const H10[] = {0,3,0,1,1, 2,2, 3
                         ,4,8,5,9,6,10,7,11};
static int const H12[] = {3,2, 1, 0
                         ,0,5, 8, 4
                         ,1,6, 9, 5
                         ,2,7,10, 6
                         ,3,4,11, 7
                         ,8,9,10,11};
static int const H21[] = {0,1,0,2,0,3,0,4
                         ,1,4,1,2,2,3,3,4
                         ,1,5,2,5,3,5,4,5};

int const* mds_types[MDS_TYPES][4] =
{{0 ,0 ,0 ,0}
,{e0,0 ,0 ,0}
,{t0,t1,0 ,0}
,{q0,q1,0 ,0}
,{W0,W1,W2,0}
,{P0,P1,P2,0}
,{T0,T1,T2,0}
,{H0,H1,H2,0}
};
static int const* convs[MDS_TYPES][4][4] =
{{{0,0  ,0,0},{0  ,0,0  ,0},{0,0  ,0,0},{0,0,0,0}}
,{{0,0  ,0,0},{0  ,0,0  ,0},{0,0  ,0,0},{0,0,0,0}}
,{{0,t01,0,0},{t10,0,0  ,0},{0,0  ,0,0},{0,0,0,0}}
,{{0,q01,0,0},{q10,0,0  ,0},{0,0  ,0,0},{0,0,0,0}}
,{{0,W01,0,0},{W10,0,W12,0},{0,W21,0,0},{0,0,0,0}}
,{{0,P01,0,0},{P10,0,P12,0},{0,P21,0,0},{0,0,0,0}}
,{{0,T01,0,0},{T10,0,T12,0},{0,T21,0,0},{0,0,0,0}}
,{{0,H01,0,0},{H10,0,H12,0},{0,H21,0,0},{0,0,0,0}}
};

static void resize_down(struct mds* m, int from, int to,
    mds_id new_cap[MDS_TYPES])
{
  int t;
  int deg;
  for (t = 0; t < MDS_TYPES; ++t)
    if (mds_dim[t] == from) {
      deg = mds_degree[t][to];
      REALLOC(mds_id*,m->down[to][t],new_cap[t] * deg);
    }
}

static void resize_up(struct mds* m, int from, int to,
    mds_id old_cap[MDS_TYPES],
    mds_id new_cap[MDS_TYPES])
{
  int t;
  int deg;
  mds_id i;
  for (t = 0; t < MDS_TYPES; ++t) {
    if (mds_dim[t] == to) {
      deg = mds_degree[t][from];
      REALLOC(mds_id*,m->up[from][t],new_cap[t] * deg);
    } else if (mds_dim[t] == from) {
      REALLOC(mds_id*,m->first_up[to][t],new_cap[t]);
      for (i = old_cap[t]; i < new_cap[t]; ++i) {
        // PCU_ALWAYS_ASSERT(m->first_up[to][t]);
        m->first_up[to][t][i] = MDS_NONE;
      }
    }
  }
}

int mds_type(mds_id e)
{
  return TYPE(e);
}

mds_id mds_index(mds_id e)
{
  return INDEX(e);
}

mds_id mds_identify(int type, mds_id idx)
{
  return ID(type,idx);
}

static mds_id* at_id(mds_id* a[MDS_TYPES], mds_id x)
{
  return &(a[TYPE(x)][INDEX(x)]);
}

int smb2mds(int smb_type)
{
  int const table[SMB_TYPES] =
  {MDS_VERTEX
  ,MDS_EDGE
  ,MDS_TRIANGLE
  ,MDS_QUADRILATERAL
  ,MDS_HEXAHEDRON
  ,MDS_WEDGE
  ,MDS_PYRAMID
  ,MDS_TETRAHEDRON};
  return table[smb_type];
}

int mds2smb(int mds_type)
{
  int const table[MDS_TYPES] =
  {SMB_VERT
  ,SMB_EDGE
  ,SMB_TRI
  ,SMB_QUAD
  ,SMB_PRIS
  ,SMB_PYR
  ,SMB_TET
  ,SMB_HEX};
  return table[mds_type];
}

int down_degree(int t)
{
  return mds_degree[t][mds_dim[t] - 1];
}
