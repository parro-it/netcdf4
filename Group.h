#ifndef NETCDF4JS_GROUP_H
#define NETCDF4JS_GROUP_H

#include <node.h>
#include <node_object_wrap.h>
#include <vector>
#include <memory>

namespace netcdf4js {
    class Variable;

    class Group : public node::ObjectWrap {
    public:
        Group(const int& id);
        static void Init(v8::Local<v8::Object> exports);

    private:
        static v8::Persistent<v8::Function> constructor;
        int id;
        std::vector<std::unique_ptr<Variable>> variables;
        // TODO subgroups
        // TODO attributes
        // TODO dimensions
    };
}

#endif
