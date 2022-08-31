const expect = require("chai").expect;
const netcdf4 = require("..");
const { copyFileSync, unlinkSync } = require("fs");
const { tmpdir } = require("os");
const { join } = require("path");
const uniqueFilename = require('unique-filename');


const fixture = join(__dirname, "test_hgroups.nc");

describe("Attribute", function () {
  let file,tempFileName;

  beforeEach(function () {
    tempFileName = uniqueFilename(tmpdir(), `rc`);
    copyFileSync(fixture, tempFileName);
    file = new netcdf4.File(tempFileName, "w");

  });

  afterEach(function () {
    file.close();
    unlinkSync(tempFileName);
  });

  it("is an object and have a name and a value", function () {
    const attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
    const attr = attributes["airport_dep"];
    expect(typeof attr).equals("object");
    expect(attr).be.not.null;
    expect(attr.name).equals("airport_dep");
    expect(attr.value).equals("FRA");
  });

  it("with a custom inspect", function () {
    const attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
    const attr = attributes["airport_dep"];
    expect(attr.inspect()).equals("[Attribute airport_dep, type char]");
  });

  it("value is writable", function () {
    let attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
    let attr = attributes["airport_dep"];
    expect(attr.value).equals("FRA");
    attr.value = "ciao";
    expect(attr.value).equals("ciao");
    attr.value = "FRQ";
    expect(attr.value).equals("FRQ");
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
    attr = attributes["airport_dep"];
    expect(attr.value).equals("FRQ");

  });

  it("name is writable and inspect value is changed", function () {
    let attr =
      file.root.subgroups.mozaic_flight_2012030419144751_ascent.attributes
        .airport_dep;
    attr.name = "changed";
    expect(attr.name).equals("changed");
    expect(attr.inspect()).equals("[Attribute changed, type char]");
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    attr =
    file.root.subgroups.mozaic_flight_2012030419144751_ascent.attributes
      .changed;
    expect(attr.name).equals("changed");
    expect(attr.inspect()).equals("[Attribute changed, type char]");
  });

  it("should read variable attribute name", function () {
    var attributes =
      file.root.subgroups["mozaic_flight_2012030419144751_ascent"].variables[
        "air_press"
      ].attributes;
    expect(attributes["name"].value).to.equal("air_pressure");
  });
});
