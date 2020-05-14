const expect = require("chai").expect;
const netcdf4 = require("..");
const { copyFileSync, unlinkSync } = require("fs");
const { tmpdir } = require("os");
const { join } = require("path");

const fixture = join(__dirname, "test_hgroups.nc");

describe("Dimension", function () {
  let file, dim, attr;
  const tempFileName = join(tmpdir(), `${Date.now()}.rc`);

  before(function () {
    copyFileSync(fixture, tempFileName);
    file = new netcdf4.File(tempFileName, "w");
    dim = file.root.dimensions["recNum"];
  });

  after(function () {
    file.close();
    unlinkSync(tempFileName);
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
