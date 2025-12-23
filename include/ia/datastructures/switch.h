#pragma once
/** @file ia/datastructures/switch.h
 *  @brief Interleaved linked list for storing mutually exclusive values.
 * 
 *  TODO docs 
 */
#include <ia/base/types.h>
#include <ia/datastructures/darray.h>
#include <ia/datastructures/map.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct ia_switch_node {
    u32 next, prev;
} ia_switch_node;

typedef struct ia_switch_page {
    ia_darray   nodes;  /**< darray<ia_switch_node> */
    ia_darray   values; /**< darray<u64> */
} ia_switch_page;

typedef struct ia_switch {
    ia_map      hdrs;
    ia_darray   pages;  /**< darray<ia_switch_page> */
} ia_switch;

#ifdef __cplusplus
}
#endif /* __cplusplus */
