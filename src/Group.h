#ifndef NETCDF4JS_GROUP_H
#define NETCDF4JS_GROUP_H

#include "napi-utils.h"

namespace netcdf4js {

class Variable;

class Group {
  public:
    Group(const int& id);
    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value Build(napi_env env, int id);
    bool get_name(char* name) const;

  private:
    ~Group();
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value GetId(napi_env env, napi_callback_info info);
    static napi_value GetVariables(napi_env env, napi_callback_info info);
    static napi_value GetDimensions(napi_env env, napi_callback_info info);
    static napi_value GetUnlimited(napi_env env, napi_callback_info info);
    static napi_value GetAttributes(napi_env env, napi_callback_info info);
    static napi_value GetSubgroups(napi_env env, napi_callback_info info);
    static napi_value GetName(napi_env env, napi_callback_info info);
    static napi_value GetFullname(napi_env env, napi_callback_info info);
    static napi_value AddAttribute(napi_env env, napi_callback_info info);
    static napi_value AddDimension(napi_env env, napi_callback_info info);
    static napi_value AddSubgroup(napi_env env, napi_callback_info info);
    static napi_value AddVariable(napi_env env, napi_callback_info info);
    static napi_value Inspect(napi_env env, napi_callback_info info);
    int id;
    static napi_ref constructor;
    napi_ref wrapper_;
    napi_env env_;

};
}  // namespace netcdf4js

#endif
