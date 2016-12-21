#include <assert.h>
#include "get_graph.h"
#include "stinger_core/stinger_error.h"
#include "stinger_core/stinger.h"

/*
 *  Return Stinger's graph.
 *
 */
int64_t get_graph(const stinger_t * S, int64_t ** edge_src, int64_t ** edge_dst, int64_t ** edge_type, int64_t ** edge_wt, int64_t *nv, int64_t *ne) {
    if (*edge_src != NULL || *edge_dst != NULL || *edge_type != NULL || *edge_wt != NULL) {
        LOG_E("get_graph output arrays should not be allocated before call.  Possible memory leak.");
    }

    int64_t * output_edge_src = NULL;
    int64_t * output_edge_dst = NULL;
    int64_t * output_edge_type = NULL;
    int64_t * output_edge_wt = NULL;

    /* Initialize to NULL in case of error */
    *edge_src = output_edge_src;
    *edge_dst = output_edge_dst;
    *edge_type = output_edge_type;
    *edge_wt = output_edge_wt;

    /* Allocate output arrays */
    *nv = stinger_max_active_vertex(S) + 1;
    int64_t ne_ubound = stinger_max_total_edges(S);
    output_edge_src = (int64_t *) xmalloc (ne_ubound * sizeof(int64_t));
    output_edge_dst = (int64_t *) xmalloc (ne_ubound * sizeof(int64_t));
    output_edge_type = (int64_t *) xmalloc(ne_ubound * sizeof(int64_t));
    output_edge_wt = (int64_t *) xmalloc(ne_ubound * sizeof(int64_t));

    /* Scan every vertex's out-star */
    int64_t v;
    int64_t e = 0;
    for (v = 0; v < *nv; v += 1) {
        STINGER_FORALL_OUT_EDGES_OF_VTX_BEGIN(S, v) {
            output_edge_src[e] = STINGER_EDGE_SOURCE;
            output_edge_dst[e] = STINGER_EDGE_DEST;
            output_edge_type[e] = STINGER_EDGE_TYPE;
            output_edge_wt[e] = STINGER_EDGE_WEIGHT;
            e += 1;
        } STINGER_FORALL_OUT_EDGES_OF_VTX_END();
    }
    *ne = e;

    /* Set output arrays */
    *edge_src = output_edge_src;
    *edge_dst = output_edge_dst;
    *edge_type = output_edge_type;
    *edge_wt = output_edge_wt;

    return (int64_t) 0;
}
