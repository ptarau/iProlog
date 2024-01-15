/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/
#include <unordered_map>
#include <set>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include <functional>
#include "IntMap.h"
#include "Integer.h"
#include "IMap.h"

namespace iProlog {


  Integer* IMap::put(Integer* key, int v) {
      IntMap* vals = get(key);
      if (nullptr == vals) {
          vals = new IntMap();
          map[phash(key)] = bucket(key, vals);
      }
      bool r = vals->add(v);
      return key;
  }

  IntMap* IMap::get(Integer* key) {
      size_t hki = phash(key);
      // assert(hki <= map.size());
      IntMap* s = map[hki].vals;
      if (nullptr == s)
          s = new IntMap();
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

  set<Integer *> IMap::keySet() {
      set <Integer*> s;
      for (bucket b : map)
          if (b.key != nullptr)
              s.insert(b.key);
    return s;
  }

  string IMap::toString() {
      return "map.toString() <stub>";
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

  Integer* IMap::put_(vector<IMap> &imaps, int pos, int key, int val) {

      Integer *ip = new Integer(key);
      bool r = imaps[pos].put(ip, val);
      return ip;
  }

  static vector<int> get(vector<IMap> &iMaps, vector<IntMap*> &vmaps, vector<int> keys) {
    size_t l = iMaps.size();
    assert(keys.size() <= l);
    vector<IntMap*> ms = vector<IntMap*>();
    vector<IntMap*> vms = vector<IntMap*>();

    for (int i = 0; i < l; i++) {
      int key = keys[i];
      if (0 == key) {
        continue;
      }
      IntMap *m = iMaps[i].get(new Integer(key));
      ms.emplace_back(m);
      vms.emplace_back(vmaps[i]);
    }
    vector<IntMap> ims = vector<IntMap>(ms.size());
    vector<IntMap> vims = vector<IntMap>(vms.size());

    for (int i = 0; i < ims.size(); i++) {
        IntMap* im = ms.at(i);
        ims[i] = *im;
        IntMap *vim = vms.at(i);
        vims[i] = *vim;
    }

    //Main.pp("-------ims=" + Arrays.toString(ims));
    //Main.pp("-------vims=" + Arrays.toString(vims));
    vector<int> cs;

    cs = IntMap::intersect(ims, vims); // $$$ add vmaps here

    vector<int> is /*= cs.toArray() */; {
        for (int i = 0; i < cs.size(); ++i)
            is.push_back(cs[i]);
        }
    for (int i = 0; i < is.size(); i++) {
      is[i] = is[i] - 1;
    }
    std::sort(is.begin(),is.end());
    return is;
  }

  string IMap::show(vector<IMap> &imaps) {
    // return Arrays.toString(imaps);
    for (int i = 0; i < imaps.size(); ++i) {
	int im_size = imaps[i].size();
	for (int j = 0; j < im_size; ++j) {
	    bucket &b = imaps[i].map[j];
	}
    }
    return "<IMap: show() stub>";
  }

  string IMap::show(vector<Integer *> is) {
    return "<stub: IMap show>";
    // return Arrays.toString(is);
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
