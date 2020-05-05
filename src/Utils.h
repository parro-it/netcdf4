#ifndef NETCDF4JS_UTILS_H
#define NETCDF4JS_UTILS_H
#include <node_api.h>

namespace netcdf4js {

extern const unsigned char type_sizes[];

napi_typedarray_type typedarray_cons_per_type(napi_env env, int type);
} // namespace netcdf4js

#endif
