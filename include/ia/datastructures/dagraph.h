#pragma once
/** @file ia/datastructures/dagraph.h
 *  @brief Directed acyclic graph optimized for parallelism.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>
#include <ia/base/atomic.h>
#include <ia/base/work.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef u32 ia_dagraph_id;

typedef struct ia_dagraph {
    i32             node_count, edge_count;
    /* mostly immutable */
    ia_dagraph_id  *edge_src;       /**< [edge_count] */
    ia_dagraph_id  *edge_dst;       /**< [edge_count] */
    ia_dagraph_id  *first_out_edge; /**< [node_count + 1] (compressed sparse row) */
    ia_dagraph_id  *topo_order;     /**< [node_count] (topologically sorted execution order) */
    ia_dagraph_id  *level;          /**< [node_count] (topological depth) */
    /* parallel runtime state */
    atomic_i32     *pending_deps;   /**< [node_count] */
    atomic_u32     *epoch;          /**< [node_count] */
    /** User payload [node_count]. */
    void          **v;
    /** Execution hooks [node_count]. */
    ia_work_fn     *work;
} ia_dagraph;

#ifdef __cplusplus
}
#endif /* __cplusplus */
