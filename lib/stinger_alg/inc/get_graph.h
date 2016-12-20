#ifndef STINGER_GET_GRAPH_H
#define STINGER_GET_GRAPH_H

#include "stinger_core/stinger_atomics.h"
#include "stinger_core/stinger.h"
#include "stinger_core/xmalloc.h"

#ifdef __cplusplus
#define restrict
extern "C" {
#endif

void get_graph(const stinger_t * S, int64_t ** edge_src, int64_t ** edge_dst, int64_t ** edge_type, int64_t ** edge_wt, int64_t *nv, int64_t *ne);

#ifdef __cplusplus
}
#endif

#endif
