#pragma once

#include "Engine.h"
#include "Clause.h"
#include "IntList.h"
#include "Spine.h"
#include <string>
#include <vector>
#include <limits>
#include <any>
#include <functional>
#include "stringbuilder.h"

namespace iProlog
{
	//import java.util.Arrays;
	using Stream = java::util::stream::Stream;
	using Spliterator = java::util::Spliterator;
	using Consumer = java::util::function::Consumer;

	class Prog : public Engine, public Spliterator<std::any>
	{
  public:
	  Prog(const std::wstring &fname);

	  static void pp(std::any const o);

	  static void println(std::any const o);

	  std::wstring showTerm(std::any const O) override;

	  static std::wstring maybeNull(std::any const O);

	  static bool isListCons(std::any const name);

	  static bool isOp(std::any const name);

	  static std::wstring st0(std::vector<std::any> &args);

	  virtual void ppCode();

	  virtual std::wstring showClause(Clause *const s);

	  /*
	  String showHead(final Cls s) {
	    final int h = s.gs[0];
	    return showCell(h) + "=>" + showTerm(h);
	  }
	  */

	  void ppGoals(IntList *bs) override;

	  void ppc(Spine *const S) override;

	  /////////////// end of show

	  // possibly finite Stream support

	  virtual Stream<std::any> *stream();

	  Spliterator<std::any> *trySplit() override;

	  int characteristics() override;

	  long long estimateSize() override;

	  bool tryAdvance(std::function<void(std::any*)> &action) override;

	};

}
