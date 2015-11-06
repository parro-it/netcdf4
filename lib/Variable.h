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
        static void GetDimensions(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void GetAttributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
        static void ReadSlice(const v8::FunctionCallbackInfo<v8::Value>& args);
        // TODO read whole data
        static v8::Persistent<v8::Function> constructor;
        static const unsigned char type_sizes[];
        int id;
        int parent_id;
        nc_type type;
    };
}

#endif
