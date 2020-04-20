#include "Dimension.h"
#include <netcdf.h>
#include "napi-utils.h"
#include "stdio.h"

namespace netcdf4js {

napi_ref Dimension::constructor = nullptr;


napi_value Dimension::Build(napi_env env, int id, int parent_id) {
    napi_value group_js;
    char* nuts_error = NULL;
printf("VARS\n");
    VAR_JS_FROM_I32(id_js, id);
    VAR_JS_FROM_I32(parent_id_js, parent_id);

    napi_value cons;

printf("napi_get_reference_value %p\n", constructor);
    NAPI_CALL(napi_get_reference_value(env, constructor, &cons));
    napi_value args[] = {id_js, parent_id_js};
printf("napi_new_instance\n");
    NAPI_CALL(napi_new_instance(env, cons, 2, args ,&group_js));
printf("napi_new_instance DONE\n");
    return group_js;
}

napi_value Dimension::New(napi_env env, napi_callback_info info) {
    ARGS(2,  I32(id), I32(parent_id))

    napi_value target;
    NAPI_CALL(napi_get_new_target(env, info, &target));

    auto self = new Dimension(id, parent_id);

    self->env_ = env;
    NAPI_CALL(napi_wrap(env,
        jsthis,
        reinterpret_cast<void*>(self),
        Dimension::Destructor,
        nullptr,  // finalize_hint
        &self->wrapper_
    ));

    return jsthis;
}
Dimension::~Dimension() { napi_delete_reference(env_, wrapper_); }

void Dimension::Destructor(napi_env env, void* nativeObject, void* finalize_hint) {
  reinterpret_cast<Dimension*>(nativeObject)->~Dimension();
}

Dimension::Dimension(const int& id_, const int& parent_id_) :
    id(id_),
    parent_id(parent_id_) {}

extern "C" {
    napi_value Dimension_Init(napi_env env, napi_value exports) {
        return Dimension::Init(env, exports);
    }
}

napi_value Dimension::Init(napi_env env, napi_value exports) {
    napi_property_descriptor properties[] = {
      DECLARE_NAPI_PROP("name", GetName, SetName),
      DECLARE_NAPI_PROP("length", GetLength, nullptr),
      DECLARE_NAPI_PROP("id", GetId, nullptr),
      DECLARE_NAPI_METHOD("inspect", Inspect),
    };

    napi_value cons;
    NAPI_CALL(napi_define_class(
        env,
        "Dimension", NAPI_AUTO_LENGTH,
        Dimension::New,
        nullptr,
        4,
        properties,
        &cons
    ));

    NAPI_CALL(napi_set_named_property(
        env,
        exports,
        "Dimension",
        cons
    ));

    NAPI_CALL(napi_create_reference(
        env,
        cons,
        0,
        &Dimension::constructor
    ));

    return nullptr;
}

bool Dimension::get_name(char* name) const {
    int retval = nc_inq_dimname(parent_id, id, name);
    if (retval != NC_NOERR) {
        //throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
        return false;
    }
    return true;
}

napi_value Dimension::GetId(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.Holder());
    info.GetReturnValue().Set(v8::Integer::New(isolate, obj->id));
    */ return NULL;
}

napi_value Dimension::GetLength(napi_env env, napi_callback_info info) {
    ARGS(0)
    Dimension* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));

    size_t len;
    NC_CALL(nc_inq_dimlen(self->parent_id, self->id, &len))

    RETURN_U64(len);
}

napi_value Dimension::GetName(napi_env env, napi_callback_info info) {
    ARGS(0)
    Dimension* self;
    NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void**>(&self)));

    char name[NC_MAX_NAME + 1];
    if (self->get_name(name)) {
        RETURN_STR(name);
    }

    return NULL;

}

napi_value Dimension::SetName(napi_env env, napi_callback_info info) {
    /*
    v8::Isolate* isolate = info.GetIsolate();
    Dimension* obj = node::ObjectWrap::Unwrap<Dimension>(info.Holder());
    v8::String::Utf8Value new_name_(
#if NODE_MAJOR_VERSION >= 8
        isolate,
#endif
        val->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    int retval = nc_rename_dim(obj->parent_id, obj->id, *new_name_);
    if (retval != NC_NOERR) {
        throw_netcdf_error(isolate, retval);
        return;
    }
    */ return NULL;
}

napi_value Dimension::Inspect(napi_env env, napi_callback_info info) {
    ARGS(0);
    RETURN_STR((char *)"[object Dimension]");
}
}  // namespace netcdf4js
