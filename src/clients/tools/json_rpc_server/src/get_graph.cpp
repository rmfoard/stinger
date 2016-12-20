#include "json_rpc_server.h"
#include "json_rpc.h"
#include "stinger_core/stinger_atomics.h"
#include "stinger_core/xmalloc.h"
#include "rapidjson/document.h"

#define LOG_AT_W  /* warning only */
#include "stinger_core/stinger_error.h"
extern "C" {
  #include "stinger_alg/get_graph.h"
}

using namespace gt::stinger;

int64_t 
JSON_RPC_get_graph::operator()(rapidjson::Value * params, rapidjson::Value & result, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> & allocator)
{
    bool strings;

    rpc_params_t p[] = {
        {"strings", TYPE_BOOL, &strings, true, 0},
        {NULL, TYPE_NONE, NULL, false, 0}
    };

    if (!contains_params(p, params)) {
        return json_rpc_error(-32602, result, allocator);
    }

    stinger_t * S = server_state->get_stinger();
    if (!S) {
        LOG_E ("STINGER pointer is invalid");
        return json_rpc_error(-32603, result, allocator);
    }

    rapidjson::Value vtx_phys;
    rapidjson::Value e_src(rapidjson::kArrayType);
    rapidjson::Value e_dst(rapidjson::kArrayType);
    rapidjson::Value e_type(rapidjson::kArrayType);
    rapidjson::Value e_wt(rapidjson::kArrayType);
    rapidjson::Value vtx_str(rapidjson::kArrayType);

    int64_t * edge_src = NULL;
    int64_t * edge_dst = NULL;
    int64_t * edge_type = NULL;
    int64_t * edge_wt = NULL;
    int64_t nv;
    int64_t ne;

    get_graph(S, &edge_src, &edge_dst, &edge_type, &edge_wt, &nv, &ne);

    if (strings) for (int64_t i = 0; i < nv; i += 1) {
        char * physID;
        uint64_t len;
        if(-1 == stinger_mapping_physid_direct(S, i, &physID, &len)) {
            physID = (char *) "";
            len = 0;
        }
        vtx_phys.SetString(physID, len, allocator);
        vtx_str.PushBack(vtx_phys, allocator);
    }

    for (int64_t j = 0; j < ne; j += 1) {
        e_src.PushBack(edge_src[j], allocator);
        e_dst.PushBack(edge_dst[j], allocator);
        e_type.PushBack(edge_type[j], allocator);
        e_wt.PushBack(edge_wt[j], allocator);
    }

    if (strings)
        result.AddMember("vertex_str", vtx_str, allocator);

    result.AddMember("edge_src", e_src, allocator);
    result.AddMember("edge_dst", e_dst, allocator);
    result.AddMember("edge_type", e_type, allocator);
    result.AddMember("edge_wt", e_wt, allocator);

    if (edge_src != NULL) {
        xfree(edge_src);
    }

    if (edge_dst != NULL) {
        xfree(edge_dst);
    }

    if (edge_type != NULL) {
        xfree(edge_type);
    }

    if (edge_wt != NULL) {
        xfree(edge_wt);
    }

    return 0;
}
