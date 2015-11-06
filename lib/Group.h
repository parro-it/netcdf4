#ifndef NETCDF4JS_GROUP_H
#define NETCDF4JS_GROUP_H

#include <node.h>
#include <node_object_wrap.h>

namespace netcdf4js {
    class Variable;

    class Group : public node::ObjectWrap {
    public:
        Group(const int& id);
        static void Init(v8::Local<v8::Object> exports);
        bool get_name(char* name) const;

    private:
        static v8::Persistent<v8::Function> constructor;
        static void GetVariables(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void GetDimensions(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void GetAttributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void GetSubgroups(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
        static void SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info);
        int id;
    };
}

#endif
