var expect = require("chai").expect,
    netcdf4 = require("../build/Release/netcdf4.node");

describe('Attribute', function() {
    const file = new netcdf4.File("test/test_hgroups.nc", "r");
    const attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
    const attr = attributes["airport_dep"];
    console.log(attr)
    it('is an object', function() {
        expect(typeof attr).equals("object");
        expect(attr).be.not.null;
    });

    it('with a name', function() {
        expect(attr.name).equals("airport_dep");
    });

    it('with a value', function() {
        expect(attr.value).equals("FRA");
    });

    it('value is writable', function() {
        attr.value = 'ciao';
        expect(attr.value).equals("ciao");
    });

    it('with a custom inspect', function() {
        console.log(attr.inspect)
        expect(attr.inspect()).equals("[object Attribute]");
    });
/*
        it('property name is writable', function() {
            attr.name = 'other';
            expect(attr.name).equals("other");
        });
*/


/*


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
*/
});
