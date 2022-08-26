const expect = require("chai").expect;
const netcdf4 = require("..");
const { copyFileSync, unlinkSync } = require("fs");
const { tmpdir } = require("os");
const { join } = require("path");

const fixture = join(__dirname, "test_hgroups.nc");

describe("Dimension", function () {
  let file;
  const tempFileName = join(tmpdir(), `${Date.now()}.rc`);

  beforeEach(function () {
    copyFileSync(fixture, tempFileName);
    file = new netcdf4.File(tempFileName, "w");
  });

  afterEach(function () {
    file.close();
    unlinkSync(tempFileName);
  });

  it("is an object with a name and length properties and custorm inspect method", function () {
    const dim = file.root.dimensions["recNum"];
    expect(typeof dim).equals("object");
    expect(dim).not.null;
    expect(dim.name).to.equal("recNum");
    expect(file.root.dimensions["recNum"].length).to.equal(74);
    expect(typeof dim.inspect).equals("function");
    expect(dim.inspect()).equals("[Dimension recNum, length 74]");
  });

  it("can change a name properties and custorm inspect also changed", function () {
    let dim = file.root.dimensions["recNum"];
    expect(dim.name).to.equal("recNum");
    dim.name="recs";
    expect(dim.name).to.equal("recs");
    expect(dim.inspect()).equals("[Dimension recs, length 74]");
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    dim = file.root.dimensions["recs"];
    expect(dim.inspect()).equals("[Dimension recs, length 74]");
  });


});
