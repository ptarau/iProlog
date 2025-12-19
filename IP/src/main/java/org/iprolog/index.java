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
            // Main.println ("vcreate(" + l + ")");
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
            put(imaps, vmaps, c.index_vector, i + 1); // $$$ UGLY INC - convert to clause no
        }
        /*
        Main.pp("INDEX: imaps");
        Main.pp(show(imaps));
        Main.pp(IMap.show(imaps));
        Main.pp("INDEX: vmaps");
        Main.pp(Arrays.toString(vmaps));
        Main.pp("");
         */
        return imaps;
    }

    private void put(final IMap<Integer>[] imaps, final IntMap[] vss, final int[] index_vec, final int cl_no) {
        for (int i = 0; i < imaps.length; i++) {
            final int drefd = index_vec[i];
            if (drefd != 0) {  // != tag(V,0)
                // Main.pp("put: index_vec[" + i + "] -- IMap.put(imaps, drefd=" + i + "," + drefd + "," + cl_no + ")");
                IMap.put(imaps, i, drefd, cl_no);
            } else {
                // Main.pp("put: index_vec[" + i + "] -- vss[" + i + "].add(" + cl_no + ")");
                vss[i].add(cl_no);
            }
        }
    }

    final int[] matching_clauses(final int[] iv) {
        final int l = imaps.length;
        final ArrayList<IntMap> ms = new ArrayList<>();
        final ArrayList<IntMap> vms = new ArrayList<>();

        // Main.println(" ==== matching_clauses: start iv loop, imaps.length=" + imaps.length);

        for (int i = 0; i < l; i++) {
            // Main.println ("  ["+i+"]="+iv[i]);
            final int vec_elt = iv[i];
            if (0 == vec_elt) { // index vectors are null-terminated if < MAXIND
                continue;       // [MT: but then why the "continue" rather than
                                // "break"?
            }
            // Main.println("  iv[" + i + "]=" + vec_elt);

            final IntMap m = imaps[i].get(new Integer(vec_elt));
            // Main.println("  ms  << " + m);
            ms.add(m);
            // Main.println("  vms << " + vmaps[i]);
            vms.add(vmaps[i]);
        }

        // Main.println(" ==== matching_clauses: rest of processing");

        // Main.pp("");
        // Main.pp ("  matching_clauses: ms[0.." + ms.size() + "]");
        // for (int i = 0; i < ms.size(); ++i) Main.pp ("    ["+i+"]: "+ms.get(i).toString());
        // Main.pp ("  matching_clauses: vms[0.." + vms.size() + "]");
        // for (int i = 0; i < vms.size(); ++i) Main.pp ("    ["+i+"]: "+vms.get(i).toString());
        // Main.pp("");

        final IntMap[] ims = new IntMap[ms.size()];
        final IntMap[] vims = new IntMap[vms.size()];

        for (int i = 0; i < ims.length; i++) {
            final IntMap im = ms.get(i);
            ims[i] = im;
            final IntMap vim = vms.get(i);
            vims[i] = vim;
        }
/*
        Main.pp("");
        Main.pp("  matching_clauses: ims[0.." + ims.length + "]");
        for (int i = 0; i < ims.length; ++i)
            Main.pp("     [" + i + "]: " + ims[i].toString());
        Main.pp("  matching_clauses: vims[0.." + vims.length + "]");
        for (int i = 0; i < vims.length; ++i)
            Main.pp("     [" + i + "]: " + vims[i].toString());
        Main.pp("");
*/
        // Main.pp("-------ims=" + Arrays.toString(ims));
        // Main.pp("-------vims=" + Arrays.toString(vims));

        final IntStack cs = IntMap.intersect(ims, vims); // $$$ add vmaps here
        // Main.println ("  after intersect: cs.size()=" + cs.size());
        final int[] is = cs.toArray();
        for (int i = 0; i < is.length; i++) {
            is[i] = is[i] - 1;  // compensate for "$$$ UGLY INC"-- back to clause index
        }
        // Main.println ("  is.length=" + is.length);
        java.util.Arrays.sort(is);

        // for (int i = 0; i < is.length; ++i) Main.println ("     is[" + i + "] = " + is[i]);

        // Main.println(" ==== matching_clauses: exiting");
        // Main.println("");
        return is;
    }

    /**
     * Tests if the head of a clause, not yet copied to the heap
     * for execution, could possibly match the current goal, an
     * abstraction of which has been placed in index_vector.
     * ("abstraction of which"???)
     * Supposedly, none of these "abstractions" can == -1
     */
    final boolean possible_match(final int[] index_vector, final Clause C0) {
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

    public static String show(IMap<Integer>[] imaps) {
        String s = "";

        for (int i = 0; i < imaps.length; ++i) {
            IMap<Integer> im = imaps[i];

            s += "  [" + i + "] = \n" + im.show() + "\n";
        }
        return s;
    }
}
