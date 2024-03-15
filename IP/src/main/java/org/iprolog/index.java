package org.iprolog;


import java.util.ArrayList;
import java.util.Arrays;

public class index {
    /* imaps - contains indexes for up to MAXIND>0 arg positions (0 for pred symbol itself)
     */
    final IMap<Integer>[] imaps;

    /* vmaps - contains clause numbers for which vars occur in indexed arg positions
     */
    final IntMap[] vmaps;

        index(Clause[] clauses) {
            vmaps = vcreate(Engine.MAXIND);
            imaps = make_index(clauses, vmaps);
        }

        boolean is_empty() { return imaps.length == 0; }

        public static IntMap[] vcreate(final int l) {
            final IntMap[] vss = new IntMap[l];
            for (int i = 0; i < l; i++) {
                vss[i] = new IntMap();
            }
            return vss;
        }

    final IMap<Integer>[] make_index(final Clause[] clauses, final IntMap[] vmaps) {

        // Prog.println ("Entered index() with START_INDEX=" + START_INDEX);
        // Prog.println ("  clauses.length=" + clauses.length);
        if (clauses.length < Engine.START_INDEX)
            return null;

        final IMap<Integer>[] imaps = IMap.create(vmaps.length);
        for (int i = 0; i < clauses.length; i++) {
            final Clause c = clauses[i];

            // Prog.println ("C["+i+"]="+c.toString());
            put(imaps, vmaps, c.index_vector, i + 1); // $$$ UGLY INC

        }
        Main.pp("INDEX");
        Main.pp(IMap.show(imaps));
        Main.pp(Arrays.toString(vmaps));
        Main.pp("");
        return imaps;
    }

    private void put(final IMap<Integer>[] imaps, final IntMap[] vss, final int[] keys, final int val) {
        for (int i = 0; i < imaps.length; i++) {
            final int key = keys[i];
            if (key != 0) {
                Main.pp("put: keys[" + i + "] -- IMap.put(imaps," + i + "," + key + "," + val + ")");
                IMap.put(imaps, i, key, val);
            } else {
                Main.pp("put: keys[" + i + "] -- vss[" + i + "].add(" + val + ")");
                vss[i].add(val);
            }
        }
    }

    final int[] get(final int[] keys) {
        final int l = imaps.length;
        final ArrayList<IntMap> ms = new ArrayList<IntMap>();
        final ArrayList<IntMap> vms = new ArrayList<IntMap>();

        for (int i = 0; i < l; i++) {
            final int key = keys[i];
            if (0 == key) {
                continue;
            }
            //Main.pp("i=" + i + " ,key=" + key);
            final IntMap m = imaps[i].get(new Integer(key));
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

        // Main.pp("-------ims=" + Arrays.toString(ims));
        // Main.pp("-------vims=" + Arrays.toString(vims));

        final IntStack cs = IntMap.intersect(ims, vims); // $$$ add vmaps here
        final int[] is = cs.toArray();
        for (int i = 0; i < is.length; i++) {
            is[i] = is[i] - 1;
        }
        java.util.Arrays.sort(is);
        return is;
    }

    /**
     * Tests if the head of a clause, not yet copied to the heap
     * for execution, could possibly match the current goal, an
     * abstraction of which has been placed in index_vector.
     * ("abstraction of which"???)
     * Supposedly, none of these "abstractions" can == -1
     */
    private final boolean possible_match(final int[] index_vector, final Clause C0) {
        for (int i = 0; i < Engine.MAXIND; i++) {
            final int x = index_vector[i];
            final int y = C0.index_vector[i];
            if (0 == x || 0 == y) {
                continue;
            }
            if (x != y)
                return false;
        }
        // Prog.println("*** possible match found");
        return true;
    }
}
