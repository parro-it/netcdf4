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
    });

});
