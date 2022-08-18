const expect = require("chai").expect;
const netcdf4 = require("..");
const { join } = require("path");

const fixture = join(__dirname, "testrh.nc");
const fixture1 = join(__dirname, "test_hgroups.nc");


describe("File", function () {
  it("should throw an error when file not found", function () {
    expect(function () {
      new netcdf4.File("DOESNOTEXIST", "r");
    }).to.throw("No such file or directory");
  });

  it("should throw an error for wrong file mode", function () {
    expect(function () {
      new netcdf4.File(fixture, "WRONG");
    }).to.throw("Unknown file mode");
  });

  it("should open a file", function () {
    const file = new netcdf4.File(fixture, "r");
    expect(typeof file).to.eq("object");
    expect(file).not.null;
    expect(file.id).not.null;
    expect(file.name).to.eq(fixture);
    expect(file.format).to.eq('netcdf3')
    expect(file.inspect()).to.eq(`[netcdf3 file ${fixture}]`)
    expect(file.closed).false;
    file.close();
    expect(file.inspect()).to.eq(`[Closed netcdf3 file ${fixture}]`)
    expect(file.closed).true;
  });

  it("should open a file (different format)", function () {
    const file = new netcdf4.File(fixture1, "r");
    expect(typeof file).to.eq("object");
    expect(file).not.null;
    expect(file.id).not.null;
    expect(file.name).to.eq(fixture1);
    expect(file.format).to.eq('hdf5')
    expect(file.inspect()).to.eq(`[hdf5 file ${fixture1}]`)
    expect(file.closed).false;
    file.close();
    expect(file.inspect()).to.eq(`[Closed hdf5 file ${fixture1}]`)
    expect(file.closed).true;
  });


  it("file contains variables", function () {
    var file = new netcdf4.File(fixture, "r");
    const vars = file.root.variables;
    const res = vars.var1.readSlice(0, 4);
    var results = Array.from(res);
    expect(results).to.deep.equal([420, 197, 391.5, 399]);
    file.close();
    expect(file.root).is.undefined;
  });
});
