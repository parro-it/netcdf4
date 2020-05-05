#include "File.h"
#include <assert.h>
#include <netcdf.h>
#include <string.h>
#include <string>
#include "Group.h"
//#include "Variable.h"
#include "napi-utils.h"

namespace netcdf4js {

napi_ref File::constructor;

File::File(const int &id_) : id(id_), closed(false) {}

File::~File() {
	if (!closed) {
		int retval = nc_close(id);
		assert(retval == NC_NOERR);
	}
	napi_delete_reference(env_, wrapper_);
}

extern "C" {
napi_value File_Init(napi_env env, napi_value exports) {
	return File::Init(env, exports);
}
}

void File::Destructor(napi_env env, void *nativeObject, void *finalize_hint) {
	reinterpret_cast<File *>(nativeObject)->~File();
}

napi_value File::Init(napi_env env, napi_value exports) {
	napi_property_descriptor properties[] = {
		DECLARE_NAPI_METHOD("sync", Sync),
		DECLARE_NAPI_METHOD("close", Close),
		DECLARE_NAPI_METHOD("inspect", Inspect),
	};

	napi_value constructor;
	NAPI_CALL(napi_define_class(env, "File", NAPI_AUTO_LENGTH, File::New, nullptr, 3, properties,
								&constructor));

	NAPI_CALL(napi_set_named_property(env, exports, "File", constructor));

	NAPI_CALL(napi_create_reference(env, constructor, 0, &File::constructor));

	return nullptr;
}

napi_value File::New(napi_env env, napi_callback_info info) {
	OPTIONAL_ARGS(1, 3, STR(filename), STR(mode_arg), STR(format_arg))

	int format = NC_NETCDF4;

	if (argc == 3) {
		if (strcmp(format_arg, "classic") == 0) {
			format = 0;
		} else if (strcmp(format_arg, "classic64") == 0) {
			format = NC_64BIT_OFFSET;
		} else if (strcmp(format_arg, "netcdf4") == 0) {
			format = NC_NETCDF4;
		} else if (strcmp(format_arg, "netcdf4classic") == 0) {
			format = NC_NETCDF4 | NC_CLASSIC_MODEL;
		} else {
			napi_throw_type_error(env, NULL, "Unknown file format");
			return NULL;
		}
	}

	int id;

	if (strcmp(mode_arg, "r") == 0) {
		NC_CALL(nc_open(filename, NC_NOWRITE, &id));
	} else if (strcmp(mode_arg, "w") == 0) {
		NC_CALL(nc_open(filename, NC_WRITE, &id));
	} else if (strcmp(mode_arg, "c") == 0) {
		NC_CALL(nc_create(filename, format | NC_NOCLOBBER, &id));
	} else if (strcmp(mode_arg, "c!") == 0) {
		NC_CALL(nc_create(filename, format | NC_CLOBBER, &id));
	} else {
		napi_throw_type_error(env, NULL, "Unknown file mode");
		return NULL;
	}

	File *file = new File(id);
	file->env_ = env;

	NAPI_CALL(napi_wrap(env, jsthis, reinterpret_cast<void *>(file), File::Destructor,
						nullptr, // finalize_hint
						&file->wrapper_));

	NAPI_CALL(napi_set_named_property(env, jsthis, "root", Group::Build(env, id)));

	return jsthis;
}

napi_value File::Sync(napi_env env, napi_callback_info info) {
	ARGS(0)
	File *self;

	NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&self)));
	NC_CALL(nc_sync(self->id));

	return nullptr;
}

napi_value File::Close(napi_env env, napi_callback_info info) {
	ARGS(0)
	File *self;

	NAPI_CALL(napi_unwrap(env, jsthis, reinterpret_cast<void **>(&self)));
	NC_CALL(nc_close(self->id));

	self->closed = true;

	return nullptr;
}

napi_value File::Inspect(napi_env env, napi_callback_info info) {
	ARGS(0);
	RETURN_STR((char *)"[object File]");
}
} // namespace netcdf4js
