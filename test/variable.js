const expect = require("chai").expect;
const netcdf4 = require("..");
const { tmpdir } = require("os");
const { join } = require("path");
const { copyFileSync, unlinkSync } = require("fs");

const fixture = join(__dirname, "testrh.nc");
const fixture1 = join(__dirname, "test_hgroups.nc");

describe("Variable", function () {
  let fileold,filenew;
  const tempFileOldName = join(tmpdir(), `${Date.now()}.orc`);
  const tempFileNewName = join(tmpdir(), `${Date.now()}.nrc`);

  beforeEach(function () {
    copyFileSync(fixture, tempFileOldName);
    fileold = new netcdf4.File(tempFileOldName, "w");
    copyFileSync(fixture1, tempFileNewName);
    filenew = new netcdf4.File(tempFileNewName, "w");
  });

  afterEach(function () {
    fileold.close();
    unlinkSync(tempFileOldName);
    filenew.close();
    unlinkSync(tempFileNewName);
  });

  it("should read variable params (hdf5)", function() {
    expect(filenew.root.variables).to.have.property("UTC_time");
    expect(filenew.root.variables.UTC_time.inspect()).to.be.equal('[Variable UTC_time, type unsupported, 1 dimension(s)]')
    expect(filenew.root.variables.UTC_time.id,0)
    expect(filenew.root.variables.UTC_time.name).to.be.equal('UTC_time')
    expect(filenew.root.variables.UTC_time.type).to.be.equal('unsupported')
    expect(filenew.root.variables.UTC_time.endianness).to.be.equal('native')
    expect(filenew.root.variables.UTC_time.attributes).to.have.property('name')
    expect(filenew.root.variables.UTC_time.attributes).to.have.property('unit')
    expect(filenew.root.variables.UTC_time.dimensions).to.have.length(1);
    expect(filenew.root.variables.UTC_time.dimensions[0].inspect()).to.be.equal('[Dimension recNum, length 74]')
  });

  it("should read variable params (netCDF3)", function () {
    expect(fileold.root.variables).to.have.property("var1");
    expect(fileold.root.variables.var1.inspect(),'[Variable var1, type float, 1 dimenison(s)]')
    expect(fileold.root.variables.var1.id,0)
    expect(fileold.root.variables.var1.name).to.be.equal('var1')
    expect(fileold.root.variables.var1.type).to.be.equal('float')
    expect(fileold.root.variables.var1.attributes).to.be.empty
    expect(fileold.root.variables.var1.dimensions).to.have.length(1)
    expect(fileold.root.variables.var1.dimensions[0].inspect()).to.be.equal('[Dimension dim1, length 10000]');
  });

  it("should read an existing", function () {
    var res = fileold.root.variables.var1.read(0)
    expect(res).to.be.equal(420);
  });

  it("should read a slice of existing", function () {
    var res = fileold.root.variables.var1.readSlice(0, 4)
    var results = Array.from(res);
    expect(results).to.deep.equal([420, 197, 391.5, 399]);
//    console.log(res);
  });

  it("should write an existing", function () {
    fileold.root.variables.var1.write(0,42);
    var res = fileold.root.variables.var1.read(0)
    expect(res).to.be.equal(42);
    fileold.close();
    fileold = new netcdf4.File(tempFileOldName, "w");
    res = fileold.root.variables.var1.read(0)
    expect(res).to.be.equal(42);
  });


  it("should write a slice of existing", function () {
    const varr=new Float32Array([10,10.5,20,20.5])
    fileold.root.variables.var1.writeSlice(0, 4,varr)
    var res = fileold.root.variables.var1.readSlice(0, 4)
    var results = Array.from(res);
    expect(results).to.deep.equal([10,10.5,20,20.5]);
    fileold.close();
    fileold = new netcdf4.File(tempFileOldName, "w");
    var res = fileold.root.variables.var1.readSlice(0, 4)
    var results = Array.from(res);
    expect(results).to.deep.equal([10,10.5,20,20.5]);
  });
  it("should read a strided slice", function () {
    const varr=new Float32Array([30,20.5])
    fileold.root.variables.var1.writeStridedSlice(0, 2, 2,varr)
    var results = Array.from(
      fileold.root.variables.var1.readStridedSlice(0, 2, 2)
    );
    expect(results).to.deep.equal([30, 20.5]);
    var res = fileold.root.variables.var1.readSlice(0, 4)
    results = Array.from(res);
    expect(results).to.deep.equal([30,197,20.5,399]);
    fileold.close();
    fileold = new netcdf4.File(tempFileOldName, "w");
    results = Array.from(
      fileold.root.variables.var1.readStridedSlice(0, 2, 2)
    );
    expect(results).to.deep.equal([30, 20.5]);
    res = fileold.root.variables.var1.readSlice(0, 4)
    results = Array.from(res);
    expect(results).to.deep.equal([30,197,20.5,399]);
  });
});
