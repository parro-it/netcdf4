var expect = require("chai").expect,
    netcdf4 = require("../build/Release/netcdf4.node");

describe('Attribute', function() {
    describe('name', function() {
        it('should read name', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            var attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
            expect(attributes["airport_dep"].name).to.equal("airport_dep");
        });
    });

    describe('value', function() {
        it('should read value', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            var attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
            expect(attributes["airport_dep"].value).to.equal("FRA");
        });
    });
});
