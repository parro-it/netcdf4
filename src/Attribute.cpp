#include "Attribute.h"
#include <inttypes.h>
#include <netcdf.h>
#include <iostream>
#include "netcdf4js.h"

namespace netcdf4js {

v8::Persistent<v8::Function> Attribute::constructor;

Attribute::Attribute(const char* name_, int var_id_, int parent_id_) : name(name_), var_id(var_id_), parent_id(parent_id_) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    Wrap(obj);
    int retval = nc_inq_atttype(parent_id, var_id_, name_, &type);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

Attribute::Attribute(const char* name_, int var_id_, int parent_id_, int type_) : name(name_), var_id(var_id_), parent_id(parent_id_), type(type_) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    v8::Local<v8::Object> obj = v8::Local<v8::Function>::New(isolate, constructor)->NewInstance(isolate->GetCurrentContext()).ToLocalChecked();
    Wrap(obj);
}

void Attribute::Init(v8::Local<v8::Object> exports) {
    v8::Isolate* isolate = exports->GetIsolate();
    v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(isolate);
    tpl->SetClassName(v8::String::NewFromUtf8(isolate, "Attribute", v8::NewStringType::kNormal).ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    NODE_SET_PROTOTYPE_METHOD(tpl, "delete", Attribute::Delete);
    NODE_SET_PROTOTYPE_METHOD(tpl, "inspect", Attribute::Inspect);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "name", v8::NewStringType::kNormal).ToLocalChecked(), Attribute::GetName, Attribute::SetName);
    tpl->InstanceTemplate()->SetAccessor(v8::String::NewFromUtf8(isolate, "value", v8::NewStringType::kNormal).ToLocalChecked(), Attribute::GetValue, Attribute::SetValue);
    constructor.Reset(isolate, tpl->GetFunction(isolate->GetCurrentContext()).ToLocalChecked());
}

void Attribute::GetName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(info.Holder());
    info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, obj->name.c_str(), v8::NewStringType::kNormal).ToLocalChecked());
}

void Attribute::SetName(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(info.Holder());
    v8::String::Utf8Value new_name_(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int retval = nc_rename_att(obj->parent_id, obj->var_id, obj->name.c_str(), *new_name_);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    obj->name = *new_name_;
}

void Attribute::GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::Isolate* isolate = info.GetIsolate();
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(info.Holder());

    if ((obj->type < NC_BYTE || obj->type > NC_UINT64) && obj->type != NC_STRING) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    size_t len;
    int retval = nc_inq_attlen(obj->parent_id, obj->var_id, obj->name.c_str(), &len);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }

    switch (obj->type) {
        case NC_BYTE: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(int8_t));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<int8_t *>(v->Data())[0]));
            } else {
                info.GetReturnValue().Set(v8::Int8Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_SHORT: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(int16_t));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<int16_t *>(v->Data())[0]));
            } else {
                info.GetReturnValue().Set(v8::Int16Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_INT: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(int32_t));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<int32_t *>(v->Data())[0]));
            } else {
                info.GetReturnValue().Set(v8::Int32Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_FLOAT: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(float));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Number::New(isolate, static_cast<float *>(v->Data())[0]));
            } else {
                info.GetReturnValue().Set(v8::Float32Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_DOUBLE: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(double));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Number::New(isolate, static_cast<double *>(v->Data())[0]));
            } else {
                info.GetReturnValue().Set(v8::Float64Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_UBYTE: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(uint8_t));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<uint8_t *>(v->Data())[0]));
            } else {
                info.GetReturnValue().Set(v8::Uint8Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_USHORT: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(uint16_t));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<uint16_t *>(v->Data())[0]));
            } else {
                info.GetReturnValue().Set(v8::Uint16Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_UINT: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(uint32_t));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate, static_cast<uint32_t *>(v->Data())[0]));
            } else {
                info.GetReturnValue().Set(v8::Uint32Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_INT64: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(int64_t));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<int64_t *>(v->Data())[0]));
            } else {
                // TODO: Should return BigInt64Array
                info.GetReturnValue().Set(v8::Int32Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_UINT64: {
            std::unique_ptr<v8::BackingStore> v = v8::ArrayBuffer::NewBackingStore(isolate, len * sizeof(uint64_t));
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v->Data());
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate, static_cast<uint64_t *>(v->Data())[0]));
            } else {
                // TODO: Should return BigInt64Array
                info.GetReturnValue().Set(v8::Uint32Array::New(v8::ArrayBuffer::New(isolate, std::move(v)), 0, len));
            }
        } break;
        case NC_CHAR:
        case NC_STRING: {
            char* v = new char[len + 1];
            v[len] = 0;
            retval = nc_get_att_text(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, v, v8::NewStringType::kNormal).ToLocalChecked());
            delete[] v;
        } break;
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Attribute::SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> val, const v8::PropertyCallbackInfo<void>& info) {
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(info.Holder());
    obj->set_value(val);
}

void Attribute::set_value(const v8::Local<v8::Value>& val) {
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    if ((type < NC_BYTE || type > NC_UINT) && type != NC_STRING) {
        isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Variable type not supported yet", v8::NewStringType::kNormal).ToLocalChecked()));
        return;
    }

    int retval;
    if (val->IsUint32()) {
        uint32_t v = val->Uint32Value(isolate->GetCurrentContext()).ToChecked();
        retval = nc_put_att(parent_id, var_id, name.c_str(), NC_UINT, 1, &v);
    } else if (val->IsInt32()) {
        int32_t v = val->Int32Value(isolate->GetCurrentContext()).ToChecked();
        retval = nc_put_att(parent_id, var_id, name.c_str(), NC_INT, 1, &v);
    } else if (val->IsNumber()) {
        double v = val->NumberValue(isolate->GetCurrentContext()).ToChecked();
        retval = nc_put_att(parent_id, var_id, name.c_str(), NC_DOUBLE, 1, &v);
    } else {
        std::string v = *v8::String::Utf8Value(
#if NODE_MAJOR_VERSION >= 8
            isolate,
#endif
            val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
        retval = nc_put_att_text(parent_id, var_id, name.c_str(), v.length(), v.c_str());
    }
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
    }
}

void Attribute::Delete(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(args.Holder());
    int retval = nc_del_att(obj->parent_id, obj->var_id, obj->name.c_str());
    if (retval != NC_NOERR) {
        throw_netcdf_error(args.GetIsolate(), retval);
    }
}

void Attribute::Inspect(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, "[object Attribute]", v8::NewStringType::kNormal).ToLocalChecked());
}
}  // namespace netcdf4js
