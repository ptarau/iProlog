
package iProlog;
import java.util.*;

final class IMap<K> implements java.io.Serializable {
  private static final long serialVersionUID = 1L;

  private final HashMap<K, IntMap> map;

  IMap() {
    map = new HashMap<K, IntMap>();
  }

  public final void clear() {
    map.clear();
  }

  final boolean put(final K key, final int val) {
    IntMap vals = map.get(key);
    if (null == vals) {
      vals = new IntMap();
      map.put(key, vals);
    }
    return vals.add(val);
  }

  final IntMap get(final K key) {
    IntMap s = map.get(key);
    if (null == s) {
      s = new IntMap();
    }
    return s;
  }

  final boolean remove(final K key, final int val) {
    final IntMap vals = get(key);
    final boolean ok = vals.delete(val);
    if (vals.isEmpty()) {
      map.remove(key);
    }
    return ok;
  }

  public final boolean remove(final K key) {
    return null != map.remove(key);
  }

  public final int size() {
    final Iterator<K> I = map.keySet().iterator();
    int s = 0;
    while (I.hasNext()) {
      final K key = I.next();
      final IntMap vals = get(key);
      s += vals.size();
    }
    return s;
  }

  public final Set<K> keySet() {
    return map.keySet();
  }

  public final Iterator<K> keyIterator() {
    return keySet().iterator();
  }

  @Override
  public String toString() {
    return map.toString();
  }

  // specialization for array of int maps

  final static IMap<Integer>[] create(final int l) {
    final IMap<Integer> first = new IMap<Integer>();
    @SuppressWarnings("unchecked")
    final IMap<Integer>[] imaps = (IMap<Integer>[]) java.lang.reflect.Array.newInstance(first.getClass(), l);
    //new IMap[l];
    imaps[0] = first;
    for (int i = 1; i < l; i++) {
      imaps[i] = new IMap<Integer>();
    }
    return imaps;
  }

  final static boolean put(final IMap<Integer>[] imaps, final int pos, final int key, final int val) {
    return imaps[pos].put(new Integer(key), val);
  }

  final static int[] get(final IMap<Integer>[] iMaps, final IntMap[] vmaps, final int[] keys) {
    final int l = iMaps.length;
    final ArrayList<IntMap> ms = new ArrayList<IntMap>();
    final ArrayList<IntMap> vms = new ArrayList<IntMap>();

    for (int i = 0; i < l; i++) {
      final int key = keys[i];
      if (0 == key) {
        continue;
      }
      //Main.pp("i=" + i + " ,key=" + key);
      final IntMap m = iMaps[i].get(new Integer(key));
      //Main.pp("m=" + m);
      ms.add(m);
      vms.add(vmaps[i]);
    }
    final IntMap[] ims = new IntMap[ms.size()];
    final IntMap[] vims = new IntMap[vms.size()];

    for (int i = 0; i < ims.length; i++) {
      final IntMap im = ms.get(i);
      ims[i] = im;
      final IntMap vim = vms.get(i);
      vims[i] = vim;
    }

    //Main.pp("-------ims=" + Arrays.toString(ims));
    //Main.pp("-------vims=" + Arrays.toString(vims));

    final IntStack cs = IntMap.intersect(ims, vims); // $$$ add vmaps here
    final int[] is = cs.toArray();
    for (int i = 0; i < is.length; i++) {
      is[i] = is[i] - 1;
    }
    java.util.Arrays.sort(is);
    return is;
  }

  final static String show(final IMap<Integer>[] imaps) {
    return Arrays.toString(imaps);
  }

  final static String show(final int[] is) {
    return Arrays.toString(is);
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
