#ifndef NETCDF4JSTYPES_H
#define NETCDF4JSTYPES_H

#include <netcdf.h>
#include <netcdf_meta.h>
#include <node_version.h>

#if NODE_MAJOR_VERSION > 9
#define NC_TYPES(ID) ID < NC_BYTE || (ID > NC_UINT64 && ID != NC_STRING)?"unsupported":netcdf4js::type_names[ID]
#else
#define NC_TYPES(ID) ID < NC_BYTE || (ID > NC_UINT && ID != NC_STRING)?"unsupported":netcdf4js::type_names[ID]
#endif

// Ubuntu 22.04 and Ubuntu 20.04 have a different name for NCZARR file type constant
// So....
#if NETCDF_VERSION_MINOR < 7
#define NC_FORMATS(ID) ID < NC_FORMATX_UNDEFINED || ID > NC_FORMATX_DAP4 ?"unsupported":netcdf4js::format_names[ID]
#endif

#if NETCDF_VERSION_MINOR < 8 && NETCDF_VERSION_MINOR >7
#ifdef NC_FORMATX_NCZARR
#define NC_FORMATS(ID) ID < NC_FORMATX_UNDEFINED || ID > NC_FORMATX_NCZARR ?"unsupported":netcdf4js::format_names[ID]
#else
#define NC_FORMATS(ID) ID < NC_FORMATX_UNDEFINED || ID > NC_FORMATX_ZARR ?"unsupported":netcdf4js::format_names[ID]
#endif
#endif

#if NETCDF_VERSION_MINOR >= 8
#define NC_FORMATS(ID) ID < NC_FORMATX_UNDEFINED || ID > NC_FORMATX_NCZARR ?"unsupported":netcdf4js::format_names[ID]
#endif

namespace netcdf4js {
    extern const char *type_names[];
    extern const char *format_names[];
}

#endif