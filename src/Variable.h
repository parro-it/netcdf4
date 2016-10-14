#ifndef NETCDF4JS_VARIABLE_H
#define NETCDF4JS_VARIABLE_H

#include <netcdf.h>
#include <node.h>
#include <node_object_wrap.h>

namespace netcdf4js {

class Variable : public node::ObjectWrap {
   public:
    static void Init(v8::Local<v8::Object> exports);
    Variable(const int& id_, const int& parent_id_);
    bool get_name(char* name) const;

   private:
    static void Read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ReadSlice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void Write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void WriteSlice(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AddAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetDimensions(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetAttributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetEndianness(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEndianness(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetChecksumMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetChecksumMode(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetChunkMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetChunkMode(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetChunkSizes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetChunkSizes(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetFillMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFillMode(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetFillValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFillValue(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetCompressionShuffle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetCompressionShuffle(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetCompressionDeflate(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetCompressionDeflate(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void GetCompressionLevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetCompressionLevel(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
    static void Inspect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Persistent<v8::Function> constructor;
    static const unsigned char type_sizes[];
    static const char* type_names[];
    int id;
    int parent_id;
    nc_type type;
    int ndims;
};
}

#endif
