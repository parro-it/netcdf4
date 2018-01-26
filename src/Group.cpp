#include "Group.h"
#include <netcdf.h>
#include "Attribute.h"
#include "Dimension.h"
#include "Variable.h"
#include "netcdf4js.h"

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
    NODE_SET_PROTOTYPE_METHOD(tpl, "addVariable", Group::AddVariable);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addDimension", Group::AddDimension);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addSubgroup", Group::AddSubgroup);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addAttribute", Group::AddAttribute);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", Group::Inspect);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "id"), Group::GetId);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "variables"), Group::GetVariables);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "dimensions"), Group::GetDimensions);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "unlimited"), Group::GetUnlimited);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "attributes"), Group::GetAttributes);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "subgroups"), Group::GetSubgroups);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "name"), Group::GetName);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "fullname"), Group::GetFullname);
    constructor.Reset(isolate, tpl->GetFunction());
}

bool Group::get_name(char* name) const {
    call_netcdf_bool(nc_inq_grpname(id, name));
    return true;
}

void Group::AddAttribute(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
    if (args.Length() < 3) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    std::string type_str = *v8::String::Utf8Value(args[1]);
    int type = get_type(type_str);
    if (type == NC2_ERR) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown variable type")));
        return;
    }
    Attribute* res = new Attribute(*v8::String::Utf8Value(args[0]), NC_GLOBAL, obj->id, type);
    res->set_value(args[2]);
    args.GetReturnValue().Set(res->handle());
}

void Group::AddSubgroup(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
    if (args.Length() < 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    int new_id;
    call_netcdf(nc_def_grp(obj->id, *v8::String::Utf8Value(args[0]), &new_id));
    Group* res = new Group(new_id);
    args.GetReturnValue().Set(res->handle());
}

void Group::AddDimension(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
    if (args.Length() < 2) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    int len;
    if (std::string(*v8::String::Utf8Value(args[1])) == "unlimited") {
        len = NC_UNLIMITED;
    } else {
        if (!args[1]->IsUint32()) {
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a positive integer")));
            return;
        }
        len = args[1]->Uint32Value();
    }
    int new_id;
    call_netcdf(nc_def_dim(obj->id, *v8::String::Utf8Value(args[0]), len, &new_id));
    Dimension* res = new Dimension(new_id, obj->id);
    args.GetReturnValue().Set(res->handle());
}

void Group::AddVariable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
    if (args.Length() < 3) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments")));
        return;
    }
    std::string type_str = *v8::String::Utf8Value(args[1]);
    int type = get_type(type_str);
    if (type == NC2_ERR) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown variable type")));
        return;
    }
    if (type == NC_STRING) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unsupported variable type")));
        return;
    }
    if (!args[2]->IsArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting an array")));
        return;
    }
    v8::Local<v8::Object> array = args[2]->ToObject();
    size_t ndims = array->Get(v8::String::NewFromUtf8(isolate, "length"))->Uint32Value();
    int dimids[ndims];
    for (size_t i = 0; i < ndims; i++) {
        dimids[i] = array->Get(i)->Int32Value();
    }
    int new_id;
    call_netcdf(nc_def_var(obj->id, *v8::String::Utf8Value(args[0]), type, ndims, dimids, &new_id));
    Variable* res = new Variable(new_id, obj->id);
    args.GetReturnValue().Set(res->handle());
}

void Group::GetId(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->id));
}

void Group::GetVariables(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    int nvars;
    call_netcdf(nc_inq_varids(obj->id, &nvars, NULL));
    int var_ids[nvars];
    call_netcdf(nc_inq_varids(obj->id, NULL, var_ids));
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
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

void Group::GetDimensions(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    int ndims;
    call_netcdf(nc_inq_dimids(obj->id, &ndims, NULL, 0));
    int dim_ids[ndims];
    call_netcdf(nc_inq_dimids(obj->id, NULL, dim_ids, 0));
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
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

void Group::GetUnlimited(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    int ndims;
    call_netcdf(nc_inq_unlimdims(obj->id, &ndims, NULL));
    int dim_ids[ndims];
    call_netcdf(nc_inq_unlimdims(obj->id, NULL, dim_ids));
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
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
    char name[NC_MAX_NAME + 1];
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
    int grp_ids[ngrps];
    call_netcdf(nc_inq_grps(obj->id, NULL, grp_ids));
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
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

void Group::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    char name[NC_MAX_NAME + 1];
    if (obj->get_name(name)) {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name));
    }
}

void Group::GetFullname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    size_t len;
    call_netcdf(nc_inq_grpname_len(obj->id, &len));
    char name[len + 1];
    name[len] = 0;
    call_netcdf(nc_inq_grpname_full(obj->id, NULL, name));
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name));
}

void Group::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Group]"));
}
}  // namespace netcdf4js
