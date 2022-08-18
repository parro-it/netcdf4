const expect = require("chai").expect;
const netcdf4 = require("..");
const { join } = require("path");

const fixture = join(__dirname, "testrh.nc");
const fixture1 = join(__dirname, "test_hgroups.nc");

describe("Variable", function () {
  it("should read variable params", function() {
    var file = new netcdf4.File(fixture1, "r");
    console.log(Object.keys(file.root.variables))
    console.log(file.root.variables.UTC_time)
    console.log(file.root.variables.UTC_time.id)
    console.log(file.root.variables.UTC_time.name)
    console.log(file.root.variables.UTC_time.type)
    console.log(file.root.variables.UTC_time.endianness)
    console.log(file.root.variables.UTC_time.attributes)
    console.log(file.root.variables.UTC_time.dimensions)
    console.log(file.root.variables.UTC_time.dimensions[0])


  });
  it("should read a slice", function () {
    var file = new netcdf4.File(fixture, "r");
    console.log(Object.keys(file.root.variables));
    console.log(file.root.variables.var1)
    console.log(file.root.variables.var1.id)
    console.log(file.root.variables.var1.name)
    console.log(file.root.variables.var1.type)
    console.log(file.root.variables.var1.attributes)
    console.log(file.root.variables.var1.dimensions)
    console.log(file.root.variables.var1.dimensions[0])
    var res = file.root.variables.var1.readSlice(0, 4)
    var results = Array.from(res);
    expect(results).to.deep.equal([420, 197, 391.5, 399]);
    console.log(res);
    file.close();
  });
  it("should read a strided slice", function () {
    var file = new netcdf4.File(fixture, "r");
    var results = Array.from(
      file.root.variables.var1.readStridedSlice(0, 2, 2)
    );
    expect(results).to.deep.equal([420, 391.5]);
  });
});
