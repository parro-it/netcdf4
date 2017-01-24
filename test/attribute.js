var expect = require("chai").expect,
    netcdf4 = require("../build/Release/netcdf4.node");

describe('Attribute', function() {
    describe('name', function() {
        it('should read group attribute name', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            var attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
            expect(attributes["airport_dep"].name).to.equal("airport_dep");
        });
    });

    describe('value', function() {
        it('should read group attribute value', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            var attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
            expect(attributes["airport_dep"].value).to.equal("FRA");
        });
    });

    describe('name', function() {
        it('should read variable attribute name', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            var attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].variables["air_press"].attributes;
            expect(attributes["name"].name).to.equal("name");
        });
    });

    describe('value', function() {
        it('should read variable attribute name', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            var attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].variables["air_press"].attributes;
            expect(attributes["name"].value).to.equal("air_pressure");
        });
    });
});
