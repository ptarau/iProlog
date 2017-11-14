var assert = require('assert');
var expect = require("chai").expect;
var tokenizer = require("../Toks.js");

describe("Tokenizer", function(){
  describe("To Sentences", function(){
    it("converts a file into Prolog sentences", function(){
      //assert.equal(-1, [1,2,3].indexOf(4));
      var test = tokenizer.main();

      expect(test).to.deep.equal(true);
      //TODO: write a real test of a working getword 
    });
  });
});