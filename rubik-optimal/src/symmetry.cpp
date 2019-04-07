#ifndef __SYMMETRY__
#define __SYMMETRY__

#include "permutation.cpp"

struct NoIndexFoundForSymmetry {};

enum FundamentalSymmetryIndex { S_F2, S_U4, S_LR2, S_URF3 };

enum SymmetryIndex {
    // First 3 are identical to FundamentalSymmetryIndex
    // S_F2,
    // S_U4,
    // S_LR2,
    IdentitySymmetryIndex = 3,
    S_OTHER2,
    S_OTHER3,
    S_OTHER4,
    S_OTHER5,
    S_OTHER6,
    S_OTHER7,
    S_OTHER8,
    S_OTHER9,
    S_OTHER10,
    S_OTHER11,
    S_OTHER12,
    S_OTHER13,
    SymmetryLength
};

Permutation FundamentalSymmetry[4] = {
    {
        corners : {{replaced_by : DLF, orientation : I},
                   {replaced_by : DRF, orientation : I},
                   {replaced_by : ULF, orientation : I},
                   {replaced_by : URF, orientation : I},
                   {replaced_by : DLB, orientation : I},
                   {replaced_by : DRB, orientation : I},
                   {replaced_by : ULB, orientation : I},
                   {replaced_by : URB, orientation : I}},
        edges : {{replaced_by : DF, orientation : 0},
                 {replaced_by : UF, orientation : 0},
                 {replaced_by : DB, orientation : 0},
                 {replaced_by : UB, orientation : 0},
                 {replaced_by : DR, orientation : 0},
                 {replaced_by : DL, orientation : 0},
                 {replaced_by : UR, orientation : 0},
                 {replaced_by : UL, orientation : 0},
                 {replaced_by : RF, orientation : 0},
                 {replaced_by : LB, orientation : 0},
                 {replaced_by : RB, orientation : 0},
                 {replaced_by : LF, orientation : 0}}
    },
    {
        corners : {{replaced_by : URB, orientation : I},
                   {replaced_by : URF, orientation : I},
                   {replaced_by : DRB, orientation : I},
                   {replaced_by : DRF, orientation : I},
                   {replaced_by : ULB, orientation : I},
                   {replaced_by : ULF, orientation : I},
                   {replaced_by : DLB, orientation : I},
                   {replaced_by : DLF, orientation : I}},
        edges : {{replaced_by : UR, orientation : 0},
                 {replaced_by : DR, orientation : 0},
                 {replaced_by : UL, orientation : 0},
                 {replaced_by : DL, orientation : 0},
                 {replaced_by : UF, orientation : 0},
                 {replaced_by : UB, orientation : 0},
                 {replaced_by : DF, orientation : 0},
                 {replaced_by : DB, orientation : 0},
                 {replaced_by : RF, orientation : 1},
                 {replaced_by : LB, orientation : 1},
                 {replaced_by : LF, orientation : 1},
                 {replaced_by : RB, orientation : 1}}
    },
    {
        corners : {{replaced_by : ULF, orientation : Re},
                   {replaced_by : URF, orientation : Re},
                   {replaced_by : DLF, orientation : Re},
                   {replaced_by : DRF, orientation : Re},
                   {replaced_by : ULB, orientation : Re},
                   {replaced_by : URB, orientation : Re},
                   {replaced_by : DLB, orientation : Re},
                   {replaced_by : DRB, orientation : Re}},
        edges : {{replaced_by : UF, orientation : 0},
                 {replaced_by : DF, orientation : 0},
                 {replaced_by : UB, orientation : 0},
                 {replaced_by : DB, orientation : 0},
                 {replaced_by : UR, orientation : 0},
                 {replaced_by : UL, orientation : 0},
                 {replaced_by : DR, orientation : 0},
                 {replaced_by : DL, orientation : 0},
                 {replaced_by : RF, orientation : 0},
                 {replaced_by : LB, orientation : 0},
                 {replaced_by : RB, orientation : 0},
                 {replaced_by : LF, orientation : 0}}
    },
    {
        corners : {{replaced_by : URF, orientation : G},
                   {replaced_by : DRF, orientation : G2},
                   {replaced_by : URB, orientation : G2},
                   {replaced_by : DRB, orientation : G},
                   {replaced_by : ULF, orientation : G2},
                   {replaced_by : DLF, orientation : G},
                   {replaced_by : ULB, orientation : G},
                   {replaced_by : DLB, orientation : G2}},
        edges : {{replaced_by : RF, orientation : 0},
                 {replaced_by : RB, orientation : 0},
                 {replaced_by : LF, orientation : 0},
                 {replaced_by : LB, orientation : 0},
                 {replaced_by : DF, orientation : 1},
                 {replaced_by : UF, orientation : 1},
                 {replaced_by : DB, orientation : 1},
                 {replaced_by : UB, orientation : 1},
                 {replaced_by : DR, orientation : 1},
                 {replaced_by : UL, orientation : 1},
                 {replaced_by : DL, orientation : 1},
                 {replaced_by : UR, orientation : 1}},
    }};

const Permutation SymmetryS_R4i = Permutation::mult_vector(
    {FundamentalSymmetry[S_URF3], FundamentalSymmetry[S_U4],
     FundamentalSymmetry[S_F2]});

const Permutation SymmetryS_R4 = Permutation::power(SymmetryS_R4i, 3);

Permutation* _build_symmetry() {
    Permutation* symmetry = new Permutation[16];
    symmetry[S_F2] = FundamentalSymmetry[S_F2];
    symmetry[S_U4] = FundamentalSymmetry[S_U4];
    symmetry[S_LR2] = FundamentalSymmetry[S_LR2];
    int index = 3;
    for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 4; k++) {
            for (int l = 0; l < 2; l++) {
                // when exactly 1 is 1 and others are 0
                if (j + k + l == 1) {
                    continue;
                }
                symmetry[index++] = Permutation::mult_vector(
                    {Permutation::power(FundamentalSymmetry[S_F2], j),
                     Permutation::power(FundamentalSymmetry[S_U4], k),
                     Permutation::power(FundamentalSymmetry[S_LR2], l)});
            }
        }
    }
    return symmetry;
}

// Only compatible simmetries
const Permutation* Symmetry = _build_symmetry();

const int CompatibleSymmetryLength = 16;

Permutation* _build_inverse_symmetry(int** inverse_index_address) {
    *inverse_index_address = new int[16];
    Permutation* res = new Permutation[16];
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (Permutation::equals(
                    Permutation::identity(),
                    Permutation::mult(Symmetry[i], Symmetry[j]))) {
                res[i] = Symmetry[j];

                /*************************************************/
                (*inverse_index_address)[i] = j;
                /*************************************************/

                break;
            }
        }
    }
    return res;
}

int* InverseSymmetryIndex;
const Permutation* InverseSymmetry =
    _build_inverse_symmetry(&InverseSymmetryIndex);

Permutation* _build_full_symmetry() {
    Permutation* res = new Permutation[48];
    int index = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                for (int l = 0; l < 2; l++) {
                    res[index++] = Permutation::mult_vector(
                        {Permutation::power(FundamentalSymmetry[S_URF3], i),
                         Permutation::power(FundamentalSymmetry[S_F2], j),
                         Permutation::power(FundamentalSymmetry[S_U4], k),
                         Permutation::power(FundamentalSymmetry[S_LR2], l)});
                }
            }
        }
    }
    return res;
}

// All 48 symmetries. Previous enum indexes are not usable
Permutation* FullSymmetry = _build_full_symmetry();

const int FullSymmetryLength = 48;

Permutation* _build_full_inverse_symmetry(int** inverse_index_address) {
    *inverse_index_address = new int[48];
    Permutation* res = new Permutation[48];
    for (int i = 0; i < 48; i++) {
        for (int j = 0; j < 48; j++) {
            if (Permutation::equals(
                    Permutation::identity(),
                    Permutation::mult(FullSymmetry[i], FullSymmetry[j]))) {
                res[i] = FullSymmetry[j];

                /*************************************************/
                (*inverse_index_address)[i] = j;
                /*************************************************/

                break;
            }
        }
    }
    return res;
}

int* FullInverseSymmetryIndex;
const Permutation* FullInverseSymmetry =
    _build_full_inverse_symmetry(&FullInverseSymmetryIndex);

Permutation SymmetryConjugate(Permutation p, int symmetry) {
    return Permutation::mult_vector(
        {Symmetry[symmetry], p, InverseSymmetry[symmetry]});
}

Permutation FullSymmetryConjugate(Permutation p, int symmetry) {
    return Permutation::mult_vector(
        {FullSymmetry[symmetry], p, FullInverseSymmetry[symmetry]});
}

// Only for one of the compatible simmetries (16 in total)
int SymmetryIndex(Permutation symmetry) {
    for (int i = 0; i < 16; i++) {
        if (Permutation::equals(Symmetry[i], symmetry)) {
            return i;
        }
    }
    // throw NoIndexFoundForSymmetry();
}

int FullSymmetryIndex(Permutation symmetry) {
    for (int i = 0; i < 48; i++) {
        if (Permutation::equals(FullSymmetry[i], symmetry)) {
            return i;
        }
    }
    // throw NoIndexFoundForSymmetry();
}

int** _build_symmetry_mult() {
    int** res = new int*[16];
    for (int i = 0; i < 16; i++) {
        res[i] = new int[16];
    }
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            res[i][j] =
                SymmetryIndex(Permutation::mult(Symmetry[i], Symmetry[j]));
        }
    }
    return res;
}

// Only for compatible symmetries
int** SymmetryMult = _build_symmetry_mult();

int** _build_canonical_permutation_conjugate() {
    int** res = new int*[18];
    for (int i = 0; i < 18; i++) {
        res[i] = new int[16];
    }
    for (int i = 0; i < 18; i++) {
        for (int j = 0; j < 16; j++) {
            res[i][j] = CanonicalPermutationIndex(
                SymmetryConjugate(CanonicalPermutation[i], j));
        }
    }
    return res;
}

// int[nº of canonical moves][nº of compatible symmetries]. Returns the
// index of the resulting canonical permutation
int** CanonicalPermutationConjugate = _build_canonical_permutation_conjugate();

int** _build_full_canonical_permutation_conjugate() {
    int** res = new int*[18];
    for (int i = 0; i < 18; i++) {
        res[i] = new int[48];
    }
    for (int i = 0; i < 18; i++) {
        for (int j = 0; j < 48; j++) {
            res[i][j] = CanonicalPermutationIndex(
                FullSymmetryConjugate(CanonicalPermutation[i], j));
        }
    }
    return res;
}

// int[nº of canonical moves][nº of full symmetries]. Returns the
// index of the resulting canonical permutation
int** FullCanonicalPermutationConjugate =
    _build_full_canonical_permutation_conjugate();

#endif