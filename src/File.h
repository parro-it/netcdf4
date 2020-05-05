#ifndef NETCDF4JS_FILE_H
#define NETCDF4JS_FILE_H

#include "napi-utils.h"

namespace netcdf4js {

class Group;

class File {
  public:
	static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);
	static napi_value Init(napi_env env, napi_value exports);

  private:
	explicit File(const int &id_);
	~File();

	bool open(const char *filename, const int &mode, const int &format);
	static napi_value New(napi_env env, napi_callback_info info);
	static napi_value Close(napi_env env, napi_callback_info info);
	static napi_value Sync(napi_env env, napi_callback_info info);
	static napi_value Inspect(napi_env env, napi_callback_info info);
	int id;
	bool closed;
	static napi_ref constructor;
	napi_ref wrapper_;
	napi_env env_;
};
} // namespace netcdf4js

#endif
