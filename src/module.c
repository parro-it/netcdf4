#include "napi-utils.h"

napi_value Attribute_Init(napi_env env, napi_value exports);
napi_value File_Init(napi_env env, napi_value exports);
napi_value Group_Init(napi_env env, napi_value exports);
napi_value Variable_Init(napi_env env, napi_value exports);
napi_value Dimension_Init(napi_env env, napi_value exports);

NUT_DEF_FN(test) {
	return NULL;
}

NUT_MODULE(netcdf4_napi, {
	NUT_FN(test);
	Attribute_Init(env, exports);
	File_Init(env, exports);
	Group_Init(env, exports);
	Variable_Init(env, exports);
	Dimension_Init(env, exports);
})
