#include "Group.h"
#include "Exceptions.h"
#include "Variable.h"
#include <netcdf.h>

namespace netcdf4js {
    v8::Persistent<v8::Function> Group::constructor;

    Group::Group(const int& id_) : id(id_) {
        int nvars;
        call_netcdf(nc_inq_nvars(id, &nvars));
        std::vector<int> var_ids(nvars);
        call_netcdf(nc_inq_varids(id, nullptr, &var_ids[0]));
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::Local<v8::Object> local_v8_variables = v8::Object::New(isolate);
        for (int var_id : var_ids) {
           Variable* v = new Variable(var_id, id);
           variables.emplace_back(v);
           local_v8_variables->Set(v8::String::NewFromUtf8(isolate, v->get_name().c_str()), v->handle());
        }
        v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance();
        obj->Set(v8::String::NewFromUtf8(isolate, "variables"), local_v8_variables);
        Wrap(obj);
    }

    void Group::Init(v8::Local<v8::Object> exports) {
        v8::Isolate* isolate = exports->GetIsolate();

        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
        tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Group"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        constructor.Reset(isolate, tpl->GetFunction());
    }

}
