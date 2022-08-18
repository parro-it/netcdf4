#ifndef NETCDF4JSTYPES_H
#define NETCDF4JSTYPES_H

#include <netcdf.h>


#define NC_TYPES(ID) ID < NC_BYTE || ID > NC_UINT?"unsupported":netcdf4js::type_names[ID]

#define NC_FORMATS(ID) ID < NC_FORMATX_UNDEFINED || ID > NC_FORMATX_ZARR ?"unsupported":netcdf4js::format_names[ID]

namespace netcdf4js {
    extern const char *type_names[];
    extern const char *format_names[];
}

#endif