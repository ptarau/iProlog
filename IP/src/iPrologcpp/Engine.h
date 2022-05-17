#pragma once

//====================================================================================================
//The Free Edition of Java to C++ Converter limits conversion output to 100 lines per file.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-java-to-cplus.html
//====================================================================================================

#include "Clause.h"
#include "IntStack.h"
#include "ObStack.h"
#include "Spine.h"
#include "IMap.h"
#include "IntMap.h"
#include "IntList.h"

#include "Engine.h"

#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
// #include <any>
// #include <optional>
#include "stringbuilder.h"


using namespace std;

namespace iProlog
{

	/**
	 * Implements execution mechanism
	 */
	class Engine
	{


  public:

	  static constexpr int MAXIND = 3; // number of index args
	  static constexpr int START_INDEX = 20;
	  // switches off indexing for less then START_INDEX clauses e.g. <20

	  /**
	   * Builds a new engine from a natural-language style assembler.nl file
	   */
	  virtual ~Engine()
	  {
		  delete syms;
		  delete trail;
		  delete ustack;
		  delete spines;
		  delete query;
	  }

	  Engine(const wstring &fname);

	  /**
	   * trimmed down clauses ready to be quickly relocated to the heap
	   */
	  const vector<Clause*> clauses;

	  const vector<int> cls;
	  /** symbol table made of map + reverse map from ints to syms */

// LinkedHashMap here originally

	  unordered_map<wstring, int> *const syms;
  private:
//JAVA TO C++ CONVERTER WARNING: C++ has no equivalent to a 'final' collection which allows modification of internal state:
//ORIGINAL LINE: private final ArrayList<String> slist;
	  vector<wstring> slist;

	  /** runtime areas:
	   *
	   * the heap contains code for and clauses their their copies
	   * created during execution
	   *
	   * the trail is an undo list for variable bindings
	   * that facilitates retrying failed goals with alternative
	   * matching clauses
	   *
	   * the unification stack ustack helps handling term unification non-recursively
	   *
	   * the spines stack contains abstractions of clauses and goals and performs the
	   * functions of  both a choice-point stack and goal stack
	   *
	   * imaps: contains indexes for up toMAXIND>0 arg positions (0 for pred symbol itself)
	   *
	   * vmaps: contains clause numbers for which vars occur in indexed arg positions
	   */

	  vector<int> heap;
	  int top = 0;
  public:
	  static int MINSIZE; // power of 2

  private:
	  IntStack *const trail;
	  IntStack *const ustack;
	  ObStack<Spine*> *const spines = new ObStack<Spine*>();

  public:
	  Spine *query;

	  const vector<IMap<int>*> imaps;
	  const vector<IntMap*> vmaps;

	  /**
	   * tags of our heap cells - that can also be seen as
	   * instruction codes in a compiled implementation
	   */
  private:
	  static constexpr int V = 0;
	  static constexpr int U = 1;
	  static constexpr int R = 2;

	  static constexpr int C = 3;
	  static constexpr int N = 4;

	  static constexpr int A = 5;

	  // G - ground?

	  static constexpr int BAD = 7;

	  /**
	   * tags an integer value while fliping it into a negative
	   * number to ensure that untagged cells are always negative and the tagged
	   * ones are always positive - a simple way to ensure we do not mix them up
	   * at runtime
	   */
	  static int tag(int const t, int const w) final;

	  /**
	   * removes tag after flipping sign
	   */
	  static int detag(int const w) final;

	  /**
	   * extracts the tag of a cell
	   */
	  static int tagOf(int const w) final;

	  /**
	   * places an identifier in the symbol table
	   */
	  int addSym(const wstring &sym);

	  /**
	   * returns the symbol associated to an integer index
	   * in the symbol table
	   */
	  wstring getSym(int const w);

	  void makeHeap();

	  void makeHeap(int const size);

	  int getTop();

	  int setTop(int const top);

	  void clear();

	  /**
	   * Pushes an element - top is incremented frirst than the
	   * element is assigned. This means top point to the last assigned
	   * element - which can be returned with peek().
	   */
	  void push(int const i);

  public:
	  int size();

	  /**
	   * dynamic array operation: doubles when full
	   */
  private:
	  void expand();

	  void ensureSize(int const more);

	  /**
	  * expands a "Xs lists .." statements to "Xs holds" statements
	  */

	  static std::vector<std::vector<std::wstring>> maybeExpand(std::vector<std::wstring> &Ws) final;

	  /**
	   * expands, if needed, "lists" statements in sequence of statements
	   */
	  static std::vector<std::vector<std::wstring>> mapExpand(std::vector<std::vector<std::wstring>> &Wss) final;

	  /**
	   * loads a program from a .nl file of
	   * "natural language" equivalents of Prolog/HiLog statements
	   */
  public:
	  virtual vector<Clause*> dload(const std::wstring &s);

  private:
	  static vector<int> toNums(std::vector<Clause*> &clauses) final;

	  /*
	   * encodes string constants into symbols while leaving
	   * other data types untouched
	   */
	  int encode(int const t, const wstring &s);

	  /**
	   * true if cell x is a variable
	   * assumes that variables are tagged with 0 or 1
	   */
	  static bool isVAR(int const x) final;

	  /**
	   * returns the heap cell another cell points to
	   */
  public:
	  int getRef(int const x);

	  /*
	   * sets a heap cell to point to another one
	   */
  private:
	  void setRef(int const w, int const r);

	  /**
	   * removes binding for variable cells
	   * above savedTop
	   */
	  void unwindTrail(int const savedTop);

	  /**
	   * scans reference chains starting from a variable
	   * until it points to an unbound root variable or some
	   * non-variable cell
	   */
	  int deref(int x);

	  /**
	   * raw display of a term - to be overridden
	   */
  public:
	  virtual wstring showTerm(int const x);

	  /**
	   * raw display of an externalized term
	   */
	  /* virtual std::wstring showTerm(std::any const O); */
	  virtual wstring showTerm(void * const O);

	  /**
	   * prints out content of the trail
	   */
	  virtual void ppTrail();

	  /**
	   * builds an array of embedded arrays from a heap cell
	   * representing a term for interaction with an external function
	   * including a displayer
	   */
	  virtual void * exportTerm(int x);

	  /**
	   * extracts an integer array pointing to
	   * the skeleton of a clause: a cell
	   * pointing to its head followed by cells pointing to its body's
	   * goals
	   */
	  static vector<int> getSpine(vector<int> &cs);

	  /**
	   * raw display of a cell as tag : value
	   */
	  wstring showCell(int const w);

	  /**
	   * a displayer for cells
	   */

	  virtual wstring showCells(int const __super, int const len);

	  virtual wstring showCells(vector<int> &cs);

	  /**
	  * to be overridden as a printer of a spine
	  */
	  virtual void ppc(Spine *const C);

	  /**
	   * to be overridden as a printer for current goals
	   * in a spine
	   */
	  virtual void ppGoals(IntList *const gs);

	  /**
	   * to be overriden as a printer for spines
	   */
	  virtual void ppSpines();

	  /**
	   * unification algorithm for cells X1 and X2 on ustack that also takes care
	   * to trail bindigs below a given heap address "base"
	   */
  private:
	  bool unify(int const __super);

	  bool unify_args(int const w1, int const w2);

	  /**
	   * places a clause built by the Toks reader on the heap
	   */
  public:
	  virtual Clause *putClause(vector<int> &cs, vector<int> &gs, int const neck);

	  /**
	   * relocates a variable or array reference cell by b
	   * assumes var/ref codes V,U,R are 0,1,2
	   */
  private:
	  static int relocate(int const b, int const cell) final;

	  /**
	   * pushes slice[from,to] of array cs of cells to heap
	   */
	  void pushCells(int const b, int const from, int const to, int const __super);

	  /**
	   * pushes slice[from,to] of array cs of cells to heap
	   */
	  void pushCells(int const b, int const from, int const to, vector<int> &cs);

	  /**
	   * copies and relocates head of clause at offset from heap to heap
	   */
	  int pushHead(int const b, Clause *const C);

	  /**
	   * copies and relocates body of clause at offset from heap to heap
	   * while also placing head as the first element of array gs that
	   * when returned contains references to the toplevel spine of the clause
	   */
	  vector<int> pushBody(int const b, int const head, Clause *const C);

	  /**
	   * makes, if needed, registers associated to top goal of a Spine
	   * these registers will be reused when matching with candidate clauses
	   * note that xs contains dereferenced cells - this is done once for
	   * each goal's toplevel subterms
	   */
	  void makeIndexArgs(Spine *const G, int const goal);

	  vector<int> getIndexables(int const ref);

	  int cell2index(int const cell);

	  /**
	   * tests if the head of a clause, not yet copied to the heap
	   * for execution could possibly match the current goal, an
	   * abstraction of which has been place in xs
	   */
	  bool match(vector<int> &xs, Clause *const C0);

	  /**
	   * transforms a spine containing references to choice point and
	   * immutable list of goals into a new spine, by reducing the
	   * first goal in the list with a clause that successfully
	   * unifies with it - in which case places the goals of the
	   * clause at the top of the new list of goals, in reverse order
	   */
	  Spine *unfold(Spine *const G);

	  /**
	   * extracts a query - by convention of the form
	   * goal(Vars):-body to be executed by the engine
	   */
  public:
	  virtual Clause *getQuery();

	  /**
	   * returns the initial spine built from the
	   * query from which execution starts
	   */
	  virtual Spine *init();

	  /**
	   * returns an answer as a Spine while recording in it
	   * the top of the trail to allow the caller to retrieve
	   * more answers by forcing backtracking
	   */
  private:
	  Spine *answer(int const ttop);

	  /**
	   * detects availability of alternative clauses for the
	   * top goal of this spine
	   */

//====================================================================================================
//End of the allowed output for the Free Edition of Java to C++ Converter.

//To purchase the Premium Edition, visit our website:
//https://www.tangiblesoftwaresolutions.com/order/order-java-to-cplus.html
//====================================================================================================
