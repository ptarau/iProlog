

// Should this extend something? What does StreamTokenizer do?
// var Toks = class Toks {

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
  * TODO: research the functions used in this method
  *
  * @param R - The read in string from makeToks
  */
  function Toks(s){
    var Tokenizer = require('tokenizer');
    var mycallback;
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
    t.addRule(/^\*$/, 'ordinary');
    t.addRule(/^\+$/, 'ordinary');
    t.addRule(/^,$/, 'ordinary');
    t.addRule(/^-$/, 'ordinary');
    t.addRule(/^\/$/, 'ordinary');
    t.addRule(/^:$/, 'ordinary');
    t.addRule(/^;$/, 'ordinary');
    t.addRule(/^<$/, 'ordinary');
    t.addRule(/^=$/, 'ordinary');
    t.addRule(/^>$/, 'ordinary');
    t.addRule(/^\?$/, 'ordinary');
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
    t.addRule(/^%$/, 'ordinary');

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
    t.addRule(/^\\*$/, 'comment');
    t.addRule(/^\*\/$/, 'comment');

    //slashStarComments(true)
    //slashSlashComments(true)
	  
  }

  function getWord(){
    var t = "";
    
    var eos = require('end-of-stream');

    try{
      c = nextToken;
      while(c == " " && c != TT_EOF){
        c = nextToken;
      }
    } 
    catch(err){
      return "*** tokenizer error:" + t;
    }

    switch(c)
    {
      case TT_WORD:
        var first = sval.charAt(0);
        if(first == first.toUpperCase() || '_' == first){
          t = "v:" + sval;
        } else {
          try{
            var n = parseInt(sval);
            if(Math.abs(n) < 1 << 28){
              t = "n:" + sval;
            }else{
              t = "c:" + sval;
            }
          }
          catch(err){
            t = "c:" + sval;
          }
        }
      break;
      case eos:
        t = null;
      break;
      default:
        t = "" + c;
    }
    return t;
  }

  function toSentences(s, fromFile){
    var Wsss = {};
    var Wss = {};
    var Ws = [];
    var w = "";

    toks = makeToks(s, fromFile);
    t = "";
    while(null != (t = getWord())){
      if(DOT == t){
        Wss.push(Ws);
        Wsss.push(Wss);
        Wss = {};
        ws = [];
      }
      else if((("c:") + IF) == t){
        Wss.add(Ws);
        Ws = [];
      }
      else if((("c:") + AND) == t){
        Wss.push(Ws);
        Ws = [];
      }
      else if((("c:") + HOLDS) == t){
        w = Ws[0];
        Ws.splice(0, 0, "h: " + w.substring(2));
      }
      else if((("c:") + LISTS) == t){
        w = Ws[0];
        Ws.splice(0,0, "l:" + w.substring(2));
      }
      else if((("c:") + IS) == t){
        w = Ws[0];
        Ws.splice(0,0,"f:" + w.substring(2));
      }
      else{
        Ws.push(t);
      }
    }
    return Wsss;
  }

  function toString(Wsss){
    console.log(Wsss);
  }

  exports.main = function(){
    return true;
    toSentences("prog.nl", true);
  }

