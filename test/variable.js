var expect = require("chai").expect,
    netcdf4 = require("../build/Release/netcdf4.node");

describe('Variable', function() {
  it('should read a slice', function() {
      var file = new netcdf4.File("test/testrh.nc", "r");
      var results = Array.from(file.root.variables.var1.readSlice(0, 4));
      expect(results).to.deep.equal([420, 197, 391.5, 399]);
  });

  it('should read a strided slice', function() {
      var file = new netcdf4.File("test/testrh.nc", "r");
      var results = Array.from(file.root.variables.var1.readStridedSlice(0, 2, 2));
        expect(results).to.deep.equal([420, 391.5]);
  });

});
