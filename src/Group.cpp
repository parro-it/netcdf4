#include "Group.h"
#include <netcdf.h>
#include "Attribute.h"
#include "Dimension.h"
//#include "Dimension.h"
#include "Variable.h"
#include "napi-utils.h"

namespace netcdf4js {

napi_ref Group::constructor;

Group::Group(const int &id_) : id(id_) {}

Group::~Group() {
	napi_delete_reference(env_, wrapper_);
}

extern "C" {
napi_value Group_Init(napi_env env, napi_value exports) {
	return Group::Init(env, exports);
}
}

void Group::Destructor(napi_env env, void *nativeObject, void *finalize_hint) {
	reinterpret_cast<Group *>(nativeObject)->~Group();
}

napi_value Group::Init(napi_env env, napi_value exports) {
	napi_property_descriptor properties[] = {
		DECLARE_NAPI_METHOD("addVariable", AddVariable),
		DECLARE_NAPI_METHOD("addDimension", AddDimension),
		DECLARE_NAPI_METHOD("addSubgroup", AddSubgroup),
		DECLARE_NAPI_METHOD("addAttribute", AddAttribute),
		DECLARE_NAPI_METHOD("inspect", Inspect),
		DECLARE_NAPI_PROP("id", Group::GetId, nullptr),
		DECLARE_NAPI_PROP("variables", Group::GetVariables, nullptr),
		DECLARE_NAPI_PROP("dimensions", Group::GetDimensions, nullptr),
		DECLARE_NAPI_PROP("unlimited", Group::GetUnlimited, nullptr),
		DECLARE_NAPI_PROP("attributes", Group::GetAttributes, nullptr),
		DECLARE_NAPI_PROP("subgroups", Group::GetSubgroups, nullptr),
		DECLARE_NAPI_PROP("name", Group::GetName, nullptr),
		DECLARE_NAPI_PROP("fullname", Group::GetFullname, nullptr),
	};

	napi_value cons;

	NAPI_CALL(napi_define_class(env, "Group", NAPI_AUTO_LENGTH, Group::New, nullptr, 13, properties,
								&cons));

	NAPI_CALL(napi_set_named_property(env, exports, "Group", cons));

	NAPI_CALL(napi_create_reference(env, cons, 0, &constructor));

	return nullptr;
}

napi_value Group::Build(napi_env env, int id) {
	napi_value group_js;
	char *nuts_error = NULL;

	VAR_JS_FROM_I32(id_js, id);

	napi_value cons;

	NAPI_CALL(napi_get_reference_value(env, constructor, &cons));
	NAPI_CALL(napi_new_instance(env, cons, 1, &id_js, &group_js));

	return group_js;
}

napi_value Group::New(napi_env env, napi_callback_info info) {
	ARGS(1, I32(id))
	Group *group = new Group(id);
	group->env_ = env;

	NAPI_CALL(napi_wrap(env, jsthis, reinterpret_cast<void *>(group), Group::Destructor,
						nullptr, // finalize_hint
						&group->wrapper_));

	return jsthis;
}

bool Group::get_name(char *name) const {
	int retval = nc_inq_grpname(id, name);
	if (retval != NC_NOERR) {
		return false;
	}
	return true;
}

napi_value Group::AddAttribute(napi_env env, napi_callback_info info) {
	/*
	v8::Isolate* isolate = args.GetIsolate();
	Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
	if (args.Length() < 3) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong
	number of arguments", v8::NewStringType::kNormal).ToLocalChecked())); return;
	}
	std::string type_str = *v8::String::Utf8Value(
		isolate,
		args[1]);

	int type = get_type(type_str);
	if (type == NC2_ERR) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown
	variable type", v8::NewStringType::kNormal).ToLocalChecked())); return;
	}
	Attribute* res = new Attribute(*v8::String::Utf8Value(
									   isolate,
									   args[0]),
								   NC_GLOBAL, obj->id, type);
	res->set_value(args[2]);
	args.GetReturnValue().Set(res->handle());
	*/
	return NULL;
}

napi_value Group::AddSubgroup(napi_env env, napi_callback_info info) {
	/*
	v8::Isolate* isolate = args.GetIsolate();
	Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
	if (args.Length() < 1) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong
	number of arguments", v8::NewStringType::kNormal).ToLocalChecked())); return;
	}
	int new_id;
	int retval = nc_def_grp(obj->id,
							*v8::String::Utf8Value(
								isolate,
								args[0]),
							&new_id);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	Group* res = new Group(new_id);
	args.GetReturnValue().Set(res->handle());
	*/
	return NULL;
}

napi_value Group::AddDimension(napi_env env, napi_callback_info info) {
	/*
	v8::Isolate* isolate = args.GetIsolate();
	Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
	if (args.Length() < 2) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong
	number of arguments", v8::NewStringType::kNormal).ToLocalChecked())); return;
	}
	int len;
	if (std::string(*v8::String::Utf8Value(
			isolate,
			args[1]))
		== "unlimited") {
		len = NC_UNLIMITED;
	} else {
		if (!args[1]->IsUint32()) {
			isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate,
	"Expecting a positive integer", v8::NewStringType::kNormal).ToLocalChecked())); return;
		}
		len = args[1]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
	}
	int new_id;
	int retval = nc_def_dim(obj->id,
							*v8::String::Utf8Value(
								isolate,
								args[0]),
							len, &new_id);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	Dimension* res = new Dimension(new_id, obj->id);
	args.GetReturnValue().Set(res->handle());
	*/
	return NULL;
}

napi_value Group::AddVariable(napi_env env, napi_callback_info info) {
	/*
	v8::Isolate* isolate = args.GetIsolate();
	Group* obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
	if (args.Length() < 3) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong
	number of arguments", v8::NewStringType::kNormal).ToLocalChecked())); return;
	}
	std::string type_str = *v8::String::Utf8Value(
		isolate,
		args[1]);
	int type = get_type(type_str);
	if (type == NC2_ERR) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unknown
	variable type", v8::NewStringType::kNormal).ToLocalChecked())); return;
	}
	if (type == NC_STRING) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate,
	"Unsupported variable type", v8::NewStringType::kNormal).ToLocalChecked())); return;
	}
	if (!args[2]->IsArray()) {
		isolate->ThrowException(v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Expecting
	an array", v8::NewStringType::kNormal).ToLocalChecked())); return;
	}
	v8::Local<v8::Object> array = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
	size_t ndims = array->Get(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate,
	"length",
	v8::NewStringType::kNormal).ToLocalChecked()).ToLocalChecked()->Uint32Value(isolate->GetCurrentContext()).ToChecked();
	int* dimids = new int[ndims];
	for (size_t i = 0; i < ndims; i++) {
		dimids[i] = array->Get(isolate->GetCurrentContext(),
	i).ToLocalChecked()->Int32Value(isolate->GetCurrentContext()).ToChecked();
	}
	int new_id;
	int retval = nc_def_var(obj->id,
							*v8::String::Utf8Value(
								isolate,
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
	*/
	return NULL;
}

napi_value Group::GetId(napi_env env, napi_callback_info info) {
	/*
	v8::Isolate* isolate = info.GetIsolate();
	Group* obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
	info.GetReturnValue().Set(v8::Integer::New(isolate, obj->id));
	*/
	return NULL;
}

napi_value Group::GetVariables(napi_env env, napi_callback_info info) {
	ARGS(0);

	Group *self;
	NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&self)));

	int nvars;
	NC_CALL(nc_inq_varids(self->id, &nvars, NULL));

	int var_ids[nvars];
	NC_CALL(nc_inq_varids(self->id, NULL, var_ids));

	napi_value array;
	NAPI_CALL(napi_create_array_with_length(env, nvars, &array));

	for (int i = 0; i < nvars; i++) {
		napi_value var = Variable::Build(env, var_ids[i], self->id);
		NAPI_CALL(napi_set_element(env, array, i, var));

		char name[NC_MAX_NAME + 1];
		NC_CALL(nc_inq_varname(self->id, var_ids[i], name));

		NAPI_CALL(napi_set_named_property(env, array, name, var));
	}

	return array;
}

napi_value Group::GetDimensions(napi_env env, napi_callback_info info) {
	ARGS(0);
	Group *self;
	NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&self)));
	int ndims;
	NC_CALL(nc_inq_dimids(self->id, &ndims, NULL, 0));

	int dim_ids[ndims];
	NC_CALL(nc_inq_dimids(self->id, NULL, dim_ids, 0));
	napi_value array;
	NAPI_CALL(napi_create_array_with_length(env, ndims, &array));
	for (int i = 0; i < ndims; i++) {
		printf("iter %d\n", i);
		napi_value dims = Dimension::Build(env, dim_ids[i], self->id);
		printf("Build done\n");
		NAPI_CALL(napi_set_element(env, array, i, dims));
		printf("napi_set_element done\n");

		char name[NC_MAX_NAME + 1];
		if (nc_inq_dimname(self->id, dim_ids[i], name) == NC_NOERR) {
			printf("DIM NAME %s\n", name);
			NAPI_CALL(napi_set_named_property(env, array, name, dims));
		}
		printf("iter %d done\n", i);
	}

	return array;
}

napi_value Group::GetUnlimited(napi_env env, napi_callback_info info) {
	/*
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
			result->Set(isolate->GetCurrentContext(), v8::String::NewFromUtf8(isolate, name,
	v8::NewStringType::kNormal).ToLocalChecked(), d->handle()); } else { delete[] dim_ids; return;
		}
	}
	info.GetReturnValue().Set(result);
	delete[] dim_ids;
	*/
	return NULL;
}

napi_value Group::GetAttributes(napi_env env, napi_callback_info info) {
	ARGS(0);
	Group *self;
	NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&self)));

	int natts;
	NC_CALL(nc_inq_natts(self->id, &natts));

	napi_value array;
	NAPI_CALL(napi_create_array_with_length(env, natts, &array));

	for (int i = 0; i < natts; i++) {
		char name[NC_MAX_NAME + 1];
		nc_type type;

		NC_CALL(nc_inq_attname(self->id, NC_GLOBAL, i, name));
		NC_CALL(nc_inq_atttype(self->id, NC_GLOBAL, name, &type));

		napi_value attr = Attribute::Build(env, name, NC_GLOBAL, self->id, type);
		NAPI_CALL(napi_set_element(env, array, i, attr));
		NAPI_CALL(napi_set_named_property(env, array, name, attr));
	}

	return array;
}

napi_value Group::GetSubgroups(napi_env env, napi_callback_info info) {
	ARGS(0);
	Group *self;
	NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&self)));

	int nsubgroups;
	NC_CALL(nc_inq_grps(self->id, &nsubgroups, NULL));
	int grp_ids[nsubgroups];
	NC_CALL(nc_inq_grps(self->id, NULL, grp_ids));

	napi_value array;
	NAPI_CALL(napi_create_array_with_length(env, nsubgroups, &array));
	for (int i = 0; i < nsubgroups; i++) {
		napi_value sub = Group::Build(env, grp_ids[i]);
		NAPI_CALL(napi_set_element(env, array, i, sub));

		char name[NC_MAX_NAME + 1];
		NC_CALL(nc_inq_grpname(grp_ids[i], name));
		int retval = nc_inq_grpname(grp_ids[i], name);
		if (retval == NC_NOERR) {
			NAPI_CALL(napi_set_named_property(env, array, name, sub));
		}
	}

	return array;
}

napi_value Group::GetName(napi_env env, napi_callback_info info) {
	ARGS(0)
	Group *self;
	NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&self)));
	char name[NC_MAX_NAME + 1];
	if (self->get_name(name)) {
		RETURN_STR(name);
	}

	return NULL;
}

napi_value Group::GetFullname(napi_env env, napi_callback_info info) {
	/*
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
	info.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, name,
	v8::NewStringType::kNormal).ToLocalChecked()); delete[] name;
	*/
	return NULL;
}

napi_value Group::Inspect(napi_env env, napi_callback_info info) {
	ARGS(0)
	RETURN_STR((char *)"[object Group]");
}
} // namespace netcdf4js
