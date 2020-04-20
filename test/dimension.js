var expect = require("chai").expect,
    netcdf4 = require("../build/Release/netcdf4.node");

describe('Dimension', function() {
    const file = new netcdf4.File("test/test_hgroups.nc", "r");
    const dim = file.root.dimensions["recNum"];

    it('is an object', function() {
        expect(typeof dim).equals("object");
        expect(dim).not.null;
    });

    it('with a custom inspect method', function() {
        expect(typeof dim.inspect).equals("function");
        expect(dim.inspect()).equals("[object Dimension]");
    });

    it('should read name', function() {
        expect(dim.name).to.equal("recNum");
    });

    it('should read length', function() {
        expect(file.root.dimensions["recNum"].length).to.equal(74n);
    });
});
