const expect = require("chai").expect;
const netcdf4 = require("..");

describe("Version", function () {
    it("Has version",function() {
        expect(netcdf4).to.have.property("version");
    });
    it("Has properly filled version attributes",function() {
        const version=netcdf4.version;
        expect(version).to.have.property("major");
        expect(version).to.have.property("minor");
        expect(version).to.have.property("patch");
        expect(version).to.have.property("version");
        expect(version.version).to.be.equal(`${version.major}.${version.minor}.${version.patch}`)
    });

});