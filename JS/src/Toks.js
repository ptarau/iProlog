

// Should this extend something? What does StreamTokenizer do?
var Toks = class Toks {

  IF = "if";
  AND = "and";
  DOT = ".";
  HOLDS = "holds";
  LISTS = "lists";
  IS = "is";

  /*
  * This function takes a string and reads it into a variable
  *
  * TODO: read how to take in text from a file in node.js
  *
  * @param s - a string
  * @param fromFile - boolean to determine if the string is from a file
  *
  * https://github.com/Floby/node-tokenizer
  *
  */
  function makeToks(s, fromFile){
	  
	// Read s from a file
	s = require('fs');
	s.readFile('/' + fromFile, 'utf8', function(err,data){
    if(err){
      return console.log(err);
      }
      else{
        console.log(data);
      }
  });
	
    
    var T = new Toks(s);
    return T;
  }

  /*
  *
  * TODO: research the functions used in this method
  *
  * @param R - The read in string from makeToks
  */
  function Toks(s){
    var Tokenizer = require('tokenizer');
    var t = new Tokenizer(mycallback);

    t.addRule(/^.$/, 'ordinary');
    t.addRule(/^!$/, 'ordinary');
    t.addRule(/^"$/, 'ordinary');
    t.addRule(/^#$/, 'ordinary');
    t.addRule(/^\$$/, 'ordinary');
    t.addRule(/^%$/, 'ordinary');
    t.addRule(/^&$/, 'ordinary');
    t.addRule(/^'$/, 'ordinary');
    t.addRule(/^\($/, 'ordinary');
    t.addRule(/^\)$/, 'ordinary');
    t.addRule(/^*$/, 'ordinary');
    t.addRule(/^+$/, 'ordinary');
    t.addRule(/^,$/, 'ordinary');
    t.addRule(/^-$/, 'ordinary');
    t.addRule(/^\/$/, 'ordinary');
    t.addRule(/^:$/, 'ordinary');
    t.addRule(/^;$/, 'ordinary');
    t.addRule(/^<$/, 'ordinary');
    t.addRule(/^=$/, 'ordinary');
    t.addRule(/^>$/, 'ordinary');
    t.addRule(/^?$/, 'ordinary');
    t.addRule(/^@$/, 'ordinary');
    t.addRule(/^\[$/, 'ordinary');
    t.addRule(/^\\$/, 'ordinary');
    t.addRule(/^]$/, 'ordinary');
    t.addRule(/^\^$/, 'ordinary');
    t.addRule(/^_$/, 'ordinary');
    t.addRule(/^`$/, 'ordinary');
    t.addRule(/^{$/, 'ordinary');
    t.addRule(/^\|$/, 'ordinary');
    t.addRule(/^}$/, 'ordinary');
    t.addRule(/^~$/, 'ordinary');

    t.addRule(/^a$/, 'wordchars');
    t.addRule(/^b$/, 'wordchars');
    t.addRule(/^c$/, 'wordchars');
    t.addRule(/^d$/, 'wordchars');
    t.addRule(/^e$/, 'wordchars');
    t.addRule(/^f$/, 'wordchars');
    t.addRule(/^g$/, 'wordchars');
    t.addRule(/^h$/, 'wordchars');
    t.addRule(/^i$/, 'wordchars');
    t.addRule(/^j$/, 'wordchars');
    t.addRule(/^k$/, 'wordchars');
    t.addRule(/^l$/, 'wordchars');
    t.addRule(/^m$/, 'wordchars');
    t.addRule(/^n$/, 'wordchars');
    t.addRule(/^o$/, 'wordchars');
    t.addRule(/^p$/, 'wordchars');
    t.addRule(/^q$/, 'wordchars');
    t.addRule(/^r$/, 'wordchars');
    t.addRule(/^s$/, 'wordchars');
    t.addRule(/^t$/, 'wordchars');
    t.addRule(/^u$/, 'wordchars');
    t.addRule(/^v$/, 'wordchars');
    t.addRule(/^w$/, 'wordchars');
    t.addRule(/^x$/, 'wordchars');
    t.addRule(/^y$/, 'wordchars');
    t.addRule(/^z$/, 'wordchars');

    t.addRule(/^A$/, 'wordchars');
    t.addRule(/^B$/, 'wordchars');
    t.addRule(/^C$/, 'wordchars');
    t.addRule(/^D$/, 'wordchars');
    t.addRule(/^E$/, 'wordchars');
    t.addRule(/^F$/, 'wordchars');
    t.addRule(/^G$/, 'wordchars');
    t.addRule(/^H$/, 'wordchars');
    t.addRule(/^I$/, 'wordchars');
    t.addRule(/^J$/, 'wordchars');
    t.addRule(/^K$/, 'wordchars');
    t.addRule(/^L$/, 'wordchars');
    t.addRule(/^M$/, 'wordchars');
    t.addRule(/^N$/, 'wordchars');
    t.addRule(/^O$/, 'wordchars');
    t.addRule(/^P$/, 'wordchars');
    t.addRule(/^Q$/, 'wordchars');
    t.addRule(/^R$/, 'wordchars');
    t.addRule(/^S$/, 'wordchars');
    t.addRule(/^T$/, 'wordchars');
    t.addRule(/^U$/, 'wordchars');
    t.addRule(/^V$/, 'wordchars');
    t.addRule(/^W$/, 'wordchars');
    t.addRule(/^X$/, 'wordchars');
    t.addRule(/^Y$/, 'wordchars');
    t.addRule(/^Z$/, 'wordchars');
    
    t.addRule(/^0$/, 'wordchars');
    t.addRule(/^1$/, 'wordchars');
    t.addRule(/^2$/, 'wordchars');
    t.addRule(/^3$/, 'wordchars');
    t.addRule(/^4$/, 'wordchars');
    t.addRule(/^5$/, 'wordchars');
    t.addRule(/^6$/, 'wordchars');
    t.addRule(/^7$/, 'wordchars');
    t.addRule(/^8$/, 'wordchars');
    t.addRule(/^9$/, 'wordchars');
   
   
    t.addRule(/^\/\/$/, 'comment');
    t.addRule(/^\*$/, 'comment');
    t.addRule(/^*\/$/, 'comment');
	  
  }

  function getWord(){
    var t = "";
    
    var eos = require('end-of-stream');
    
  }
}
