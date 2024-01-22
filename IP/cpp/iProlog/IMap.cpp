/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

#include "IMap.h"
#include <iostream>

namespace iProlog {


  Integer* IMap::put(Integer* key, int v) {
      cout << endl << "          Entering IMap:put(" << key << "," << v << "):" << endl;
      IntMap<int> *vals = map[phash(key)].vals;
      if (nullptr == vals) {
          vals = new IntMap<int>();
      cout << "          Making new IntMap because vals ==null" << endl;
// it's map.put(key,vals) for a HashMap<Integer, IntMap> map in Java
          map[phash(key)] = bucket(key, vals);
      }
      cout << "                   vals->size() before add("<<v<<") = " << vals->size() << endl;
      bool r = vals->add(v);
      cout << "                   vals->size() before add("<<v<<") = " << vals->size() << endl;
      return key;
  }

  IntMap<int>* IMap::get(Integer* key) {
      IntMap<int> *s = map[phash(key)].vals;
      if (s == nullptr)
	s = new IntMap<int>();
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

  Integer* IMap::put_(vector<IMap*> &imaps, int pos, cell key, int val) {

      cout << "         entered put_(imaps,pos="<<pos <<", key="<<key.as_int()<<", val="<<val
		<<") ............" << endl;
      Integer *ip = new Integer(key.as_int());
      cout << "         before imaps["<<pos<<"].put(ip,val), size()=" << imaps[pos]->size() << endl;
      bool r = imaps[pos]->put(ip, val);
      cout << "         exiting put_ with imaps["<<pos<<"]->size() = " << imaps[pos]->size() << endl;
      return ip;
  }

  vector<int> IMap::getn(vector<IMap*> &iMaps,
	                 vector<IntMap<int>*> &vmaps,
                         vector<int> &keys) {
cout<<"Entering getn"<<endl;
    size_t l = iMaps.size();
    vector<IntMap<int>*> ms = vector<IntMap<int>*>();
    vector<IntMap<int>*> vms = vector<IntMap<int>*>();

    for (int i = 0; i < l; i++) {
      int key = keys[i];
      if (0 == key) {
        continue;
      }
cout<<"   *** in getn, about to call iMaps["<<i<<"].get(...):"<<endl;
      IntMap<int> *m = iMaps[i]->get(new Integer(key));

cout<<"   *** in getn, that get(...) returned"<<endl;
      ms.emplace_back(m);
      vms.emplace_back(vmaps[i]);
    }
    vector<IntMap<int>> ims = vector<IntMap<int>>(ms.size());
    vector<IntMap<int>> vims = vector<IntMap<int>>(vms.size());

cout<<"   *** in getn, about to enter ims loop"<<endl;
cout<<"   ***    ims.size()="<<ims.size()<<endl;
cout<<"   ***    vms.size()="<<vms.size()<<endl;
cout<<"   ***    ms.size()="<<ms.size()<<endl;
cout<<"   ***    vims.size()="<<ms.size()<<endl;
    for (int i = 0; i < ims.size(); i++) {
cout<<"   ***       IntMap<int> *im = ms.at("<<i<<") about to be called"<<endl;
        IntMap<int>* im = ms.at(i);
	if (im == nullptr) abort();
cout<<"   ***       ms["<<i<<"]=*im about to be done"<<endl;
        ims[i] = *im;
cout<<"   ***       IntMap<int> *vim = vms.at("<<i<<") about to be called"<<endl;
        IntMap<int> *vim = vms.at(i);
	if (vim == nullptr) abort();
cout<<"   ***       vims["<<i<<"]=*im about to be done"<<endl;
        vims[i] = *vim;
    }

    //Main.pp("-------ims=" + Arrays.toString(ims));
    //Main.pp("-------vims=" + Arrays.toString(vims));
    vector<int> cs;

cout<<"   *** in getn, about to call IntMap intersect<int>"<<endl;
    cs = IntMap<int>::intersect(ims, vims); // $$$ add vmaps here

cout<<"   *** in getn, about to run is-filling loop"<<endl;
    vector<int> is /*= cs.toArray() */; {
        for (int i = 0; i < cs.size(); ++i)
            is.push_back(cs[i]);
        }
cout<<"   *** in getn, is-adjusting loop"<<endl;
    for (int i = 0; i < is.size(); i++) {
      is[i] = is[i] - 1;
    }
cout<<"   *** in getn, is-sorting"<<endl;
    std::sort(is.begin(),is.end());

if(is.size()==0) cout << "     !!!!!! is=0 in IMap.getn() !!!!!!!!!"<<endl;
else             cout << "     ?????? is="<<is.size()<<" in IMap.getn()"<<endl;

    return is;
  }

  string IMap::show(bucket &b) {
    string s = "@";
    return s;
  }

  string IMap::show() {
    string s = "{";
    string sep = "";
    for (int i; i < map.size(); ++i) {
	s += sep;
	sep = ",";
	s += show(map[i]);
    }
    s += "}";
    return s;
  }

  string IMap::show(vector<IMap*> &imaps) {
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
