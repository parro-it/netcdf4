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
    v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    Wrap(obj);
}

void Group::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Group", v8::NewStringType::kNormal).ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addVariable", Group::AddVariable);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addDimension", Group::AddDimension);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addSubgroup", Group::AddSubgroup);
    NODE_SET_PROTOTYPE_METHOD(tpl, "addAttribute", Group::AddAttribute);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", Group::Inspect);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "id", v8::NewStringType::kNormal).ToLocalChecked(), Group::GetId);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "variables", v8::NewStringType::kNormal).ToLocalChecked(), Group::GetVariables);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "dimensions", v8::NewStringType::kNormal).ToLocalChecked(), Group::GetDimensions);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "unlimited", v8::NewStringType::kNormal).ToLocalChecked(), Group::GetUnlimited);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "attributes", v8::NewStringType::kNormal).ToLocalChecked(), Group::GetAttributes);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "subgroups", v8::NewStringType::kNormal).ToLocalChecked(), Group::GetSubgroups);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), Group::GetName);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "fullname", v8::NewStringType::kNormal).ToLocalChecked(), Group::GetFullname);
    constructor.Reset(isolate, tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}

bool Group::get_name(char* name) const {
    int retval = nc_inq_grpname(id, name);
    if (retval != NC_NOERR) {
        throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
        return false;
    }
    return true;
}

void Group::AddAttribute(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
    if (args.Length() < 3) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    std::string type_str = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        args[1]);
    int type = get_type(type_str);
    if (type == NC2_ERR) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown variable type", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    Attribute* res = new Attribute(*v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
                                       isolate,
#endif
                                       args[0]),
                                   NC_GLOBAL, obj->id, type);
    res->set_value(args[2]);
    args.GetReturnValue().Set(res->handle());
}

void Group::AddSubgroup(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
    if (args.Length() < 1) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int new_id;
    int retval = nc_def_grp(obj->id,
                            *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
                                isolate,
#endif
                                args[0]),
                            &new_id);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    Group* res = new Group(new_id);
    args.GetReturnValue().Set(res->handle());
}

void Group::AddDimension(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
    if (args.Length() < 2) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    int len;
    if (std::string(*v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
            isolate,
#endif
            args[1]))
        == "unlimited") {
        len = NC_UNLIMITED;
    } else {
        if (!args[1]->IsUint32()) {
            isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting a positive integer", v8::NewStringType::kNormal).ToLocalChecked()));
            return;
        }
        len = args[1]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
    }
    int new_id;
    int retval = nc_def_dim(obj->id,
                            *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
                                isolate,
#endif
                                args[0]),
                            len, &new_id);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    Dimension* res = new Dimension(new_id, obj->id);
    args.GetReturnValue().Set(res->handle());
}

void Group::AddVariable(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
    if (args.Length() < 3) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    std::string type_str = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        args[1]);
    int type = get_type(type_str);
    if (type == NC2_ERR) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown variable type", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (type == NC_STRING) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unsupported variable type", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    if (!args[2]->IsArray()) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting an array", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }
    v8::Local<v8::Object> array = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
    size_t ndims = array->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, "length", v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked()->Uint32Value(isolate->GetCurrentContext()).ToChecked();
    int* dimids = new int[ndims];
    for (size_t i = 0; i < ndims; i++) {
        dimids[i] = array->Get(isolate->GetCurrentContext(), i).ToLocalChecked()->Int32Value(isolate->GetCurrentContext()).ToChecked();
    }
    int new_id;
    int retval = nc_def_var(obj->id,
                            *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
                                isolate,
#endif
                                args[0]),
                            type, ndims, dimids, &new_id);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] dimids;
        return;
    }
    Variable* res = new Variable(new_id, obj->id);
    args.GetReturnValue().Set(res->handle());
    delete[] dimids;
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
    int retval = nc_inq_varids(obj->id, &nvars, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    int* var_ids = new int[nvars];
    retval = nc_inq_varids(obj->id, NULL, var_ids);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] var_ids;
        return;
    }
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
    for (int i = 0; i < nvars; ++i) {
        Variable* v = new Variable(var_ids[i], obj->id);
        if (v->get_name(name)) {
            result->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(), v->handle());
        } else {
            delete[] var_ids;
            return;
        }
    }
    info.GetReturnValue().Set(result);
    delete[] var_ids;
}

void Group::GetDimensions(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    int ndims;
    int retval = nc_inq_dimids(obj->id, &ndims, NULL, 0);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    int* dim_ids = new int[ndims];
    retval = nc_inq_dimids(obj->id, NULL, dim_ids, 0);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] dim_ids;
        return;
    }
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
    for (int i = 0; i < ndims; ++i) {
        Dimension* d = new Dimension(dim_ids[i], obj->id);
        if (d->get_name(name)) {
            result->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(), d->handle());
        } else {
            delete[] dim_ids;
            return;
        }
    }
    info.GetReturnValue().Set(result);
    delete[] dim_ids;
}

void Group::GetUnlimited(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    int ndims;
    int retval = nc_inq_unlimdims(obj->id, &ndims, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    int* dim_ids = new int[ndims];
    retval = nc_inq_unlimdims(obj->id, NULL, dim_ids);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] dim_ids;
        return;
    }
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
    for (int i = 0; i < ndims; ++i) {
        Dimension* d = new Dimension(dim_ids[i], obj->id);
        if (d->get_name(name)) {
            result->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(), d->handle());
        } else {
            delete[] dim_ids;
            return;
        }
    }
    info.GetReturnValue().Set(result);
    delete[] dim_ids;
}

void Group::GetAttributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    int natts;
    int retval = nc_inq_natts(obj->id, &natts);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
    for (int i = 0; i < natts; i++) {
        retval = nc_inq_attname(obj->id, NC_GLOBAL, i, name);
        if (retval != NC_NOERR) {
            throw_netcdf_error(isolate, retval);
            return;
        }
        Attribute* a = new Attribute(name, NC_GLOBAL, obj->id);
        result->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(), a->handle());
    }
    info.GetReturnValue().Set(result);
}

void Group::GetSubgroups(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    int ngrps;
    int retval = nc_inq_grps(obj->id, &ngrps, NULL);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    int* grp_ids = new int[ngrps];
    retval = nc_inq_grps(obj->id, NULL, grp_ids);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] grp_ids;
        return;
    }
    v8::Local<v8::Object> result = v8::Object::New(isolate);
    char name[NC_MAX_NAME + 1];
    for (int i = 0; i < ngrps; ++i) {
        Group* g = new Group(grp_ids[i]);
        if (g->get_name(name)) {
            result->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(), g->handle());
        } else {
            delete[] grp_ids;
            return;
        }
    }
    info.GetReturnValue().Set(result);
    delete[] grp_ids;
}

void Group::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    char name[NC_MAX_NAME + 1];
    if (obj->get_name(name)) {
        info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked());
    }
}

void Group::GetFullname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
    size_t len;
    int retval = nc_inq_grpname_len(obj->id, &len);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    char* name = new char[len + 1];
    name[len] = 0;
    retval = nc_inq_grpname_full(obj->id, NULL, name);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        delete[] name;
        return;
    }
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked());
    delete[] name;
}

void Group::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Group]", v8::NewStringType::kNormal).ToLocalChecked());
}
}  // namespace netcdf4js
