#ifndef NETCDF4JS_DIMENSION_H
#define NETCDF4JS_DIMENSION_H

#include <node.h>
#include <node_object_wrap.h>

namespace netcdf4js {

    class Dimension : public node::ObjectWrap {
    public:
        static void Init(v8::Local<v8::Object> exports);
        Dimension(const int& id_, const int& parent_id_);
        bool get_name(char* name) const;

    private:
        static v8::Persistent<v8::Function> constructor;
        static void GetLength(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
        int id;
        int parent_id;
    };
}

#endif
