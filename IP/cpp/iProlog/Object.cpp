
#include "Object.h"

namespace iProlog {

    using namespace std;

        string Object::toString() const {
            switch (type) {
            case e_nullptr:
                return "$null";
            case e_integer:
                return to_string(i);
            case e_string:
                return s;
            case e_vector: {
                string j;
                for (auto a : v) {
                    if (!j.empty())
                        j += ",";
                    j += a.toString();
                }
                return "(" + j + ")";
            }
            }
            throw logic_error("invalid term");
        }
} // end namespace
