#pragma once

#include <ia/foundation.h>

#ifdef IA_MODULE_AUDIO
#include <ia/modules/audio.h>
#endif /* IA_MODULE_AUDIO */

#ifdef IA_MODULE_ECS
#include <ia/modules/ecs.h>
#endif /* IA_MODULE_ECS */

#ifdef IA_MODULE_NET
#include <ia/modules/net.h>
#endif /* IA_MODULE_NET */

#ifdef IA_MODULE_PHYS
#include <ia/modules/phys.h>
#endif /* IA_MODULE_PHYS */

#ifdef IA_MODULE_RENDER
#include <ia/modules/render.h>
#endif /* IA_MODULE_RENDER */

#ifdef IA_MODULE_VIDEO
#include <ia/modules/video.h>
#endif /* IA_MODULE_VIDEO */

#ifdef IA_MODULE_XR
#include <ia/modules/xr.h>
#endif /* IA_MODULE_XR */

#ifdef IA_INSTRUMENT_IMGUI_TOOLS
#include <ia/instruments/imgui_tools.h>
#endif /* IA_INSTRUMENT_IMGUI_TOOLS */

#ifdef IA_INSTRUMENT_RENDER_GRAPH
#include <ia/instruments/render_commands.h>
#include <ia/instruments/render_graph.h>
#endif /* IA_INSTRUMENT_RENDER_GRAPH */

#ifdef IA_INSTRUMENT_SORCERESS
#include <ia/instruments/sorceress.h>
#endif /* IA_INSTRUMENT_SORCERESS */
