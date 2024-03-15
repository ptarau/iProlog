package org.iprolog;


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

    final void put(final IMap<Integer>[] imaps, final IntMap[] vss, final int[] keys, final int val) {
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
}
