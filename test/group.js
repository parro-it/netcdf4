var expect = require("chai").expect,
    netcdf4 = require("../build/Release/netcdf4.node");

describe('Group', function() {
    describe('variables', function() {
        it('should read list of variables', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            expect(file.root.variables).to.have.property("UTC_time");
        });
    });
/*
    describe('dimensions', function() {
        it('should read list of dimensions', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            expect(file.root.dimensions).to.have.property("recNum");
        });
    });
*/
    describe('subgroups', function() {
        it('should read list of subgroups', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            var subgroups = file.root.subgroups;
            expect(subgroups).to.have.property("mozaic_flight_2012030319051051_descent");
            expect(subgroups).to.have.property("mozaic_flight_2012030319051051_descent");
            expect(subgroups).to.have.property("mozaic_flight_2012030321335035_descent");
            expect(subgroups).to.have.property("mozaic_flight_2012030403540535_ascent");
            expect(subgroups).to.have.property("mozaic_flight_2012030403540535_descent");
            expect(subgroups).to.have.property("mozaic_flight_2012030412545335_ascent");
            expect(subgroups).to.have.property("mozaic_flight_2012030419144751_ascent");
            expect(subgroups["mozaic_flight_2012030419144751_ascent"].constructor.name).to.equal("Group");
        });
    });

    describe('attributes', function() {
        it('should read list of attributes', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            var attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
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
    });

    describe('name', function() {
        it('should read name', function() {
            var file = new netcdf4.File("test/test_hgroups.nc", "r");
            expect(file.root.name).to.equal("/");
        });
    });
});
