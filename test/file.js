var expect = require("chai").expect,
    netcdf4 = require("../build/Release/netcdf4.node");

describe('File', function() {
    describe('new', function() {
        it('should throw an error when file not found', function() {
            expect(function() {
                var file = new netcdf4.File("DOESNOTEXIST", "r");
            }).to.throw("No such file or directory");
        });

        it('should throw an error for wrong file mode', function() {
            expect(function() {
                var file = new netcdf4.File("test/testrh.nc", "WRONG");
            }).to.throw("Unknown file mode");
        });


        it('should open a file', function() {
            var file = new netcdf4.File("test/testrh.nc", "r");
            expect(typeof file).to.eq("object")
            expect(file).not.null
            file.close();
        });

        it('file contains variables', function() {
            var file = new netcdf4.File("test/testrh.nc", "r");
            const vars = file.root.variables;
            const res = vars.var1.readSlice(0, 4);
            var results = Array.from(res);
            expect(results).to.deep.equal([420, 197, 391.5, 399]);
            file.close();
        });
    });

});
