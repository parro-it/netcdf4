const expect = require("chai").expect;
const netcdf4 = require("..");
const { copyFileSync, unlinkSync } = require("fs");
const { tmpdir } = require("os");
const { join } = require("path");

const fixture = join(__dirname, "test_hgroups.nc");

describe("Group", function () {
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

  it("should read name", function () {
    expect(file.root.name).to.equal("/");
  });

  it("should read full name", function () {
    expect(file.root.fullname).to.equal("/");
  });

  it("should read id", function () {
    expect(typeof file.root.id).eq("number");
    expect(file.root.id).greaterThan(0);
  });

  it("has custom inspection", function () {
    expect(file.root.inspect()).eq("[Group /]");
  });

  it("should read list of subgroups", function () {
    var subgroups = file.root.subgroups;
    expect(subgroups).to.have.property(
      "mozaic_flight_2012030319051051_descent"
    );
    expect(subgroups).to.have.property(
      "mozaic_flight_2012030319051051_descent"
    );
    expect(subgroups).to.have.property(
      "mozaic_flight_2012030321335035_descent"
    );
    expect(subgroups).to.have.property("mozaic_flight_2012030403540535_ascent");
    expect(subgroups).to.have.property(
      "mozaic_flight_2012030403540535_descent"
    );
    expect(subgroups).to.have.property("mozaic_flight_2012030412545335_ascent");
    expect(subgroups).to.have.property("mozaic_flight_2012030419144751_ascent");
    expect(
      subgroups["mozaic_flight_2012030419144751_ascent"].constructor.name
    ).to.equal("Group");
  });


  it("should read name/fullname/subgroup of subgroups", function () {
    var subgroups = file.root.subgroups;
    expect(subgroups.mozaic_flight_2012030419144751_ascent.inspect(),'[Group mozaic_flight_2012030419144751_ascent]');
    expect(subgroups.mozaic_flight_2012030419144751_ascent.name,'mozaic_flight_2012030419144751_ascent');
    expect(subgroups.mozaic_flight_2012030419144751_ascent.fullname,'/ mozaic_flight_2012030419144751_ascent');
  });

  it("should rename groups", function () {
    var subgroup = file.root.subgroups.mozaic_flight_2012030419144751_ascent;
    subgroup.name='new_mozaic_flight';
    expect(subgroup.name,'new_mozaic_flight');
    expect(subgroup.fullname,'/ new_mozaic_flight');
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    subgroup = file.root.subgroups.new_mozaic_flight;
    expect(subgroup.name,'new_mozaic_flight');
    expect(subgroup.fullname,'/ new_mozaic_flight');

  });

  it("should create new group",function() {
    var newGroup=file.root.addSubgroup("new_group");
    expect(newGroup.name,'new_group');
    expect(newGroup.fullname,'/ new_group');
    expect(file.root.subgroups).to.have.property("new_group");
    var newSubGroup=newGroup.addSubgroup("new_nested_group");
    expect(newSubGroup.name,'new_nested_group');
    expect(newSubGroup.fullname,'/ new_group new_nested_group');
    expect(newGroup.subgroups).to.have.property("new_nested_group");
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    var subgroup = file.root.subgroups.new_group;
    expect(subgroup.name,'new_group');
    expect(subgroup.fullname,'/ new_group');
    expect(subgroup.subgroups).to.have.property("new_nested_group");

  })

  it("should read list of dimensions", function () {
    expect(file.root.dimensions).to.have.property("recNum");
  });

  it("should add new dimension",function() {
    expect(file.root.dimensions).to.not.have.property("new_dim");
    var newDim=file.root.addDimension("new_dim",10);
    expect(newDim.inspect(),"[Dimension new_dim,length 10");
    expect(file.root.dimensions).to.have.property("new_dim");
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    expect(file.root.dimensions).to.have.property("new_dim");
    expect(file.root.dimensions.new_dim.inspect(),"[Dimension new_dim,length 10");
  });

  it("should add new unlimited dimension",function() {
    expect(file.root.dimensions).to.not.have.property("new_dim");
    expect(file.root.unlimited).to.not.have.property("new_dim");
    var newDim=file.root.addDimension("new_dim",'unlimited');
    expect(newDim.inspect(),"[Dimension new_dim,length unlimited");
    expect(file.root.dimensions).to.have.property("new_dim");
    expect(file.root.unlimited).to.have.property("new_dim");
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    expect(file.root.dimensions).to.have.property("new_dim");
    expect(file.root.unlimited).to.have.property("new_dim");
    expect(file.root.dimensions.new_dim.inspect(),"[Dimension new_dim,length unlimited");
  });

  it("should read list of variables", function () {
    expect(file.root.variables).to.have.property("UTC_time");
  });

  it("should create new variable (with dimension id)", function () {
    expect(file.root.variables).to.not.have.property("test_variable");
    var newVar=file.root.addVariable('test_variable','byte',[file.root.dimensions.recNum.id]);
    expect(newVar.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
    expect(file.root.variables).to.have.property("test_variable");
    expect(file.root.variables.test_variable.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    expect(file.root.variables).to.have.property("test_variable");
    expect(file.root.variables.test_variable.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
  });

  it("should create new variable (with dimension name)", function () {
    expect(file.root.variables).to.not.have.property("test_variable");
    var newVar=file.root.addVariable('test_variable','byte',['recNum']);
    expect(newVar.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
    expect(file.root.variables).to.have.property("test_variable");
    expect(file.root.variables.test_variable.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    expect(file.root.variables).to.have.property("test_variable");
    expect(file.root.variables.test_variable.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
  });

  it("should read list of attributes", function () {
    const attributes =
      file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
    expect(attributes).to.have.property("airport_dep");
    expect(attributes).to.have.property("flight");
    expect(attributes).to.have.property("level");
    expect(attributes).to.have.property("airport_arr");
    expect(attributes).to.have.property("mission");
    expect(attributes).to.have.property("time_dep");
    expect(attributes).to.have.property("aircraft");
    expect(attributes).to.have.property("link");
    expect(attributes).to.have.property("phase");
    expect(attributes).to.have.property("time_arr");
  });

  it("should add attribute",function() {
    expect(file.root.attributes).to.not.have.property("root_attr_prop");
    const attr=file.root.addAttribute("root_attr_prop","string","root attr property");
    expect(attr.inspect(),"[Attribute root_attr_prop, type string]");
    expect(file.root.attributes).to.have.property("root_attr_prop");
    file.close();
    file = new netcdf4.File(tempFileName, "r");
    expect(file.root.attributes).to.have.property("root_attr_prop");
    expect(file.root.attributes.root_attr_prop.inspect(),"[Attribute root_attr_prop, type string]");
  })  




});
