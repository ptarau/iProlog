var assert = require('assert');
var expect = require("chai").expect;
var tokenizer = require("../Toks.js");

describe("Tokenizer", function(){
  describe("Get Word", function(){
    it("finds words from tokens", function(){
      assert.equal(-1, [1,2,3].indexOf(4));
      //TODO: write a real test of a working getword 
    });
  });
});