const expect = require("chai").expect;
const netcdf4 = require("..");
const { join } = require("path");

const fixture = join(__dirname, "test_hgroups.nc");
const fixture2 = join(__dirname, "test_hgroups2.nc");

describe("Attribute", function () {
  const file = new netcdf4.File(fixture, "w");
  const attributes =
    file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
  const attr = attributes["airport_dep"];

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
    const file = new netcdf4.File(fixture2, "w");
    const attr =
      file.root.subgroups.mozaic_flight_2012030419144751_ascent.attributes
        .airport_dep;
    attr.name = "changed";
    expect(attr.name).equals("changed");
    attr.name = "airport_dep";
    expect(attr.name).equals("airport_dep");
    file.close();
  });

  it("should read variable attribute name", function () {
    var file = new netcdf4.File("test/test_hgroups.nc", "r");
    var attributes =
      file.root.subgroups["mozaic_flight_2012030419144751_ascent"].variables[
        "air_press"
      ].attributes;
    expect(attributes["name"].value).to.equal("air_pressure");
  });
});
