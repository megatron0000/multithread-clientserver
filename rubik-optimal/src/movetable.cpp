#ifndef __MOVETABLE__
#define __MOVETABLE__
#include "coordinate.cpp"

int** _build_corner_orientation_move() {
    int** res = new int*[CornerOrientationCoordinateLength];
    for (int i = 0; i < CornerOrientationCoordinateLength; i++) {
        res[i] = new int[CanonicalPermutationLength];
    }
    Permutation p;
    for (int i = 0; i < CornerOrientationCoordinateLength; i++) {
        for (int j = 0; j < CanonicalPermutationLength; j++) {
            p = Permutation::mult(CornerOrientationCoordinateInverse(i),
                                  CanonicalPermutation[j]);
            res[i][j] = CornerOrientationCoordinate(p);
        }
    }

    return res;
}

// int[corner orientation coord length][canonical move count]. Returns the
// coordinate of the result
int** CornerOrientationMove = _build_corner_orientation_move();

int** _build_edge_orientation_move() {
    int** res = new int*[EdgeOrientationCoordinateLength];
    for (int i = 0; i < EdgeOrientationCoordinateLength; i++) {
        res[i] = new int[CanonicalPermutationLength];
    }
    Permutation p;
    for (int i = 0; i < EdgeOrientationCoordinateLength; i++) {
        for (int j = 0; j < CanonicalPermutationLength; j++) {
            p = Permutation::mult(EdgeOrientationCoordinateInverse(i),
                                  CanonicalPermutation[j]);
            res[i][j] = EdgeOrientationCoordinate(p);
        }
    }

    return res;
}

// int[edge orientation coord length][canonical move count]. Returns the
// coordinate of the result
int** EdgeOrientationMove = _build_edge_orientation_move();

int** _build_sym_ud_slice_sorted_representant_move() {
    int** res = new int*[UDSliceSortedClassCount];
    for (int i = 0; i < UDSliceSortedClassCount; i++) {
        res[i] = new int[CanonicalPermutationLength];
    }

    Permutation perm;
    for (int i = 0; i < UDSliceSortedClassCount; i++) {
        for (int j = 0; j < CanonicalPermutationLength; j++) {
            perm = Permutation::mult(UDSliceSortedCoordinateInverse(
                                         UDSliceSortedClass2Representant[i]),
                                     CanonicalPermutation[j]);
            res[i][j] = SymUDSliceSortedCoordinate(perm);
        }
    }

    return res;
}

// int[equivalence class count][canonical permutation length]. Returns the "sym
// coordinate" of the move result
int** SymUDSliceSortedRepresentantMove =
    _build_sym_ud_slice_sorted_representant_move();

// Returns the sym coordinate of the result
inline int SymUDSliceSortedMove(int sym_coord, int canonical_move) {
    /*  S(i) R(j) S(i)^-1 M
        = S(i) R(j) [S(i)^-1 M S(i)] S(i)^-1
        = S(i) (R(j) M1) S(i)^-1
        = S(i) S(i1) R(j1) S(i1)^-1 S(i)^-1
        = S(i2) R(j1) S(i2)^-1
     */
    const int i = SymUDSliceSortedSymmetry(sym_coord);
    const int j = SymUDSliceSortedClass(sym_coord);
    const int M1 =
        CanonicalPermutationConjugate[canonical_move][InverseSymmetryIndex[i]];

    const int RjM1 = SymUDSliceSortedRepresentantMove[j][M1];
    const int i1 = SymUDSliceSortedSymmetry(RjM1);
    const int j1 = SymUDSliceSortedClass(RjM1);

    const int i2 = SymmetryMult[i][i1];

    return j1 * SymmetryLength + i2;
}

int** _build_corner_permutation_move() {
    int** res = new int*[CornerPermutationCoordinateLength];
    for (int i = 0; i < CornerPermutationCoordinateLength; i++) {
        res[i] = new int[CanonicalPermutationLength];
    }
    Permutation inv, moved;
    for (int i = 0; i < CornerPermutationCoordinateLength; i++) {
        inv = CornerPermutationCoordinateInverse(i);
        for (int j = 0; j < CanonicalPermutationLength; j++) {
            moved = Permutation::mult(inv, CanonicalPermutation[j]);
            res[i][j] = CornerPermutationCoordinate(moved);
        }
    }
    return res;
}

int** CornerPermutationMove = _build_corner_permutation_move();

int** _build_fb_slice_sorted_move() {
    int** res = new int*[FBSliceSortedCoordinateLength];
    for (int i = 0; i < FBSliceSortedCoordinateLength; i++) {
        res[i] = new int[CanonicalPermutationLength];
    }
    Permutation inv, moved;
    for (int i = 0; i < FBSliceSortedCoordinateLength; i++) {
        inv = FBSliceSortedCoordinateInverse(i);
        for (int j = 0; j < CanonicalPermutationLength; j++) {
            moved = Permutation::mult(inv, CanonicalPermutation[j]);
            res[i][j] = FBSliceSortedCoordinate(moved);
        }
    }
    return res;
}

int** FBSliceSortedMove = _build_fb_slice_sorted_move();

int** _build_lr_slice_sorted_move() {
    int** res = new int*[LRSliceSortedCoordinateLength];
    for (int i = 0; i < LRSliceSortedCoordinateLength; i++) {
        res[i] = new int[CanonicalPermutationLength];
    }
    Permutation inv, moved;
    for (int i = 0; i < LRSliceSortedCoordinateLength; i++) {
        inv = LRSliceSortedCoordinateInverse(i);
        for (int j = 0; j < CanonicalPermutationLength; j++) {
            moved = Permutation::mult(inv, CanonicalPermutation[j]);
            res[i][j] = LRSliceSortedCoordinate(moved);
        }
    }
    return res;
}

int** LRSliceSortedMove = _build_lr_slice_sorted_move();

#endif