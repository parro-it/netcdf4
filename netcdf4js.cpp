#include <node.h>
#include "File.h"
#include "Variable.h"
#include "Group.h"

namespace netcdf4js {
    using v8::Local;
    using v8::Object;

    void InitAll(Local<Object> exports) {
        File::Init(exports);
        Variable::Init(exports);
        Group::Init(exports);
    }

    NODE_MODULE(netcdf4, InitAll)
}
