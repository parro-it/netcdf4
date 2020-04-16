#include "napi-utils.h"

napi_value Attribute_Init(napi_env env, napi_value exports);
napi_value File_Init(napi_env env, napi_value exports);

NUT_DEF_FN(test) {
    return NULL;
}

NUT_MODULE(netcdf4_napi, {
    NUT_FN(test);
    Attribute_Init(env, exports);
    File_Init(env, exports);
})
