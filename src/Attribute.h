#ifndef NETCDF4JS_ATTRIBUTE_H
#define NETCDF4JS_ATTRIBUTE_H

#include <string>
#include "napi-utils.h"

namespace netcdf4js {

class Attribute {
  public:
	static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);
	static napi_value Init(napi_env env, napi_value exports);
	Attribute(const char *name_, int var_id_, int parent_id_);
	Attribute(const char *name_, int var_id_, int parent_id_, int type_);
	static napi_value Build(napi_env env, char *name, int id, int parentId, int type);

  private:
	static napi_ref constructor;
	~Attribute();
	static napi_value New(napi_env env, napi_callback_info info);
	// static napi_value Delete(napi_env env, napi_callback_info info);
	static napi_value GetName(napi_env env, napi_callback_info info);
	static napi_value SetName(napi_env env, napi_callback_info info);
	static napi_value GetValue(napi_env env, napi_callback_info info);
	static napi_value SetValue(napi_env env, napi_callback_info info);
	static napi_value Inspect(napi_env env, napi_callback_info info);
	std::string name;
	int var_id;
	int parent_id;
	int type;
	napi_ref wrapper_;
	napi_env env_;
};
} // namespace netcdf4js

#endif
