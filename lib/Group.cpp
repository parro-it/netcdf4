#include "Group.h"
#include "Exceptions.h"
#include "Variable.h"
#include "Dimension.h"
#include "Attribute.h"
#include <netcdf.h>
#include <vector>

namespace netcdf4js {
    v8::Persistent<v8::Function> Group::constructor;

    Group::Group(const int& id_) : id(id_) {
        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance();
        Wrap(obj);
    }

    void Group::Init(v8::Local<v8::Object> exports) {
        v8::Isolate* isolate = exports->GetIsolate();

        v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
        tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Group"));
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "variables"), Group::GetVariables);
        tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "dimensions"), Group::GetDimensions);
        tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "attributes"), Group::GetAttributes);
        tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "subgroups"), Group::GetSubgroups);
        tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "name"), Group::GetName);

        constructor.Reset(isolate, tpl->GetFunction());
    }


    void Group::GetVariables(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Isolate* isolate = info.GetIsolate();
        Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
        int nvars;
        call_netcdf(nc_inq_varids(obj->id, &nvars, NULL));
        std::vector<int> var_ids(nvars);
        call_netcdf(nc_inq_varids(obj->id, NULL, &var_ids[0]));
        v8::Local<v8::Object> result = v8::Object::New(isolate);
        char name[NC_MAX_NAME+1];
        for (int var_id : var_ids) {
           Variable* v = new Variable(var_id, obj->id);
           if (v->get_name(name)) {
               result->Set(v8::String::NewFromUtf8(isolate, name), v->handle());
           } else {
               return;
           }
        }
        info.GetReturnValue().Set(result);
    }

    bool Group::get_name(char* name) const {
        call_netcdf_bool(nc_inq_grpname(id, name));
        return true;
    }

    void Group::GetDimensions(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Isolate* isolate = info.GetIsolate();
        Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
        int ndims;
        call_netcdf(nc_inq_dimids(obj->id, &ndims, NULL, 0));
        std::vector<int> dim_ids(ndims);
        call_netcdf(nc_inq_dimids(obj->id, NULL, &dim_ids[0], 0));
        v8::Local<v8::Object> result = v8::Object::New(isolate);
        char name[NC_MAX_NAME+1];
        for (int dim_id : dim_ids) {
           Dimension* d = new Dimension(dim_id, obj->id);
           if (d->get_name(name)) {
               result->Set(v8::String::NewFromUtf8(isolate, name), d->handle());
           } else {
               return;
           }
        }
        info.GetReturnValue().Set(result);
    }

    void Group::GetAttributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Isolate* isolate = info.GetIsolate();
        Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
        int natts;
        call_netcdf(nc_inq_natts(obj->id, &natts));
        v8::Local<v8::Object> result = v8::Object::New(isolate);
        char name[NC_MAX_NAME+1];
        for (int i = 0; i < natts; i++) {
            call_netcdf(nc_inq_attname(obj->id, NC_GLOBAL, i, name));
            Attribute* a = new Attribute(name, NC_GLOBAL, obj->id);
            result->Set(v8::String::NewFromUtf8(isolate, name), a->handle());
        }
        info.GetReturnValue().Set(result);
    }

    void Group::GetSubgroups(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
        v8::Isolate* isolate = info.GetIsolate();
        Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
        int ngrps;
        call_netcdf(nc_inq_grps(obj->id, &ngrps, NULL));
        std::vector<int> grp_ids(ngrps);
        call_netcdf(nc_inq_grps(obj->id, NULL, &grp_ids[0]));
        v8::Local<v8::Object> result = v8::Object::New(isolate);
        char name[NC_MAX_NAME+1];
        for (int grp_id : grp_ids) {
           Group* g = new Group(grp_id);
           if (g->get_name(name)) {
               result->Set(v8::String::NewFromUtf8(isolate, name), g->handle());
           } else {
               return;
           }
        }
        info.GetReturnValue().Set(result);
    }

    void Group::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info) {
        v8::Isolate* isolate = info.GetIsolate();
        Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
        char name[NC_MAX_NAME+1];
        if (obj->get_name(name)) {
            info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name));
        }
    }
}
