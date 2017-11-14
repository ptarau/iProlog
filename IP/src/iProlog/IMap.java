package iProlog;

import java.util.*;

final class IMap<K> implements java.io.Serializable {
    private static final long serialVersionUID = 1L;

    private final HashMap<K, IntMap> map;

    private IMap() {
        map = new HashMap<>();
    }

    static final IMap<Integer>[] create(int l) {
        IMap<Integer> first = new IMap<>();
        @SuppressWarnings("unchecked") IMap<Integer>[] imaps = (IMap<Integer>[]) java.lang.reflect.Array.newInstance(first.getClass(), l);
        //new IMap[l];
        imaps[0] = first;
        for (int i = 1; i < l; i++) {
            imaps[i] = new IMap<>();
        }
        return imaps;
    }

    static final boolean put(IMap<Integer>[] imaps, int pos, int key, int val) {
        return imaps[pos].put(key, val);
    }

    static final int[] get(IMap<Integer>[] iMaps, IntMap[] vmaps, int[] keys) {
        int l = iMaps.length;
        ArrayList<IntMap> ms = new ArrayList<>();
        ArrayList<IntMap> vms = new ArrayList<>();

        for (int i = 0; i < l; i++) {
            int key = keys[i];
            if (0 == key) {
                continue;
            }
            //Main.pp("i=" + i + " ,key=" + key);
            IntMap m = iMaps[i].get(key);
            //Main.pp("m=" + m);
            ms.add(m);
            vms.add(vmaps[i]);
        }
        IntMap[] ims = new IntMap[ms.size()];
        IntMap[] vims = new IntMap[vms.size()];

        for (int i = 0; i < ims.length; i++) {
            IntMap im = ms.get(i);
            ims[i] = im;
            IntMap vim = vms.get(i);
            vims[i] = vim;
        }

        //Main.pp("-------ims=" + Arrays.toString(ims));
        //Main.pp("-------vims=" + Arrays.toString(vims));

        IntStack cs = IntMap.intersect(ims, vims); // $$$ add vmaps here
        int[] is = cs.toArray();
        for (int i = 0; i < is.length; i++) {
            is[i] -= 1;
        }
        java.util.Arrays.sort(is);
        return is;
    }

    static final String show(IMap<Integer>[] imaps) {
        return Arrays.toString(imaps);
    }

    static final String show(int[] is) {
        return Arrays.toString(is);
    }

    public final void clear() {
        map.clear();
    }

    final boolean put(K key, int val) {
        IntMap vals = map.computeIfAbsent(key, k -> new IntMap());
        return vals.add(val);
    }

    final IntMap get(K key) {
        IntMap s = map.get(key);
        if (null == s) {
            s = new IntMap();
        }
        return s;
    }

    final boolean remove(K key, int val) {
        IntMap vals = get(key);
        boolean ok = vals.delete(val);
        if (vals.isEmpty()) {
            map.remove(key);
        }
        return ok;
    }

    // specialization for array of int maps

    public final boolean remove(K key) {
        return null != map.remove(key);
    }

    public final int size() {
        Iterator<K> I = map.keySet().iterator();
        int s = 0;
        while (I.hasNext()) {
            K key = I.next();
            IntMap vals = get(key);
            s += vals.size();
        }
        return s;
    }

    private Set<K> keySet() {
        return map.keySet();
    }

    public final Iterator<K> keyIterator() {
        return keySet().iterator();
    }

    @Override
    public String toString() {
        return map.toString();
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

// end
