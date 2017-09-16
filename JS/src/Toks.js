// Should this extend something? What does StreamTokenizer do?
var Toks = class Toks {
  var IF = "if";
  var AND = "and";
  var DOT = ".";
  var HOLDS = "holds";
  var LISTS = "lists";
  var IS = "is";

  /*
  * This function takes a string and reads it into a variable
  *
  * TODO: read how to take in text from a file in node.js
  *
  * @param s - a string
  * @param fromFile - boolean to determine if the string is from a file
  */
  function makeToks(s, fromFile){

    if(fromFile){
      // Read s from a file
    }
    else{
      // Read s like a string
    }
    var T = new Toks(R);
    return T;
  }

  /*
  *
  * TODO: research the functions used in this method
  *
  * @param R - The read in string from makeToks
  */
  Toks(R){
  }
}
