#include "community_on_demand.h"
#include "modularity.h"
#include "stinger_core/stinger_error.h"

/*
 *  On-demand community detection
 *
 */
int64_t community_on_demand(const stinger_t * S, int64_t ** vertices, int64_t ** partitions, int64_t ** intsums, int64_t ** extsums) {
  if (*vertices != NULL || *partitions != NULL || *intsums != NULL || *extsums != NULL) {
    LOG_E("Community on demand output arrays should not be allocated before call.  Possible memory leak.");
  }

  int64_t * output_vertices = NULL;
  int64_t * output_partitions = NULL;
  int64_t * output_intsums = NULL;
  int64_t * output_extsums = NULL;

  /* Initialize to NULL in case of error */
  *vertices = output_vertices;
  *partitions = output_partitions;
  *intsums = output_intsums;
  *extsums = output_extsums;

  /* Allocate output array */
  int64_t nv = stinger_max_active_vertex(S) + 1;
  output_vertices = (int64_t *) xmalloc (nv * sizeof(int64_t));
  output_partitions = (int64_t *) xmalloc (nv * sizeof(int64_t));
  output_intsums = (int64_t *) xmalloc(nv * sizeof(int64_t));
  output_extsums = (int64_t *) xmalloc(nv * sizeof(int64_t));

  /* Initialize vertex numbers */
  int64_t v;
  for (v = 0; v < nv; v += 1) {
      output_vertices[v] = v;
  }

  /* Note wiring of max iterations to 5 */
  community_detection(S, nv, output_partitions, output_intsums, output_extsums, (int64_t) 5);

  /* Set output arrays */
  *vertices = output_vertices;
  *partitions = output_partitions;
  *intsums = output_intsums;
  *extsums = output_extsums;

  return nv;
}
