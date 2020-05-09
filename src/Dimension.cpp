#include <netcdf.h>
#include "netcdf4js.h"
#include "stdio.h"

namespace netcdf4js {

Napi::FunctionReference Dimension::constructor;

Napi::Object Dimension::Build(Napi::Env env, int parent_id, int id) {
	return constructor.New({Napi::Number::New(env, parent_id), Napi::Number::New(env, id)});
}

Dimension::Dimension(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Dimension>(info) {
	if (info.Length() < 2) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return;
	}

	parent_id = info[0].As<Napi::Number>().Int32Value();
	id = info[1].As<Napi::Number>().Int32Value();
}

Napi::Object Dimension::Init(Napi::Env env, Napi::Object exports) {

	Napi::HandleScope scope(env);

	auto properties = {
		InstanceMethod("inspect", &Dimension::Inspect),
		InstanceAccessor<&Dimension::GetId>("id"),
		InstanceAccessor<&Dimension::GetLength>("length"),
		InstanceAccessor<&Dimension::GetName, &Dimension::SetName>("name"),
	};

	Napi::Function func = DefineClass(env, "Dimension", properties);
	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Dimension", func);
	return exports;
}
/*
bool Dimension::get_name(char *name) const {
	int retval = nc_inq_dimname(parent_id, id, name);
	if (retval != NC_NOERR) {
		throw_netcdf_error(v8::Isolate::GetCurrent(), retval);
		return false;
	}
	return true;
}
*/

Napi::Value Dimension::GetId(const Napi::CallbackInfo &info) {
	return Napi::Number::New(info.Env(), id);
}

Napi::Value Dimension::GetLength(const Napi::CallbackInfo &info) {
	size_t len;
	NC_CALL(nc_inq_dimlen(this->parent_id, this->id, &len));
	return Napi::Number::New(info.Env(), len);
}

Napi::Value Dimension::GetName(const Napi::CallbackInfo &info) {
	char name[NC_MAX_NAME + 1];
	NC_CALL(nc_inq_dimname(parent_id, id, name));
	return Napi::String::New(info.Env(), name);
}

void Dimension::SetName(const Napi::CallbackInfo &info, const Napi::Value &value) {
	NC_CALL_VOID(nc_rename_dim(this->parent_id, this->id, value.ToString().Utf8Value().c_str()));
}

Napi::Value Dimension::Inspect(const Napi::CallbackInfo &info) {
	return Napi::String::New(info.Env(), "[object Dimension]");
}
} // namespace netcdf4js
