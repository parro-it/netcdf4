#ifndef NETCDF4JS_VARIABLE_H
#define NETCDF4JS_VARIABLE_H

#include <netcdf.h>
#include "napi-utils.h"

namespace netcdf4js {

class Variable {
  public:
    static void Destructor(napi_env env, void* nativeObject, void* finalize_hint);
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value Build(napi_env env, int id, int parent_id);
    Variable(const int& id_, const int& parent_id_);
    bool get_name(char* name) const;

  private:
    ~Variable();
    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value Read(napi_env env, napi_callback_info info);
    static napi_value ReadSlice(napi_env env, napi_callback_info info);
    static napi_value ReadStridedSlice(napi_env env, napi_callback_info info);
    static napi_value Write(napi_env env, napi_callback_info info);
    static napi_value WriteSlice(napi_env env, napi_callback_info info);
    static napi_value WriteStridedSlice(napi_env env, napi_callback_info info);
    static napi_value AddAttribute(napi_env env, napi_callback_info info);
    static napi_value GetId(napi_env env, napi_callback_info info);
    static napi_value GetType(napi_env env, napi_callback_info info);
    static napi_value GetDimensions(napi_env env, napi_callback_info info);
    static napi_value GetAttributes(napi_env env, napi_callback_info info);
    static napi_value GetName(napi_env env, napi_callback_info info);
    static napi_value SetName(napi_env env, napi_callback_info info);
    static napi_value GetEndianness(napi_env env, napi_callback_info info);
    static napi_value SetEndianness(napi_env env, napi_callback_info info);
    static napi_value GetChecksumMode(napi_env env, napi_callback_info info);
    static napi_value SetChecksumMode(napi_env env, napi_callback_info info);
    static napi_value GetChunkMode(napi_env env, napi_callback_info info);
    static napi_value SetChunkMode(napi_env env, napi_callback_info info);
    static napi_value GetChunkSizes(napi_env env, napi_callback_info info);
    static napi_value SetChunkSizes(napi_env env, napi_callback_info info);
    static napi_value GetFillMode(napi_env env, napi_callback_info info);
    static napi_value SetFillMode(napi_env env, napi_callback_info info);
    static napi_value GetFillValue(napi_env env, napi_callback_info info);
    static napi_value SetFillValue(napi_env env, napi_callback_info info);
    static napi_value GetCompressionShuffle(napi_env env, napi_callback_info info);
    static napi_value SetCompressionShuffle(napi_env env, napi_callback_info info);
    static napi_value GetCompressionDeflate(napi_env env, napi_callback_info info);
    static napi_value SetCompressionDeflate(napi_env env, napi_callback_info info);
    static napi_value GetCompressionLevel(napi_env env, napi_callback_info info);
    static napi_value SetCompressionLevel(napi_env env, napi_callback_info info);
    static napi_value Inspect(napi_env env, napi_callback_info info);


    static napi_ref constructor;
    napi_ref wrapper_;
    napi_env env_;
    static const char* type_names[];
    int id;
    int parent_id;
    nc_type type;
    int ndims;
};
}  // namespace netcdf4js

#endif
