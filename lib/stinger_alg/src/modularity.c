//
// Created by jdeeb3 on 5/31/16.
//
#include "modularity.h"
#include <float.h>
#include "stinger_core/stinger.h"
/**
 * This method calculates Newman's Modularity for a pair of verticies
 * This works for directed, non-negative weighted graphs
 * m is the sum of the total weights in the graph
 */
double_t
modularity(stinger_t * S, int64_t vertex_a, int64_t vertex_b, double_t m){
    int64_t ka_out = 0;
    int64_t kb_in = 0;
    int64_t edge_between = 0;
    double_t modularity = 0;
    STINGER_FORALL_OUT_EDGES_OF_VTX_BEGIN(S, vertex_a){
                                ka_out += STINGER_EDGE_WEIGHT;
                                if (STINGER_EDGE_DEST == vertex_b){
                                    edge_between += STINGER_EDGE_WEIGHT;
                                }
                            }STINGER_FORALL_OUT_EDGES_OF_VTX_END();
    STINGER_FORALL_IN_EDGES_OF_VTX_BEGIN(S, vertex_b){
                                kb_in += STINGER_EDGE_WEIGHT;
                                if (STINGER_EDGE_SOURCE == vertex_a){
                                    edge_between += STINGER_EDGE_WEIGHT;
                                }
                            }STINGER_FORALL_IN_EDGES_OF_VTX_END();

    modularity = (edge_between/m) - ((double_t)(ka_out*kb_in)/pow(m,2));
    return modularity;

}

int64_t *
louvain_method(stinger_t * S, int64_t * partitions, int64_t size, int64_t m, int64_t maxIter) {
    int64_t num_moves;
    int64_t num_iter = 0;
    do {
        num_moves = 0;
        num_iter += 1;
        for (int64_t i = 0; i < size; i++) {
            double_t maxMod = -DBL_MAX;
            int64_t label = partitions[i];
            STINGER_FORALL_OUT_EDGES_OF_VTX_BEGIN(S, i)
                                    {
                                        double_t modul = modularity(S, i, STINGER_EDGE_DEST, m);
                                        if (modul > maxMod) {
                                            label = partitions[STINGER_EDGE_DEST];
                                            maxMod = modul;
                                        }
                                    }
            STINGER_FORALL_OUT_EDGES_OF_VTX_END();
            if (partitions[i] != label) {
                //we move the vertex
                partitions[i] = label;
                num_moves += 1;
            }
        }
    } while (num_moves > 0 && num_iter < maxIter);
    return partitions;
}

/**
 * Return the ratio of v's partition's internal to external edge weight sums
 */
double_t
int_ext_wt_ratio(stinger_t * S, int64_t NV, int64_t * partitions, double_t * parmod, int64_t v) {
    // Sum internal and external weights for each vertex in v's partition
    int64_t v_partition = partitions[v];
    double_t internal_wt = 0.0;
    double_t external_wt = 0.0;
    // for outward and...
    for (int64_t i = 0; i < NV; i += 1) if (partitions[i] == v_partition) {
        STINGER_FORALL_OUT_EDGES_OF_VTX_BEGIN(S, i)
        {
            if (partitions[STINGER_EDGE_DEST] == v_partition) {
                internal_wt += STINGER_EDGE_WEIGHT;
            } else {
                external_wt += STINGER_EDGE_WEIGHT;
            }
        }
        STINGER_FORALL_OUT_EDGES_OF_VTX_END();
    }
    // ...inward edges
    for (int64_t i = 0; i < NV; i += 1) if (partitions[i] == v_partition) {
        STINGER_FORALL_IN_EDGES_OF_VTX_BEGIN(S, i)
        {
            if (partitions[STINGER_EDGE_SOURCE] == v_partition) {
                internal_wt += STINGER_EDGE_WEIGHT;
            } else {
                external_wt += STINGER_EDGE_WEIGHT;
            }
        }
        STINGER_FORALL_IN_EDGES_OF_VTX_END();
    }

    if (external_wt > 0.0) {
        return internal_wt/external_wt;
    } else {
        return 0.0001;
    }
}

/**
 * This method returns the first level of louvain's method for community detection
 * This implementation is structured so that in future this can be adjusted to return the hierchical version of the
 * community assignment.
 * This implementation takes in the STINGER graph S, the total number of verticies, an array to hold the labels for the
 * partitions, and a the maximum number of iterations the algorithm will run for
 */
void
community_detection(stinger_t * S, int64_t NV, int64_t * partitions, double_t * parmod, int64_t maxIter) {
    // We need the sum of the total weights in the graph to calculate modularity
    double_t m = 0;
    STINGER_FORALL_EDGES_OF_ALL_TYPES_BEGIN(S) {
                            m += STINGER_EDGE_WEIGHT;
                        } STINGER_FORALL_EDGES_OF_ALL_TYPES_END();

    // Begin by setting each vertex to its own partiton, parmod[] to -1
    for (int64_t i = 0; i < NV; i++) {
        partitions[i] = i;
        parmod[i] = (double_t) -1.0;
    }

    // Assign each vertex to a community
    partitions = louvain_method(S, partitions, NV, m, maxIter);

    // Store the connectedness of each community in association with its vertices.
    for (int64_t i = 0; i < NV; i += 1) {
        if (parmod[i] < 0.0) {  // if we haven't already seen this vertex's partition
            double_t connectedness = int_ext_wt_ratio(S, NV, partitions, parmod, i);
            for (int64_t j = 0; j < NV; j += 1) if (partitions[j] == i) parmod[j] = connectedness;
        }
    }
}
