#include <napi.h>
#include "netcdf4js.h"

using namespace netcdf4js;

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	File::Init(env, exports);
	Group::Init(env, exports);
	Dimension::Init(env, exports);
	Attribute::Init(env, exports);
	Variable::Init(env, exports);
	return exports;
}

NODE_API_MODULE(hello, Init)
