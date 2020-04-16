#include "Attribute.h"
#include <inttypes.h>
#include <netcdf.h>
#include <iostream>
#include "napi-utils.h"

namespace netcdf4js {

napi_ref Attribute::constructor;

napi_value Attribute::New(napi_env env, napi_callback_info info) {
    ARGS(4, STR(_name), I32(_var_id), I32(_parent_id), I32(_type))

    napi_value target;
    NAPI_CALL(napi_get_new_target(env, info, &target));

    Attribute* obj = new Attribute(_name, _var_id, _parent_id, _type);
    obj->env_ = env;
    NAPI_CALL(napi_wrap(env,
        jsthis,
        reinterpret_cast<void*>(obj),
        Attribute::Destructor,
        nullptr,  // finalize_hint
        &obj->wrapper_
    ));

    return jsthis;
}


napi_value Attribute::Inspect(napi_env env, napi_callback_info info) {
    ARGS(0)
    RETURN_STR((char*)"[object Attribute]");
}

Attribute::~Attribute() { napi_delete_reference(env_, wrapper_); }

void Attribute::Destructor(napi_env env, void* nativeObject, void* finalize_hint) {
  reinterpret_cast<Attribute*>(nativeObject)->~Attribute();
}

Attribute::Attribute(const char* name_, int var_id_, int parent_id_) : name(name_), var_id(var_id_), parent_id(parent_id_) {

}

Attribute::Attribute(const char* name_, int var_id_, int parent_id_, int type_) : name(name_), var_id(var_id_), parent_id(parent_id_), type(type_) {

}

extern "C" {
    napi_value Attribute_Init(napi_env env, napi_value exports) {
        return Attribute::Init(env, exports);
    }
}

napi_value Attribute::Init(napi_env env, napi_value exports) {
    napi_property_descriptor properties[] = {
      //{ "value", 0, 0, GetValue, SetValue, 0, napi_default, 0 },
      { "name", 0, 0, GetName, SetName, 0, napi_default, 0 },
      //DECLARE_NAPI_METHOD("delete", Delete),
      DECLARE_NAPI_METHOD("inspect", Inspect),
    };

    napi_value constructor;
    NAPI_CALL(napi_define_class(
        env,
        "Attribute", NAPI_AUTO_LENGTH,
        Attribute::New,
        nullptr,
        2,
        properties,
        &constructor
    ));
    NAPI_CALL(napi_set_named_property(
        env,
        exports,
        "Attribute",
        constructor
    ));
    return nullptr;
}

napi_value Attribute::GetName(napi_env env, napi_callback_info info) {
    ARGS(0)
    Attribute* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));

    RETURN_STR((char*)self->name.c_str());
}

napi_value Attribute::SetName(napi_env env, napi_callback_info info) {
    ARGS(1, STR(new_name))
    Attribute* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));

    NC_CALL(nc_rename_att(self->parent_id, self->var_id, self->name.c_str(), new_name));
    self->name = new_name;

    return NULL;
}

} // end namespace

/*
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
            int8_t* v = new int8_t[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, v[0]));
            } else {
                info.GetReturnValue().Set(v8::Int8Array::New(v8::ArrayBuffer::New(isolate, v, len * 1), 0, len));
            }
            delete[] v;
        } break;
        case NC_SHORT: {
            int16_t* v = new int16_t[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, v[0]));
            } else {
                info.GetReturnValue().Set(v8::Int16Array::New(v8::ArrayBuffer::New(isolate, v, len * 2), 0, len));
            }
            delete[] v;
        } break;
        case NC_INT: {
            int32_t* v = new int32_t[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, v[0]));
            } else {
                info.GetReturnValue().Set(v8::Int32Array::New(v8::ArrayBuffer::New(isolate, v, len * 4), 0, len));
            }
            delete[] v;
        } break;
        case NC_FLOAT: {
            float* v = new float[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Number::New(isolate, v[0]));
            } else {
                info.GetReturnValue().Set(v8::Float32Array::New(v8::ArrayBuffer::New(isolate, v, len * 4), 0, len));
            }
            delete[] v;
        } break;
        case NC_DOUBLE: {
            double* v = new double[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Number::New(isolate, v[0]));
            } else {
                info.GetReturnValue().Set(v8::Float64Array::New(v8::ArrayBuffer::New(isolate, v, len * 8), 0, len));
            }
            delete[] v;
        } break;
        case NC_UBYTE: {
            uint8_t* v = new uint8_t[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, v[0]));
            } else {
                info.GetReturnValue().Set(v8::Uint8Array::New(v8::ArrayBuffer::New(isolate, v, len * 1), 0, len));
            }
            delete[] v;
        } break;
        case NC_USHORT: {
            uint16_t* v = new uint16_t[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, v[0]));
            } else {
                info.GetReturnValue().Set(v8::Uint16Array::New(v8::ArrayBuffer::New(isolate, v, len * 2), 0, len));
            }
            delete[] v;
        } break;
        case NC_UINT: {
            uint32_t* v = new uint32_t[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate, v[0]));
            } else {
                info.GetReturnValue().Set(v8::Uint32Array::New(v8::ArrayBuffer::New(isolate, v, len * 4), 0, len));
            }
            delete[] v;
        } break;
        case NC_INT64: {
            int64_t* v = new int64_t[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::New(isolate, static_cast<int32_t>(v[0])));
            } else {
                info.GetReturnValue().Set(v8::Int32Array::New(v8::ArrayBuffer::New(isolate, v, len * 8), 0, len));
            }
            delete[] v;
        } break;
        case NC_UINT64: {
            uint64_t* v = new uint64_t[len];
            retval = nc_get_att(obj->parent_id, obj->var_id, obj->name.c_str(), v);
            if (len == 1) {
                info.GetReturnValue().Set(v8::Integer::NewFromUnsigned(isolate, static_cast<uint32_t>(v[0])));
            } else {
                info.GetReturnValue().Set(v8::Uint32Array::New(v8::ArrayBuffer::New(isolate, v, len * 8), 0, len));
            }
            delete[] v;
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
*/
