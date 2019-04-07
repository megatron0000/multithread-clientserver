#ifndef __PERMUTATION__
#define __PERMUTATION__

#include <iostream>
#include <vector>
using namespace std;

struct NoIndexFoundForPermutation {};

enum CornerCubieIndex {
    URF,
    ULF,
    DRF,
    DLF,
    URB,
    ULB,
    DRB,
    DLB,
    CornerCubieLength
};
const string CornerCubieName[8] = {"URF", "ULF", "DRF", "DLF",
                                   "URB", "ULB", "DRB", "DLB"};

// UDSlice edges are the last 4 ones (convenient for UDSliceCoordinate)
enum EdgeCubieIndex {
    UF,
    DF,
    UB,
    DB,
    UL,
    UR,
    DL,
    DR,
    LF,
    RB,
    LB,
    RF,
    EdgeCubieLength
};

const string EdgeCubieName[12] = {"UF", "DF", "UB", "DB", "UL", "UR",
                                  "DL", "DR", "LF", "RB", "LB", "RF"};

struct CubiePermutation {
    unsigned short int replaced_by;
    unsigned short int orientation;
};

// Orientations are: I, G, G2, Re, GRe, G2Re (Re is "refletido", G is "girado no
// sentido horário" and I is "identidade"). These are only for corners (edges
// are simple mod2 arithmetic)
enum CornerOrientation { I, G, G2, Re, GRe, G2Re };
unsigned short int orientation_sum[][6] = {
    {0, 1, 2, 3, 4, 5}, {1, 2, 0, 4, 5, 3}, {2, 0, 1, 5, 3, 4},
    {3, 5, 4, 0, 2, 1}, {4, 3, 5, 1, 0, 2}, {5, 4, 3, 2, 1, 0}};

// Possible performance bottleneck: copying done by mult and equals
struct Permutation {
    CubiePermutation corners[CornerCubieLength];
    CubiePermutation edges[EdgeCubieLength];
    static Permutation mult(const Permutation a, const Permutation b) {
        Permutation res;
        // corners
        for (unsigned short int i = URF; i <= DLB; i++) {
            // calc replaced_by
            res.corners[i].replaced_by =
                a.corners[b.corners[i].replaced_by].replaced_by;
            // calc orientation
            res.corners[i].orientation =
                orientation_sum[a.corners[b.corners[i].replaced_by].orientation]
                               [b.corners[i].orientation];
        }
        // edges
        for (unsigned short int i = UF; i <= RF; i++) {
            // calc replaced by
            res.edges[i].replaced_by =
                a.edges[b.edges[i].replaced_by].replaced_by;
            // calc orientation
            res.edges[i].orientation =
                a.edges[b.edges[i].replaced_by].orientation ^
                b.edges[i].orientation;
        }
        return res;
    }
    static Permutation mult_vector(const vector<Permutation> perms) {
        Permutation res = perms[0];
        for (int i = 1; i < perms.size(); i++) {
            res = Permutation::mult(res, perms[i]);
        }
        return res;
    }
    static Permutation power(Permutation p, int times) {
        if (times == 0) {
            return Permutation::identity();
        } else if (times == 1) {
            return p;
        } else {
            return Permutation::mult(p, Permutation::power(p, times - 1));
        }
    }
    static bool equals(const Permutation a, const Permutation b) {
        // corners
        for (unsigned short int i = URF; i <= DLB; i++) {
            if (a.corners[i].orientation != b.corners[i].orientation ||
                a.corners[i].replaced_by != b.corners[i].replaced_by) {
                return false;
            }
        }
        // edges
        for (unsigned short int i = UF; i <= RF; i++) {
            if (a.edges[i].orientation != b.edges[i].orientation ||
                a.edges[i].replaced_by != b.edges[i].replaced_by) {
                return false;
            }
        }
        return true;
    }
    static Permutation identity() {
        Permutation res;
        for (int i = 0; i < CornerCubieLength; i++) {
            res.corners[i].replaced_by = i;
            res.corners[i].orientation = 0;
        }
        for (int i = 0; i < EdgeCubieLength; i++) {
            res.edges[i].replaced_by = i;
            res.edges[i].orientation = 0;
        }
        return res;
    }
    static void print(Permutation p) {
        cout << "corners: " << endl;
        for (int i = 0; i < CornerCubieLength; i++) {
            cout << CornerCubieName[i] << "->"
                 << CornerCubieName[p.corners[i].replaced_by] << "("
                 << p.corners[i].orientation << ")" << endl;
        }
        cout << "edges: " << endl;
        for (int i = 0; i < EdgeCubieLength; i++) {
            cout << EdgeCubieName[i] << "->"
                 << EdgeCubieName[p.edges[i].replaced_by] << "("
                 << p.edges[i].orientation << ")" << endl;
        }
    }
    inline static bool edge_permutation_is_even(Permutation& p) {
        bool already_seen[12]{false};
        int replaced_by;
        int cycle_length;
        int parity = 0;
        for (int i = 0; i < 12; i++) {
            if (already_seen[i]) {
                continue;
            }
            cycle_length = 1;
            already_seen[i] = true;
            replaced_by = p.edges[i].replaced_by;
            while (!already_seen[replaced_by]) {
                cycle_length++;
                already_seen[replaced_by] = true;
                replaced_by = p.edges[replaced_by].replaced_by;
            }
            parity ^= (~cycle_length) & 1;
        }
        return (parity & 1) == 0;
    }
};

enum CanonicalPermutationIndex {
    U,
    R,
    F,
    D,
    L,
    B,
    U2,
    R2,
    F2,
    D2,
    L2,
    B2,
    Ui,
    Ri,
    Fi,
    Di,
    Li,
    Bi,
    CanonicalPermutationLength
};

string CanonicalPermutationName[] = {"U",  "R",  "F",  "D",  "L",  "B",
                                     "U2", "R2", "F2", "D2", "L2", "B2",
                                     "Ui", "Ri", "Fi", "Di", "Li", "Bi"};

// Ui, Ri, Fi, Di, Li, Bi
const Permutation _FacePermutationInverse[6] = {
    {
        corners : {{replaced_by : ULF, orientation : I},
                   {replaced_by : ULB, orientation : I},
                   {replaced_by : DRF, orientation : I},
                   {replaced_by : DLF, orientation : I},
                   {replaced_by : URF, orientation : I},
                   {replaced_by : URB, orientation : I},
                   {replaced_by : DRB, orientation : I},
                   {replaced_by : DLB, orientation : I}},
        edges : {{replaced_by : UL, orientation : I},
                 {replaced_by : DF, orientation : I},
                 {replaced_by : UR, orientation : I},
                 {replaced_by : DB, orientation : I},
                 {replaced_by : UB, orientation : I},
                 {replaced_by : UF, orientation : I},
                 {replaced_by : DL, orientation : I},
                 {replaced_by : DR, orientation : I},
                 {replaced_by : LF, orientation : I},
                 {replaced_by : RB, orientation : I},
                 {replaced_by : LB, orientation : I},
                 {replaced_by : RF, orientation : I}}
    },
    {
        corners : {{replaced_by : URB, orientation : G2},
                   {replaced_by : ULF, orientation : I},
                   {replaced_by : URF, orientation : G},
                   {replaced_by : DLF, orientation : I},
                   {replaced_by : DRB, orientation : G},
                   {replaced_by : ULB, orientation : I},
                   {replaced_by : DRF, orientation : G2},
                   {replaced_by : DLB, orientation : I}},
        edges : {{replaced_by : UF, orientation : 0},
                 {replaced_by : DF, orientation : 0},
                 {replaced_by : UB, orientation : 0},
                 {replaced_by : DB, orientation : 0},
                 {replaced_by : UL, orientation : 0},
                 {replaced_by : RB, orientation : 0},
                 {replaced_by : DL, orientation : 0},
                 {replaced_by : RF, orientation : 0},
                 {replaced_by : LF, orientation : 0},
                 {replaced_by : DR, orientation : 0},
                 {replaced_by : LB, orientation : 0},
                 {replaced_by : UR, orientation : 0}}
    },
    {
        corners : {{replaced_by : DRF, orientation : G},
                   {replaced_by : URF, orientation : G2},
                   {replaced_by : DLF, orientation : G2},
                   {replaced_by : ULF, orientation : G},
                   {replaced_by : URB, orientation : I},
                   {replaced_by : ULB, orientation : I},
                   {replaced_by : DRB, orientation : I},
                   {replaced_by : DLB, orientation : I}},
        edges : {{replaced_by : RF, orientation : 1},
                 {replaced_by : LF, orientation : 1},
                 {replaced_by : UB, orientation : 0},
                 {replaced_by : DB, orientation : 0},
                 {replaced_by : UL, orientation : 0},
                 {replaced_by : UR, orientation : 0},
                 {replaced_by : DL, orientation : 0},
                 {replaced_by : DR, orientation : 0},
                 {replaced_by : UF, orientation : 1},
                 {replaced_by : RB, orientation : 0},
                 {replaced_by : LB, orientation : 0},
                 {replaced_by : DF, orientation : 1}}
    },
    {
        corners : {{replaced_by : URF, orientation : I},
                   {replaced_by : ULF, orientation : I},
                   {replaced_by : DRB, orientation : I},
                   {replaced_by : DRF, orientation : I},
                   {replaced_by : URB, orientation : I},
                   {replaced_by : ULB, orientation : I},
                   {replaced_by : DLB, orientation : I},
                   {replaced_by : DLF, orientation : I}},
        edges : {{replaced_by : UF, orientation : 0},
                 {replaced_by : DR, orientation : 0},
                 {replaced_by : UB, orientation : 0},
                 {replaced_by : DL, orientation : 0},
                 {replaced_by : UL, orientation : 0},
                 {replaced_by : UR, orientation : 0},
                 {replaced_by : DF, orientation : 0},
                 {replaced_by : DB, orientation : 0},
                 {replaced_by : LF, orientation : 0},
                 {replaced_by : RB, orientation : 0},
                 {replaced_by : LB, orientation : 0},
                 {replaced_by : RF, orientation : 0}}
    },
    {
        corners : {{replaced_by : URF, orientation : I},
                   {replaced_by : DLF, orientation : G},
                   {replaced_by : DRF, orientation : I},
                   {replaced_by : DLB, orientation : G2},
                   {replaced_by : URB, orientation : I},
                   {replaced_by : ULF, orientation : G2},
                   {replaced_by : DRB, orientation : I},
                   {replaced_by : ULB, orientation : G}},
        edges : {{replaced_by : UF, orientation : 0},
                 {replaced_by : DF, orientation : 0},
                 {replaced_by : UB, orientation : 0},
                 {replaced_by : DB, orientation : 0},
                 {replaced_by : LF, orientation : 0},
                 {replaced_by : UR, orientation : 0},
                 {replaced_by : LB, orientation : 0},
                 {replaced_by : DR, orientation : 0},
                 {replaced_by : DL, orientation : 0},
                 {replaced_by : RB, orientation : 0},
                 {replaced_by : UL, orientation : 0},
                 {replaced_by : RF, orientation : 0}}
    },
    {
        corners : {{replaced_by : URF, orientation : I},
                   {replaced_by : ULF, orientation : I},
                   {replaced_by : DRF, orientation : I},
                   {replaced_by : DLF, orientation : I},
                   {replaced_by : ULB, orientation : G2},
                   {replaced_by : DLB, orientation : G},
                   {replaced_by : URB, orientation : G},
                   {replaced_by : DRB, orientation : G2}},
        edges : {{replaced_by : UF, orientation : 0},
                 {replaced_by : DF, orientation : 0},
                 {replaced_by : LB, orientation : 1},
                 {replaced_by : RB, orientation : 1},
                 {replaced_by : UL, orientation : 0},
                 {replaced_by : UR, orientation : 0},
                 {replaced_by : DL, orientation : 0},
                 {replaced_by : DR, orientation : 0},
                 {replaced_by : LF, orientation : 0},
                 {replaced_by : UB, orientation : 1},
                 {replaced_by : DB, orientation : 1},
                 {replaced_by : RF, orientation : 0}}
    }};

Permutation* _build_face_permutation() {
    Permutation* res = new Permutation[6];
    for (int i = 0; i < 6; i++) {
        res[i] =
            Permutation::mult(Permutation::mult(_FacePermutationInverse[i],
                                                _FacePermutationInverse[i]),
                              _FacePermutationInverse[i]);
    }
    return res;
}

const Permutation* _FacePermutation = _build_face_permutation();

Permutation* _build_face_2permutation() {
    Permutation* res = new Permutation[6];
    for (int i = 0; i < 6; i++) {
        res[i] = _FacePermutationInverse[i].mult(_FacePermutationInverse[i],
                                                 _FacePermutationInverse[i]);
    }
    return res;
}

const Permutation* _Face2Permutation = _build_face_2permutation();

Permutation* _build_canonical_permutation() {
    Permutation* res = new Permutation[18];
    for (int i = 0; i < 6; i++) {
        res[i] = _FacePermutation[i];
        res[i + 6] = _Face2Permutation[i];
        res[i + 12] = _FacePermutationInverse[i];
    }
    return res;
}

const Permutation* CanonicalPermutation = _build_canonical_permutation();

int CanonicalPermutationIndex(Permutation p) {
    for (int i = 0; i < 18; i++) {
        if (Permutation::equals(CanonicalPermutation[i], p)) {
            return i;
        }
    }
    // throw NoIndexFoundForPermutation();
}

#endif