#include <napi.h>
#include "netcdf4js.h"
#include <netcdf_meta.h>

using namespace netcdf4js;

Napi::Object Init(Napi::Env env, Napi::Object exports) {
	Napi::Object version=Napi::Object::New(env);
	version.Set("major",NC_VERSION_MAJOR);
	version.Set("minor",NC_VERSION_MINOR);
	version.Set("patch",NC_VERSION_PATCH);
	version.Set("version",NC_VERSION);
	exports.Set("version",version);
	File::Init(env, exports);
	Group::Init(env, exports);
	Dimension::Init(env, exports);
	Attribute::Init(env, exports);
	Variable::Init(env, exports);
	return exports;
}

NODE_API_MODULE(hello, Init)
