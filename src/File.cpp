#include <iostream>
#include <netcdf.h>
#include <string>
#include "netcdf4js.h"

namespace netcdf4js {

Napi::FunctionReference File::constructor;

File::~File() {
	if (!closed) {
		NC_CALL_ENV(this->Env(), nc_close(id));
	}
}

Napi::Object File::Init(Napi::Env env, Napi::Object exports) {
	Napi::HandleScope scope(env);

	Napi::Function func =
		DefineClass(env, "File",
					{InstanceMethod("sync", &File::Sync), InstanceMethod("close", &File::Close),
					 InstanceMethod("inspect", &File::Inspect)});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("File", func);
	return exports;
}

File::File(const Napi::CallbackInfo &info) : Napi::ObjectWrap<File>(info) {

	if (info.Length() < 2) {
		Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
		return;
	}

	std::string filename = info[0].As<Napi::String>().Utf8Value();

	std::string mode_arg = info[1].As<Napi::String>().Utf8Value();

	int format = NC_NETCDF4;
	if (info.Length() > 2) {
		std::string format_arg = info[2].As<Napi::String>().Utf8Value();

		if (format_arg == "classic") {
			format = 0;
		} else if (format_arg == "classic64") {
			format = NC_64BIT_OFFSET;
		} else if (format_arg == "netcdf4") {
			format = NC_NETCDF4;
		} else if (format_arg == "netcdf4classic") {
			format = NC_NETCDF4 | NC_CLASSIC_MODEL;
		} else {
			Napi::TypeError::New(info.Env(), "Unknown file format").ThrowAsJavaScriptException();
			return;
		}
	}
	if (mode_arg == "r") {
		NC_CALL_VOID(nc_open(filename.c_str(), NC_NOWRITE, &id));
	} else if (mode_arg == "w") {
		NC_CALL_VOID(nc_open(filename.c_str(), NC_WRITE, &id));
	} else if (mode_arg == "c") {
		NC_CALL_VOID(nc_create(filename.c_str(), format | NC_NOCLOBBER, &id));
	} else if (mode_arg == "c!") {
		NC_CALL_VOID(nc_create(filename.c_str(), format | NC_CLOBBER, &id));
	} else {
		Napi::TypeError::New(info.Env(), "Unknown file mode").ThrowAsJavaScriptException();
		return;
	}

	auto group = Group::Build(info.Env(), id);
	this->Value().Set("root", group);
}

Napi::Value File::Sync(const Napi::CallbackInfo &info) {
	Napi::Env env = info.Env();

	int retval = nc_sync(this->id);
	if (retval != NC_NOERR) {
		Napi::Error::New(env, nc_strerror(retval)).ThrowAsJavaScriptException();
	}

	return info.Env().Undefined();
}

Napi::Value File::Close(const Napi::CallbackInfo &info) {
	int retval = nc_close(this->id);
	if (retval != NC_NOERR) {
		Napi::Error::New(info.Env(), nc_strerror(retval)).ThrowAsJavaScriptException();
	}
	this->closed = true;
	return info.Env().Undefined();
}

Napi::Value File::Inspect(const Napi::CallbackInfo &info) {
	return Napi::String::New(info.Env(), "[object File]");
}
} // namespace netcdf4js
