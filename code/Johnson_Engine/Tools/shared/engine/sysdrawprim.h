#ifdef __NULLREND
#include "sys/null/nulldrawprim.h"

#else

#ifdef __PS2REND
#include "sys/ps2/ps2drawprim.h"
#endif

#ifdef __OPENGLREND
#include "sys/opengl/opengldrawprim.h"
#endif

#ifdef __D3DREND
#include "sys/d3d/d3ddrawprim.h"
#endif

#endif
