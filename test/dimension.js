const expect = require("chai").expect;
const netcdf4 = require("..");
const { join } = require("path");

const fixture = join(__dirname, "test_hgroups.nc");

describe("Dimension", function () {
  let file, dim;
  before(function () {
    file = new netcdf4.File(fixture, "r");
    dim = file.root.dimensions["recNum"];
  });

  after(function () {
    file.close();
  });

  it("is an object", function () {
    expect(typeof dim).equals("object");
    expect(dim).not.null;
  });

  it("with a custom inspect method", function () {
    expect(typeof dim.inspect).equals("function");
    expect(dim.inspect()).equals("[object Dimension]");
  });

  it("should read name", function () {
    expect(dim.name).to.equal("recNum");
  });

  it("should read length", function () {
    expect(file.root.dimensions["recNum"].length).to.equal(74);
  });
});
