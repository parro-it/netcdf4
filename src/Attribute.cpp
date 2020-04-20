#include "Attribute.h"
#include "Utils.h"
#include <inttypes.h>
#include <netcdf.h>
#include <iostream>
#include "napi-utils.h"
#include "string.h"

namespace netcdf4js {

napi_ref Attribute::constructor;

napi_value Attribute::New(napi_env env, napi_callback_info info) {
    OPTIONAL_ARGS(1, 4,  STR(_name), I32(_var_id), I32(_parent_id), I32(_type))

    napi_value target;
    NAPI_CALL(napi_get_new_target(env, info, &target));

    Attribute* self;
    if (argc == 4) {
        self = new Attribute(_name, _var_id, _parent_id, _type);
    } else {
        self = new Attribute(_name, _var_id, _parent_id);
    }

    self->env_ = env;
    NAPI_CALL(napi_wrap(env,
        jsthis,
        reinterpret_cast<void*>(self),
        Attribute::Destructor,
        nullptr,  // finalize_hint
        &self->wrapper_
    ));

    return jsthis;
}

napi_value Attribute::Build(napi_env env, char* name, int id, int parent_id, int type) {
    napi_value attr_js;
    char* nuts_error = NULL;

    VAR_JS_FROM_STR(name_js, name);
    VAR_JS_FROM_I32(id_js, id);
    VAR_JS_FROM_I32(parent_id_js, parent_id);
    VAR_JS_FROM_I32(type_js, type);

    napi_value cons;

    NAPI_CALL(napi_get_reference_value(env, constructor, &cons));
    napi_value args[] = {name_js, id_js, parent_id_js, type_js};

    NAPI_CALL(napi_new_instance(env, cons, type == -1 ? 3 : 4, args ,&attr_js));

    return attr_js;
}

napi_value Attribute::Inspect(napi_env env, napi_callback_info info) {
    OPTIONAL_ARGS(5, 5)
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
      { "value", 0, 0, GetValue, SetValue, 0, napi_default, 0 },
      { "name", 0, 0, GetName, SetName, 0, napi_default, 0 },
      //DECLARE_NAPI_METHOD("delete", Delete),
      DECLARE_NAPI_METHOD("inspect", Inspect),
    };

    napi_value cons;
    NAPI_CALL(napi_define_class(
        env,
        "Attribute", NAPI_AUTO_LENGTH,
        Attribute::New,
        nullptr,
        3,
        properties,
        &cons
    ));

    NAPI_CALL(napi_set_named_property(
        env,
        exports,
        "Attribute",
        cons
    ));

    NAPI_CALL(napi_create_reference(
        env,
        cons,
        0,
        &Attribute::constructor
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


napi_value Attribute::GetValue(napi_env env, napi_callback_info info) {
    ARGS(0)
    Attribute* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));

    if ((self->type < NC_BYTE || self->type > NC_UINT64) && self->type != NC_STRING) {
        napi_throw_error(env, NULL, "Variable type not supported yet");
        return NULL;
    }

    size_t len;
    NC_CALL(nc_inq_attlen(self->parent_id, self->var_id, self->name.c_str(), &len));

    if (self->type == NC_CHAR || self->type == NC_STRING) {
        printf("STRING TYPE: %ld\n", len);
        char v[len + 1];
        v[len] = 0;
        NC_CALL(nc_get_att_text(self->parent_id, self->var_id, self->name.c_str(), v));
        RETURN_STR(v);
    }

    if (len == 1) {
        switch (self->type) {
            case NC_BYTE: {
                int8_t v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_I8(v);
            }
            case NC_SHORT: {
                int16_t v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_I16(v);
            }
            case NC_INT: {
                int32_t v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_I32(v);
            }
            case NC_FLOAT: {
                float v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_FLOAT(v);
            }
            case NC_DOUBLE: {
                double v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_DOUBLE(v);
            }
            case NC_UBYTE: {
                uint8_t v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_U8(v);
            }
            case NC_USHORT: {
                uint16_t v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_U16(v);
            }
            case NC_UINT: {
                uint32_t v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_U32(v);
            }
            case NC_INT64: {
                int64_t v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_I64(v);
            }
            case NC_UINT64: {
                uint64_t v;
                NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), &v));
                RETURN_U64(v);
            }
            default:
                napi_throw_error(env, NULL, "Variable type not supported yet");
                return NULL;
        }
    }

    napi_typedarray_type type = typedarray_cons_per_type(env, self->type);

    napi_value array;


    void* buffer_data = NULL;
    napi_value buffer;

    NAPI_CALL(napi_create_arraybuffer(
        env,
        len * type_sizes[self->type],
        &buffer_data,
        &buffer
    ));

    NAPI_CALL(napi_create_typedarray(
        env,
        type,
        len,
        buffer,
        0,
        &array
    ));

    NC_CALL(nc_get_att(self->parent_id, self->var_id, self->name.c_str(), buffer_data));

    return array;
}

napi_value Attribute::SetValue(napi_env env, napi_callback_info info) {
    ARGS(1, VALUE(val))
    Attribute* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));

    if ((self->type < NC_BYTE || self->type > NC_UINT64) && self->type != NC_STRING) {
        napi_throw_error(env, NULL, "Variable type not supported yet");
        return NULL;
    }

    switch (self->type) {
        case NC_BYTE: {
            VAR_I8_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_SHORT: {
            VAR_I16_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_INT: {
            VAR_I32_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_FLOAT: {
            VAR_FLOAT_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_DOUBLE: {
            VAR_DOUBLE_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_UBYTE: {
            VAR_U8_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_USHORT: {
            VAR_U16_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_UINT: {
            VAR_U32_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_INT64: {
            VAR_I64_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_UINT64: {
            VAR_U64_FROM_JS(value, val);
            NC_CALL(nc_put_att(self->parent_id, self->var_id, self->name.c_str(), self->type, 1, &value));
        } break;
        case NC_CHAR:
        case NC_STRING: {
            VAR_STR_FROM_JS(value, val);
            NC_CALL(nc_put_att_text(self->parent_id, self->var_id, self->name.c_str(), strlen(value), value));
        } break;
        default:
            napi_throw_error(env, NULL, "Variable type not supported yet");
            return NULL;
    }

    return NULL;
}
/*
void Attribute::Delete(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Attribute* obj = node::ObjectWrap::Unwrap<Attribute>(args.Holder());
    int retval = nc_del_att(obj->parent_id, obj->var_id, obj->name.c_str());
    if (retval != NC_NOERR) {
        throw_netcdf_error(args.GetIsolate(), retval);
    }
}
*/

} // end namespace
