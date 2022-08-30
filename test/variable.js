const chai = require("chai");
const expect = chai.expect;
const chaiAlmost = require('chai-almost');

chai.use(chaiAlmost(0.001));


const netcdf4 = require("..");
const { tmpdir } = require("os");
const { join } = require("path");
const { copyFileSync, unlinkSync } = require("fs");

const fixture = join(__dirname, "testrh.nc");
const fixture1 = join(__dirname, "test_hgroups.nc");

describe("Variable", function () {
  let fileold,filenew;
  const tempFileOldName = join(tmpdir(), `${Date.now()}.orc`);
  const tempFileNewName = join(tmpdir(), `${Date.now()}.nrc`);

  beforeEach(function () {
    copyFileSync(fixture, tempFileOldName);
    fileold = new netcdf4.File(tempFileOldName, "w");
    copyFileSync(fixture1, tempFileNewName);
    filenew = new netcdf4.File(tempFileNewName, "w");
  });

  afterEach(function () {
    fileold.close();
    unlinkSync(tempFileOldName);
    filenew.close();
    unlinkSync(tempFileNewName);
  });

  it("should read variable params (hdf5)", function() {
    expect(filenew.root.variables).to.have.property("UTC_time");
    expect(filenew.root.variables.UTC_time.inspect()).to.be.equal('[Variable UTC_time, type unsupported, 1 dimension(s)]')
    expect(filenew.root.variables.UTC_time.id,0)
    expect(filenew.root.variables.UTC_time.name).to.be.equal('UTC_time')
    expect(filenew.root.variables.UTC_time.type).to.be.equal('unsupported')
    expect(filenew.root.variables.UTC_time.endianness).to.be.equal('native')
    expect(filenew.root.variables.UTC_time.attributes).to.have.property('name')
    expect(filenew.root.variables.UTC_time.attributes).to.have.property('unit')
    expect(filenew.root.variables.UTC_time.dimensions).to.have.length(1);
    expect(filenew.root.variables.UTC_time.dimensions[0].inspect()).to.be.equal('[Dimension recNum, length 74]')
  });

  it("should read variable params (netCDF3)", function () {
    expect(fileold.root.variables).to.have.property("var1");
    expect(fileold.root.variables.var1.inspect(),'[Variable var1, type float, 1 dimenison(s)]')
    expect(fileold.root.variables.var1.id,0)
    expect(fileold.root.variables.var1.name).to.be.equal('var1')
    expect(fileold.root.variables.var1.type).to.be.equal('float')
    expect(fileold.root.variables.var1.attributes).to.be.empty
    expect(fileold.root.variables.var1.dimensions).to.have.length(1)
    expect(fileold.root.variables.var1.dimensions[0].inspect()).to.be.equal('[Dimension dim1, length 10000]');
  });

  it("should read an existing", function () {
    var res = fileold.root.variables.var1.read(0)
    expect(res).to.be.equal(420);
  });

  it("should rename an existing", function(){
    fileold.root.variables.var1.name = "var2";
    expect(fileold.root.variables).to.have.property("var2");
    fileold.close();
    fileold = new netcdf4.File(tempFileOldName, "w");
    expect(fileold.root.variables).to.have.property("var2");

  });


  it("should read a slice of existing", function () {
    var res = fileold.root.variables.var1.readSlice(0, 4)
    var results = Array.from(res);
    expect(results).to.deep.equal([420, 197, 391.5, 399]);
//    console.log(res);
  });

  it("should read a strided slice", function () {
    var res = fileold.root.variables.var1.readStridedSlice(0, 2, 2)
    var results = Array.from(res);
    expect(results).to.deep.equal([420, 391.5]);
  });


  it("should write an existing", function () {
    fileold.root.variables.var1.write(0,42);
    var res = fileold.root.variables.var1.read(0)
    expect(res).to.be.equal(42);
    fileold.close();
    fileold = new netcdf4.File(tempFileOldName, "w");
    res = fileold.root.variables.var1.read(0)
    expect(res).to.be.equal(42);
  });


  it("should write a slice of existing", function () {
    const varr=new Float32Array([10,10.5,20,20.5])
    fileold.root.variables.var1.writeSlice(0, 4,varr)
    var res = fileold.root.variables.var1.readSlice(0, 4)
    var results = Array.from(res);
    expect(results).to.deep.equal([10,10.5,20,20.5]);
    fileold.close();
    fileold = new netcdf4.File(tempFileOldName, "w");
    var res = fileold.root.variables.var1.readSlice(0, 4)
    var results = Array.from(res);
    expect(results).to.deep.equal([10,10.5,20,20.5]);
  });

  it("should write a strided slice", function () {
    const varr=new Float32Array([30,20.5])
    fileold.root.variables.var1.writeStridedSlice(0, 2, 2,varr)
    var results = Array.from(
      fileold.root.variables.var1.readStridedSlice(0, 2, 2)
    );
    expect(results).to.deep.equal([30, 20.5]);
    var res = fileold.root.variables.var1.readSlice(0, 4)
    results = Array.from(res);
    expect(results).to.deep.equal([30,197,20.5,399]);
    fileold.close();
    fileold = new netcdf4.File(tempFileOldName, "w");
    results = Array.from(
      fileold.root.variables.var1.readStridedSlice(0, 2, 2)
    );
    expect(results).to.deep.equal([30, 20.5]);
    res = fileold.root.variables.var1.readSlice(0, 4)
    results = Array.from(res);
    expect(results).to.deep.equal([30,197,20.5,399]);
  });

  it("should add new Variable whith set all parametrs", function(){
    //    console.log(filenew.root.dimensions);
        expect(filenew.root.variables).to.not.have.property("test_variable");
        var newVar=filenew.root.addVariable('test_variable','byte',[filenew.root.dimensions.recNum.id]);
        expect(newVar.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
        expect(filenew.root.variables).to.have.property("test_variable");
        expect(filenew.root.variables.test_variable.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
        newVar.endianness = "little";
        newVar.checksummode = "fletcher32";
        newVar.chunkmode = "chunked";
        newVar.chunksizes = new Uint32Array([8]);
        newVar.fillmode = true;
        newVar.fillvalue = 8;
        newVar.compressionshuffle = true;
        newVar.compressiondeflate = true;
        newVar.compressionlevel = 8;
        newVar.addAttribute("len", "int", 42);
        expect(newVar.attributes).to.have.property("len");
        filenew.close();
      
        filenew = new netcdf4.File(tempFileNewName, "r");
        expect(filenew.root.variables).to.have.property("test_variable");
        expect(filenew.root.variables.test_variable.inspect(),'[Variable test_variable, type byte, 1 dimension(s)]');
        newVar = filenew.root.variables.test_variable;
        expect(newVar.endianness).to.equal("little");
        expect(newVar.checksummode).to.equal("fletcher32");
        expect(newVar.chunkmode).to.equal("chunked");
        expect(newVar.chunksizes).to.deep.equal([8]);
        expect(newVar.fillmode).to.equal(true);
        expect(newVar.compressionshuffle).to.equal(true);
        expect(newVar.compressiondeflate).to.equal(true);
        expect(newVar.compressionlevel).to.equal(8);
        expect(newVar.attributes).to.have.property("len");
      });
    

  const arrTypes={
    "byte":[Int8Array,Number],
    "short":[Int16Array,Number],
    "int":[Int32Array,Number],
    "float":[Float32Array,Number],
    "double":[Float64Array,Number],
    "ubyte":[Uint8Array,Number],
    "ushort":[Uint16Array,Number],
    "uint":[Uint32Array,Number]
  };
  if (process.versions.node.split(".")[0]>=10) {
    arrTypes["uint64"]=[BigUint64Array,BigInt];
    arrTypes["int64"]=[BigInt64Array,BigInt];
  };

  const testFunc=(file,type,value,values)=>{
    const methods=arrTypes[type];

    it(`should create/read/write ${type} variable (${file})`,
      function(){
        const [fd,path]=file==='netcdf3'?[fileold,tempFileOldName]:[filenew,tempFileNewName];
        const dim=file==='netcdf3'?"dim1":"recNum"
        expect(methods).to.be.not.empty;
        expect(fd.root.variables).to.not.have.property("test_variable");
        newVar=fd.root.addVariable('test_variable',type,[dim]);
        expect(newVar.name).to.be.equal('test_variable')
        expect(newVar.type).to.be.equal(type)
        expect(newVar.read(0)).to.be.equal(newVar.fillvalue);
        newVar.write(0,methods[1](value));
        expect(newVar.read(0)).to.be.almost.eql(methods[1](value));
        const fd2=new netcdf4.File(path,'r');
        expect(fd2.root.variables).to.have.property("test_variable");
        expect(fd2.root.variables.test_variable.read(0)).to.almost.eql(methods[1](value));
        fd2.close();
      }
    )
    it(`should read/write slice ${type} variable (${file}) `,
      function() {
        const [fd,path]=file==='netcdf3'?[fileold,tempFileOldName]:[filenew,tempFileNewName];
        const dim=file==='netcdf3'?"dim1":"recNum"
        expect(methods).to.be.not.empty;
        expect(fd.root.variables).to.not.have.property("test_variable");
        newVar=fd.root.addVariable('test_variable',type,[dim]);
        newVar.writeSlice(0, 4,new methods[0](values))
        expect(Array.from(fd.root.variables.test_variable.readSlice(0,4))).to.deep.almost.equal(values);
        const fd2=new netcdf4.File(path,'r');
        expect(fd2.root.variables).to.have.property("test_variable");
        expect(Array.from(fd.root.variables.test_variable.readSlice(0,4))).to.deep.almost.equal(values);
      }
    )
    it(`should read/write strided slice ${type} variable (${file}) `,
      function() {
        const [fd,path]=file==='netcdf3'?[fileold,tempFileOldName]:[filenew,tempFileNewName];
        const dim=file==='netcdf3'?"dim1":"recNum"
        expect(methods).to.be.not.empty;
        expect(fd.root.variables).to.not.have.property("test_variable");
        newVar=fd.root.addVariable('test_variable',type,[dim]);
        newVar.writeStridedSlice(0, 2,2,new methods[0]([values[0],values[2]]))
        expect(Array.from(fd.root.variables.test_variable.readStridedSlice(0,2,2))).to.deep.almost.equal([values[0],values[2]]);
        const fd2=new netcdf4.File(path,'r');
        expect(fd2.root.variables).to.have.property("test_variable");
        expect(Array.from(fd.root.variables.test_variable.readStridedSlice(0,2,2))).to.deep.almost.equal([values[0],values[2]]);
      }
    )    
  }

  const testSuiteOld=[
    ['byte',10,[10,20,30,40]],
    ['short',1024,[20,512,333,1024]],
    ['int',100000,[0,-200,3000,555666]],
    ['float',153.2,[-12,33,55.5,106.2]],
    ['double',153.2,[-12,33,55.5,106.2]],
    ['ubyte',10,[10,20,30,40]],
    ['ushort',1024,[20,512,333,1024]],
    ['uint',100000,[0,200,3000,555666]]
  ];
  if (process.versions.node.split(".")[0]>=10) {
    testSuiteOld.push(['uint64',1024,[20n,512555n,333n,77788889n]]);
    testSuiteOld.push(['int64',100000,[0n,200n,3000n,555666n]]);
  };
  testSuiteOld.forEach(v=>testFunc('hdf5',v[0],v[1],v[2]));


});
