#include <netcdf.h>
#include "netcdf4js.h"
#include "stdio.h"

namespace netcdf4js {

Napi::FunctionReference Group::constructor;

Napi::Object Group::Build(Napi::Env env, int id) {
	return constructor.New({Napi::Number::New(env, id)});
}

Group::Group(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Group>(info) {
	if (info.Length() < 1) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return;
	}

	id = info[0].As<Napi::Number>().Int32Value();
}

Napi::Object Group::Init(Napi::Env env, Napi::Object exports) {

	Napi::HandleScope scope(env);

	auto properties = {
		InstanceMethod("addVariable", &Group::AddVariable),
		InstanceMethod("addDimension", &Group::AddDimension),
		InstanceMethod("addSubgroup", &Group::AddSubgroup),
		InstanceMethod("addAttribute", &Group::AddAttribute),
		InstanceMethod("inspect", &Group::Inspect),
		InstanceAccessor<&Group::GetId>("id"),
		InstanceAccessor<&Group::GetVariables>("variables"),
		InstanceAccessor<&Group::GetDimensions>("dimensions"),
		InstanceAccessor<&Group::GetUnlimited>("unlimited"),
		InstanceAccessor<&Group::GetAttributes>("attributes"),
		InstanceAccessor<&Group::GetSubgroups>("subgroups"),
		InstanceAccessor<&Group::GetName>("name"),
		InstanceAccessor<&Group::GetFullname>("fullname"),

	};

	Napi::Function func = DefineClass(env, "Group", properties);
	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Group", func);
	return exports;
}
/*
bool Group::get_name(char *name) const {
	int retval = nc_inq_grpname(id, name);
	if (retval != NC_NOERR) {
		throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
		return false;
	}
	return true;
}
*/
Napi::Value Group::AddAttribute(const Napi::CallbackInfo &info) {
	/*

	v8::Isolate *isolate = args.GetIsolate();
	Group *obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
	if (args.Length() < 3) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	std::string type_str = *v8::String::Utf8Value(

		isolate, args[1]);
	int type = get_type(type_str);
	if (type == NC2_ERR) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Unknown variable type", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	Attribute *res = new Attribute(*v8::String::Utf8Value(

									   isolate, args[0]),
								   NC_GLOBAL, obj->id, type);
	res->set_value(args[2]);
	args.GetReturnValue().Set(res->handle());
	*/
	return info.Env().Undefined();
}

Napi::Value Group::AddSubgroup(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = args.GetIsolate();
	Group *obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
	if (args.Length() < 1) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	int new_id;
	int retval = nc_def_grp(obj->id,
							*v8::String::Utf8Value(

								isolate, args[0]),
							&new_id);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	Group *res = new Group(new_id);
	args.GetReturnValue().Set(res->handle());
	*/
	return info.Env().Undefined();
}

Napi::Value Group::AddDimension(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = args.GetIsolate();
	Group *obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
	if (args.Length() < 2) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	int len;
	if (std::string(*v8::String::Utf8Value(

			isolate, args[1])) == "unlimited") {
		len = NC_UNLIMITED;
	} else {
		if (!args[1]->IsUint32()) {
			isolate->ThrowException(v8::Exception::TypeError(
				v8::String::NewFromUtf8(isolate, "Expecting a positive integer",
										v8::NewStringType::kNormal)
					.ToLocalChecked()));
			return;
		}
		len = args[1]->Uint32Value(isolate->GetCurrentContext()).ToChecked();
	}
	int new_id;
	int retval = nc_def_dim(obj->id,
							*v8::String::Utf8Value(

								isolate, args[0]),
							len, &new_id);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	Dimension *res = new Dimension(new_id, obj->id);
	args.GetReturnValue().Set(res->handle());
	*/
	return info.Env().Undefined();
}

Napi::Value Group::AddVariable(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = args.GetIsolate();
	Group *obj = node::ObjectWrap::Unwrap<Group>(args.Holder());
	if (args.Length() < 3) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Wrong number of arguments",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	std::string type_str = *v8::String::Utf8Value(

		isolate, args[1]);
	int type = get_type(type_str);
	if (type == NC2_ERR) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Unknown variable type", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	if (type == NC_STRING) {
		isolate->ThrowException(
			v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "Unsupported variable type",
															 v8::NewStringType::kNormal)
										 .ToLocalChecked()));
		return;
	}
	if (!args[2]->IsArray()) {
		isolate->ThrowException(v8::Exception::TypeError(
			v8::String::NewFromUtf8(isolate, "Expecting an array", v8::NewStringType::kNormal)
				.ToLocalChecked()));
		return;
	}
	v8::Local<v8::Object> array = args[2]->ToObject(isolate->GetCurrentContext()).ToLocalChecked();
	size_t ndims = array
					   ->Get(isolate->GetCurrentContext(),
							 v8::String::NewFromUtf8(isolate, "length", v8::NewStringType::kNormal)
								 .ToLocalChecked())
					   .ToLocalChecked()
					   ->Uint32Value(isolate->GetCurrentContext())
					   .ToChecked();
	int *dimids = new int[ndims];
	for (size_t i = 0; i < ndims; i++) {
		dimids[i] = array->Get(isolate->GetCurrentContext(), i)
						.ToLocalChecked()
						->Int32Value(isolate->GetCurrentContext())
						.ToChecked();
	}
	int new_id;
	int retval = nc_def_var(obj->id,
							*v8::String::Utf8Value(

								isolate, args[0]),
							type, ndims, dimids, &new_id);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		delete[] dimids;
		return;
	}
	Variable *res = new Variable(new_id, obj->id);
	args.GetReturnValue().Set(res->handle());
	delete[] dimids;
	*/
	return info.Env().Undefined();
}

Napi::Value Group::GetId(const Napi::CallbackInfo &info) {
	return Napi::Number::New(info.Env(), id);
}

Napi::Value Group::GetVariables(const Napi::CallbackInfo &info) {
	int nvars;
	NC_CALL(nc_inq_varids(this->id, &nvars, NULL));
	int *var_ids = new int[nvars];
	NC_CALL(nc_inq_varids(this->id, NULL, var_ids));

	Napi::Object vars = Napi::Object::New(info.Env());

	char name[NC_MAX_NAME + 1];
	for (int i = 0; i < nvars; ++i) {
		Napi::Object var = Variable::Build(info.Env(), var_ids[i], this->id);

		int retval = nc_inq_varname(this->id, var_ids[i], name);
		if (retval == NC_NOERR) {
			vars.Set(name, var);
		}
	}
	delete[] var_ids;
	return vars;
}

Napi::Value Group::GetDimensions(const Napi::CallbackInfo &info) {
	int ndims;
	NC_CALL(nc_inq_dimids(this->id, &ndims, NULL, 0));
	int *dim_ids = new int[ndims];
	NC_CALL(nc_inq_dimids(this->id, NULL, dim_ids, 0));

	Napi::Object dims = Napi::Object::New(info.Env());

	char name[NC_MAX_NAME + 1];
	for (int i = 0; i < ndims; ++i) {
		Napi::Object dim = Dimension::Build(info.Env(), this->id, dim_ids[i]);

		int retval = nc_inq_dimname(this->id, dim_ids[i], name);
		if (retval == NC_NOERR) {
			dims.Set(name, dim);
		}
	}
	delete[] dim_ids;
	return dims;
}

Napi::Value Group::GetUnlimited(const Napi::CallbackInfo &info) {
	/*
	v8::Isolate *isolate = info.GetIsolate();
	Group *obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
	int ndims;
	int retval = nc_inq_unlimdims(obj->id, &ndims, NULL);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		return;
	}
	int *dim_ids = new int[ndims];
	retval = nc_inq_unlimdims(obj->id, NULL, dim_ids);
	if (retval != NC_NOERR) {
		throw_netcdf_error(isolate, retval);
		delete[] dim_ids;
		return;
	}
	v8::Local<v8::Object> result = v8::Object::New(isolate);
	char name[NC_MAX_NAME + 1];
	for (int i = 0; i < ndims; ++i) {
		Dimension *d = new Dimension(dim_ids[i], obj->id);
		if (d->get_name(name)) {
			result->Set(
				isolate->GetCurrentContext(),
				v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(),
				d->handle());
		} else {
			delete[] dim_ids;
			return;
		}
	}
	info.GetReturnValue().Set(result);
	delete[] dim_ids;
	*/
	return info.Env().Undefined();
}
/*
void Group::GetAttributes(v8::Local<v8::String> property,
						  const v8::PropertyCallbackInfo<v8::Value> &info) {
	v8::Isolate *isolate = info.GetIsolate();
	Group *obj = node::ObjectWrap::Unwrap<Group>(info.Holder());
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
		Attribute *a = new Attribute(name, NC_GLOBAL, obj->id);
		result->Set(
			isolate->GetCurrentContext(),
			v8::String::NewFromUtf8(isolate, name, v8::NewStringType::kNormal).ToLocalChecked(),
			a->handle());
	}
	info.GetReturnValue().Set(result);
}
*/
Napi::Value Group::GetAttributes(const Napi::CallbackInfo &info) {
	int natts;
	NC_CALL(nc_inq_natts(this->id, &natts));

	Napi::Object attrs = Napi::Object::New(info.Env());

	char name[NC_MAX_NAME + 1];
	for (int i = 0; i < natts; ++i) {
		NC_CALL(nc_inq_attname(this->id, NC_GLOBAL, i, name));
		Napi::Object attr = Attribute::Build(info.Env(), name, NC_GLOBAL, this->id);
		attrs.Set(name, attr);
	}

	return attrs;
}

Napi::Value Group::GetSubgroups(const Napi::CallbackInfo &info) {
	int ngrps;
	NC_CALL(nc_inq_grps(this->id, &ngrps, NULL));
	int *grp_ids = new int[ngrps];

	NC_CALL(nc_inq_grps(this->id, NULL, grp_ids));

	Napi::Object subgroups = Napi::Object::New(info.Env());

	char name[NC_MAX_NAME + 1];
	for (int i = 0; i < ngrps; ++i) {
		Napi::Object subgroup = Group::Build(info.Env(), grp_ids[i]);
		int retval = nc_inq_grpname(grp_ids[i], name);
		if (retval == NC_NOERR) {
			subgroups.Set(name, subgroup);
		}
	}

	delete[] grp_ids;
	return subgroups;
}

Napi::Value Group::GetName(const Napi::CallbackInfo &info) {
	char name[NC_MAX_NAME + 1];
	NC_CALL(nc_inq_grpname(id, name));
	return Napi::String::New(info.Env(), name);
}

Napi::Value Group::GetFullname(const Napi::CallbackInfo &info) {
	size_t len;
	NC_CALL(nc_inq_grpname_len(this->id, &len));
	char *name = new char[len + 1];

	name[len] = 0;
	NC_CALL(nc_inq_grpname_full(this->id, NULL, name));
	auto ret = Napi::String::New(info.Env(), name);
	delete[] name;
	return ret;
}

Napi::Value Group::Inspect(const Napi::CallbackInfo &info) {
	return Napi::String::New(info.Env(), "[object Group]");
}
} // namespace netcdf4js
