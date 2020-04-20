var expect = require("chai").expect,
    netcdf4 = require("../build/Release/netcdf4.node");

describe('Attribute', function() {
    const file = new netcdf4.File("test/test_hgroups.nc", "w");
    const attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].attributes;
    const attr = attributes["airport_dep"];

    it('is an object', function() {
        expect(typeof attr).equals("object");
        expect(attr).be.not.null;
    });

    it('should read variable attribute name', function() {
        var file = new netcdf4.File("test/test_hgroups.nc", "r");
        var attributes = file.root.subgroups["mozaic_flight_2012030419144751_ascent"].variables["air_press"].attributes;
        expect(attributes["name"].value).to.equal("air_pressure");
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
        attr.value = 'FRA';
        expect(attr.value).equals("FRA");
    });

    it('with a custom inspect', function() {
        console.log(attr.inspect)
        expect(attr.inspect()).equals("[object Attribute]");
    });
/* Error: NetCDF: String match to name in use

    it('property name is writable', function() {
        try {
            attr.name = 'otherThings';
            expect(attr.name).equals("otherThings");
        } finally {
            attr.name = 'airport_dep';
            expect(attr.name).equals("airport_dep");
        }

    });
*/

});
