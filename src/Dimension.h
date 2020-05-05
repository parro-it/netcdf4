#ifndef NETCDF4JS_DIMENSION_H
#define NETCDF4JS_DIMENSION_H

#include "Dimension.h"
#include "napi-utils.h"

namespace netcdf4js {

class Dimension {
  public:
	static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);
	static napi_value Init(napi_env env, napi_value exports);
	Dimension(const int &id_, const int &parent_id_);
	bool get_name(char *name) const;
	static napi_value Build(napi_env env, int id, int parent_id);

  private:
	static napi_ref constructor;
	~Dimension();
	static napi_value New(napi_env env, napi_callback_info info);
	static napi_value GetId(napi_env env, napi_callback_info info);
	static napi_value GetLength(napi_env env, napi_callback_info info);
	static napi_value GetName(napi_env env, napi_callback_info info);
	static napi_value SetName(napi_env env, napi_callback_info info);
	static napi_value Inspect(napi_env env, napi_callback_info info);
	int id;
	int parent_id;
	napi_ref wrapper_;
	napi_env env_;
};
} // namespace netcdf4js

#endif
