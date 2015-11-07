#include <node.h>
#include "File.h"
#include "Variable.h"
#include "Group.h"
#include "Dimension.h"
#include "Attribute.h"

namespace netcdf4js {
    void InitAll(v8::Local<v8::Object> exports) {
        File::Init(exports);
        Variable::Init(exports);
        Group::Init(exports);
        Dimension::Init(exports);
        Attribute::Init(exports);
    }

    NODE_MODULE(netcdf4, InitAll)
}
