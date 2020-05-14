const expect = require("chai").expect;
const netcdf4 = require("..");
const { copyFileSync, unlinkSync } = require("fs");
const { tmpdir } = require("os");
const { join } = require("path");

const fixture = join(__dirname, "test_hgroups.nc");

describe("Attribute", function () {
  let file, attributes, attr;
  const tempFileName = join(tmpdir(), `${Date.now()}.rc`);

  before(function () {
    copyFileSync(fixture, tempFileName);
    file = new netcdf4.File(tempFileName, "w");

    attributes =
      file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
    attr = attributes["airport_dep"];
  });

  after(function () {
    file.close();
    unlinkSync(tempFileName);
  });

  it("is an object", function () {
    expect(typeof attr).equals("object");
    expect(attr).be.not.null;
  });

  it("with a name", function () {
    expect(attr.name).equals("airport_dep");
  });

  it("with a value", function () {
    expect(attr.value).equals("FRA");
  });

  it("with a custom inspect", function () {
    expect(attr.inspect()).equals("[object Attribute]");
  });

  it("value is writable", function () {
    attr.value = "ciao";
    expect(attr.value).equals("ciao");
    attr.value = "FRA";
    expect(attr.value).equals("FRA");
  });

  it("name is writable", function () {
    const tempFileName = join(tmpdir(), `${Date.now()}.rc`);

    copyFileSync(fixture, tempFileName);
    const file2 = new netcdf4.File(tempFileName, "w");

    const attr =
      file2.root.subgroups.mozaic_flight_2012030419144751_ascent.attributes
        .airport_dep;
    attr.name = "changed";
    expect(attr.name).equals("changed");
    attr.name = "airport_dep";
    expect(attr.name).equals("airport_dep");

    file2.close();
    unlinkSync(tempFileName);
  });

  it("should read variable attribute name", function () {
    const tempFileName = join(tmpdir(), `${Date.now()}.rc`);

    copyFileSync(fixture, tempFileName);
    const file2 = new netcdf4.File(tempFileName, "w");

    var attributes =
      file2.root.subgroups["mozaic_flight_2012030419144751_ascent"].variables[
        "air_press"
      ].attributes;
    expect(attributes["name"].value).to.equal("air_pressure");
    file2.close();
    unlinkSync(tempFileName);
  });
});
