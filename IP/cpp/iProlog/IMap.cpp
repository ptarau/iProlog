/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

#include <iostream>
#include "index.h"
#include "IMap.h"

namespace iProlog {

  bool IMap::put(const Integer* vec_elt_obj, ClauseNumber cl_no) {
      int b = phash(vec_elt_obj);
      IntMap<int,int>* vals = map[b].vals;

      if (nullptr == vals) {
          vals = new IntMap<int,int>();
          map[b] = bucket(vec_elt_obj, vals);
      }
      return vals->add(cl_no);
  }

  IntMap<int,int>* IMap::get(const Integer* vec_elt_obj) const {
      IntMap<int,int> *s = map[phash(vec_elt_obj)].vals;
      if (s == nullptr)
	        s = new IntMap<int,int>();
      return s;
  }

  // N.B.: O(n)
  size_t IMap::size() {
    size_t s = 0;
    for (bucket b : map) {
        s += b.vals->size();
    }
    return s;
  }

#if 0
  set<Integer *> IMap::keySet() {
      set <Integer*> s;
      for (bucket b : map)
          if (b.key != nullptr)
              s.insert(b.key);
    return s;
  }
#endif

  string IMap::toString() const {
      return "IMap::toString() <stub>";
  }


  // "specialization for array of int maps"

  vector<IMap*> IMap::create(int l) {
      IMap *first = new IMap();
      vector<IMap*> imaps = vector<IMap*>(l);
      imaps[0] = first;
      for (int i = 1; i < l; i++)
          imaps[i] = new IMap;
      return imaps;
  }

  string IMap::show(const bucket &b) {
    string s = "@";
    return s;
  }

  string IMap::show() {
    string s = "{";
    string sep = "";
    for (int i = 0; i < (int) map.size(); ++i) {
	    s += sep;
	    sep = ",";
	    s += show(map[i]);
    }
    s += "}";
    return s;
  }

  string IMap::show(const vector<IMap*> &imaps) {
    // return Arrays.toString(imaps); // Java
    string s = "[";
    string sep = "";
    for (int i = 0; i < imaps.size(); ++i) {
	    s += sep;
	    sep = ",";
	    s += imaps[i]->show();
    }
    s += "]";
    return s;
  }

  string IMap::show(vector<Integer *> is) {
      string s = "{";
      for (int i = 0; i < is.size(); ++i)
          s += "<stub>";
      s += "}";

      return s;
  }

  /*
  public static void main(final String[] args) {
    final IMap<Integer>[] imaps = create(3);
    put(imaps, 0, 10, 100);
    put(imaps, 1, 20, 200);
    put(imaps, 2, 30, 777);

    put(imaps, 0, 10, 1000);
    put(imaps, 1, 20, 777);
    put(imaps, 2, 30, 3000);

    put(imaps, 0, 10, 777);
    put(imaps, 1, 20, 20000);
    put(imaps, 2, 30, 30000);

    put(imaps, 0, 10, 888);
    put(imaps, 1, 20, 888);
    put(imaps, 2, 30, 888);

    put(imaps, 0, 10, 0);
    put(imaps, 1, 20, 0);
    put(imaps, 2, 30, 0);

    //Main.pp(show(imaps));

    //final int[] keys = { 10, 20, 30 };
    //Main.pp("get=" + show(get(imaps, keys)));


    final IMap<Integer>[] m = create(4);
    Engine.put(m, new int[] { -3, -4, 0, 0 }, 0);
    Engine.put(m, new int[] { -3, -21, 0, -21 }, 1);
    Engine.put(m, new int[] { -19, 0, 0, 0 }, 2);
    
    final int[] ks = new int[] { -3, -21, -21, 0 };
    Main.pp(show(m));
    Main.pp("ks=" + Arrays.toString(ks));
    
    Main.pp("get=" + show(get(m, ks)));

  }*/
}
