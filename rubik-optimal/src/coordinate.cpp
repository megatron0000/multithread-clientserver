#ifndef __COORDINATE__
#define __COORDINATE__

#include <iostream>
#include "permutation.cpp"
#include "symmetry.cpp"

struct InvalidCombinatorial {};
struct SymCoordNotFound {};

using namespace std;

int** _build_combinatorial() {
    int** res = new int*[12];
    for (int i = 0; i < 12; i++) {
        res[i] = new int[4];
        for (int j = 0; j <= i && j <= 3; j++) {
            switch (j) {
                case 0:
                    res[i][j] = 1;
                    break;
                case 1:
                    res[i][j] = i;
                    break;
                case 2:
                    res[i][j] = (i * (i - 1)) / 2;
                    break;
                case 3:
                    res[i][j] = (i * (i - 1) * (i - 2)) / 6;
                    break;
                default:
                    // throw InvalidCombinatorial();
                    break;
            }
        }
    }
    return res;
}

// The program needs only a few combinatorial numbers ([12][4])
int** Combinatorial = _build_combinatorial();

// Ignores reflections
int CornerOrientationCoordinate(Permutation& p) {
    int factor = 1;
    int coord = 0;
    for (int i = CornerCubieLength - 2; i >= 0; i--) {
        coord += p.corners[i].orientation * factor;
        factor *= 3;
    }
    return coord;
}

const int CornerOrientationCoordinateLength = 2187;

// Does not deal with reflections
Permutation CornerOrientationCoordinateInverse(int coord) {
    Permutation res = Permutation::identity();
    int remainder = 0;
    int total = 0;
    for (int i = CornerCubieLength - 2; i >= 0; i--) {
        remainder = coord % 3;
        coord = (coord - remainder) / 3;
        res.corners[i].orientation = remainder;
        total += remainder;
    }
    res.corners[CornerCubieLength - 1].orientation = (3 - (total % 3)) % 3;
    return res;
}

int EdgeOrientationCoordinate(Permutation& p) {
    int factor = 1;
    int coord = 0;
    for (int i = EdgeCubieLength - 2; i >= 0; i--) {
        coord += p.edges[i].orientation * factor;
        factor *= 2;
    }
    return coord;
}

const int EdgeOrientationCoordinateLength = 2048;

int FBEdgeOrientationCoordinate(Permutation& p) {
    Permutation conjugate =
        Permutation::mult_vector({SymmetryS_R4i, p, SymmetryS_R4});
    return EdgeOrientationCoordinate(conjugate);
}

int LREdgeOrientationCoordinate(Permutation& p) {
    Permutation conjugate =
        Permutation::mult_vector({SymmetryS_R4i, FundamentalSymmetry[S_U4], p,
                                  InverseSymmetry[S_U4], SymmetryS_R4});
    return EdgeOrientationCoordinate(conjugate);
}

Permutation EdgeOrientationCoordinateInverse(int coord) {
    Permutation res = Permutation::identity();
    int remainder = 0;
    int total = 0;
    for (int i = EdgeCubieLength - 2; i >= 0; i--) {
        remainder = coord & 1;
        coord = coord >> 1;
        res.edges[i].orientation = remainder;
        total += remainder;
    }
    res.edges[EdgeCubieLength - 1].orientation = total & 1;
    return res;
}

int CornerPermutationCoordinate(Permutation& p) {
    int coord = 0;
    int factorial = 1;
    for (int i = 1; i < CornerCubieLength; i++) {
        int bigger = 0;
        for (int j = 0; j < i; j++) {
            if (p.corners[j].replaced_by > p.corners[i].replaced_by) {
                bigger++;
            }
        }
        coord += bigger * factorial;
        factorial *= (i + 1);
    }
    return coord;
}

const int CornerPermutationCoordinateLength = 40320;

int FBCornerOrientationCoordinate(Permutation& p) {
    Permutation conjugate =
        Permutation::mult_vector({SymmetryS_R4i, p, SymmetryS_R4});
    return CornerOrientationCoordinate(conjugate);
}

int LRCornerOrientationCoordinate(Permutation& p) {
    Permutation conjugate =
        Permutation::mult_vector({SymmetryS_R4i, FundamentalSymmetry[S_U4], p,
                                  InverseSymmetry[S_U4], SymmetryS_R4});
    return CornerOrientationCoordinate(conjugate);
}

// May generate an "unsolvable" cube becase the "evenness" of the overall
// permutation (corners + edges)  is not kept
Permutation CornerPermutationCoordinateInverse(int coord) {
    int bigger_list[8]{0};  // not applicable for index 0. It is essential that
                            // it is initialized with 0 for later processing
    int factor = 2;
    int factorial = 2;
    for (int i = 1; i < 8; i++) {
        bigger_list[i] = coord % factor;
        coord /= factor++;
    }
    bool already_taken[8]{false};
    int replaced_by[8]{-1};
    for (int i = 7; i >= 0; i--) {
        replaced_by[i] = 7 - bigger_list[i];
        for (int j = 7; j >= replaced_by[i]; j--) {
            if (already_taken[j]) {
                replaced_by[i]--;
            }
        }
        already_taken[replaced_by[i]] = true;
    }
    Permutation res = Permutation::identity();
    for (int i = 0; i < 8; i++) {
        res.corners[i].replaced_by = replaced_by[i];
    }
    return res;
}

int EdgePermutationCoordinate(Permutation& p) {
    int coord = 0;
    int factorial = 1;
    for (int i = 1; i < EdgeCubieLength; i++) {
        int bigger = 0;
        for (int j = 0; j < i; j++) {
            if (p.edges[j].replaced_by > p.edges[i].replaced_by) {
                bigger++;
            }
        }
        coord += bigger * factorial;
        factorial *= (i + 1);
    }
    return coord;
}

int UDSliceCoordinate(Permutation& p) {
    // find where UDSlice edge cubies have gone to
    int positions[4] = {-1, -1, -1, -1};
    int index = 0;
    for (int i = 0; i < EdgeCubieLength; i++) {
        int cubie = p.edges[i].replaced_by;
        if (cubie == RF || cubie == LF || cubie == RB || cubie == LB) {
            positions[index++] = i;
        }
    }

    // sort
    int aux;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < 3; i++) {
            if (positions[i] > positions[i + 1]) {
                aux = positions[i];
                positions[i] = positions[i + 1];
                positions[i + 1] = aux;
                changed = true;
            }
        }
    }

    int coord = 0;
    for (int i = 0; i < 4; i++) {
        for (int pos = positions[i] + 1;
             (i == 3 && pos < 12) || (i < 3 && pos < positions[i + 1]); pos++) {
            coord += Combinatorial[pos][i];
        }
    }

    return coord;
}

int UDSliceSortedCoordinate(Permutation& p) {
    int positions[4] = {-1, -1, -1, -1};
    // Ordering LF, RB, LB, RF is essential (cf permutation.cpp)
    for (int i = 0; i < EdgeCubieLength; i++) {
        switch (p.edges[i].replaced_by) {
            case LF:
                positions[0] = i;
                break;
            case RB:
                positions[1] = i;
                break;
            case LB:
                positions[2] = i;
                break;
            case RF:
                positions[3] = i;
                break;
            default:
                break;
        }
    }

    int coord = 0;
    int factor = 1;
    int factorial = 1;
    for (int i = 1; i < 4; i++) {
        int bigger = 0;
        for (int j = 0; j < i; j++) {
            if (positions[j] > positions[i]) {
                bigger++;
            }
        }
        coord += bigger * factorial;
        factorial *= ++factor;
    }
    return UDSliceCoordinate(p) * 24 + coord;
}

const int UDSliceSortedCoordinateLength = 11880;

int FBSliceSortedCoordinate(Permutation& p) {
    Permutation changed =
        Permutation::mult_vector({SymmetryS_R4i, p, SymmetryS_R4});
    return UDSliceSortedCoordinate(changed);
}

const int FBSliceSortedCoordinateLength = 11880;

int LRSliceSortedCoordinate(Permutation& p) {
    Permutation changed =
        Permutation::mult_vector({SymmetryS_R4i, FundamentalSymmetry[S_U4], p,
                                  InverseSymmetry[S_U4], SymmetryS_R4});
    return UDSliceSortedCoordinate(changed);
}

const int LRSliceSortedCoordinateLength = 11880;

// Cannot be used to invert CornerPermutationCoordinate simultaneously
Permutation UDSliceSortedCoordinateInverse(int coord) {
    int sorted_coord = coord % 24;
    int unsorted_coord = (coord - sorted_coord) / 24;

    // calculate positions (the non-sorted aspect of the problem, aka
    // cubies)
    int positions[4] = {-1, -1, -1, -1};
    int remaining = 4;
    int next_pos = 11;
    while (remaining > 0) {
        int sum = 0;
        while (true) {
            sum += Combinatorial[next_pos][remaining - 1];
            if (unsorted_coord >= sum) {
                next_pos--;
            } else {
                break;
            }
        }
        unsorted_coord -= sum - Combinatorial[next_pos][remaining - 1];
        positions[remaining - 1] = next_pos;
        next_pos--;
        remaining--;
    }

    // calculate ordering (the sorted aspect of the problem, aka
    // permutation)
    int bigger_list[3] = {-1, -1, -1};
    bigger_list[0] = sorted_coord % 2;
    sorted_coord = (sorted_coord - bigger_list[0]) / 2;
    bigger_list[1] = sorted_coord % 3;
    bigger_list[2] = (sorted_coord - bigger_list[1]) / 3;

    int order[4] = {-1, -1, -1, -1};
    order[3] = 3 - bigger_list[2];
    order[2] = 2 - bigger_list[1];
    if (order[2] >= order[3]) {
        order[2]++;
    }
    order[1] = 1 - bigger_list[0];
    int max = order[3] > order[2] ? order[3] : order[2];
    int min = order[3] < order[2] ? order[3] : order[2];
    if (order[1] >= min) {
        order[1]++;
    }
    if (order[1] >= max) {
        order[1]++;
    }
    order[0] = 0 + 1 + 2 + 3 - order[3] - order[2] - order[1];

    // The following relies on the fact that LF, RB, LB, RF are the last four
    // edge cubies (index-wise)
    Permutation res = Permutation::identity();
    int index = 0;
    for (int i = 0; i < EdgeCubieLength; i++) {
        if (i == positions[order[0]]) {
            res.edges[i].replaced_by = LF;
            continue;
        }
        if (i == positions[order[1]]) {
            res.edges[i].replaced_by = RB;
            continue;
        }
        if (i == positions[order[2]]) {
            res.edges[i].replaced_by = LB;
            continue;
        }
        if (i == positions[order[3]]) {
            res.edges[i].replaced_by = RF;
            continue;
        }
        res.edges[i].replaced_by = index++;
    }
    // Only even permutations (positions of corners with edges) are achievable
    // !! This should be taken care of
    if (!Permutation::edge_permutation_is_even(res)) {
        // Add one more swap so that the total permutation (edges + corners)
        // will become even
        res.corners[0].replaced_by = 1;
        res.corners[1].replaced_by = 0;
    }

    return res;
}

// Cannot be used to invert CornerPermutationCoordinate simultaneously
Permutation FBSliceSortedCoordinateInverse(int coord) {
    Permutation p = UDSliceSortedCoordinateInverse(coord);
    return Permutation::mult_vector({SymmetryS_R4, p, SymmetryS_R4i});
}

// Cannot be used to invert CornerPermutationCoordinate simultaneously
Permutation LRSliceSortedCoordinateInverse(int coord) {
    Permutation p = UDSliceSortedCoordinateInverse(coord);
    return Permutation::mult_vector({InverseSymmetry[S_U4], SymmetryS_R4, p,
                                     SymmetryS_R4i, FundamentalSymmetry[S_U4]});
}

// sets `class_count`
int* _build_ud_slice_sorted_class_2_representant(int* class_count) {
    int* res;
    bool visited[UDSliceSortedCoordinateLength];
    bool is_representant[UDSliceSortedCoordinateLength];
    for (int i = 0; i < UDSliceSortedCoordinateLength; i++) {
        visited[i] = false;
        is_representant[i] = false;
    }
    Permutation instance, next;
    int equivalence_class_count = 0;
    for (int i = 0; i < UDSliceSortedCoordinateLength; i++) {
        if (visited[i]) {
            continue;
        }
        equivalence_class_count++;
        visited[i] = true;
        is_representant[i] = true;
        instance = UDSliceSortedCoordinateInverse(i);
        for (int j = 0; j < CompatibleSymmetryLength; j++) {
            next = SymmetryConjugate(instance, j);
            visited[UDSliceSortedCoordinate(next)] = true;
        }
    }

    res = new int[equivalence_class_count];

    /*****************************************************************/
    *class_count = equivalence_class_count;
    /*****************************************************************/

    int equivalence_index = 0;
    for (int i = 0; i < UDSliceSortedCoordinateLength; i++) {
        if (is_representant[i]) {
            res[equivalence_index++] = i;
        }
    }
    return res;
}

// initialized below
int UDSliceSortedClassCount;

// Returns the UDSliceSortedCoordinate of the representant
const int* UDSliceSortedClass2Representant =
    _build_ud_slice_sorted_class_2_representant(&UDSliceSortedClassCount);

// Some permutations have intrinsic symmetries which give them more than 1
// possible sym-coordinate. This gives only 1 of these possible coordinates
int SymUDSliceSortedCoordinate(Permutation& p) {
    int p_coord = UDSliceSortedCoordinate(p);
    Permutation representant;
    Permutation candidate;
    for (int i = 0; i < UDSliceSortedClassCount; i++) {
        representant =
            UDSliceSortedCoordinateInverse(UDSliceSortedClass2Representant[i]);
        for (int j = 0; j < SymmetryLength; j++) {
            candidate = SymmetryConjugate(representant, j);
            if (p_coord == UDSliceSortedCoordinate(candidate)) {
                return i * SymmetryLength + j;
            }
        }
    }
    // throw SymCoordNotFound();
}

const int SymUDSliceSortedCoordinateLength = 16 * UDSliceSortedClassCount;

// self-expanding vector that doubles its size everytime needed
struct Vector {
    int* elements;
    int capacity;
    int pushback_index;
    Vector() : elements(new int[1]), capacity(1), pushback_index(0) {}
    inline int operator[](int index) { return elements[index]; }
    void push_back(int element) {
        if (pushback_index == capacity) {  // move
            int* new_elements = new int[2 * capacity];
            for (int i = 0; i < capacity; i++) {
                new_elements[i] = elements[i];
            }
            delete[] elements;
            elements = new_elements;
            capacity *= 2;
        }
        elements[pushback_index++] = element;
    }
    inline int size() { return pushback_index; }
    void print() {
        cout << "Vector(";
        for (int i = 0; i < size(); i++) {
            cout << elements[i];
            if (i != size() - 1) {
                cout << ",";
            }
        }
        cout << ")" << endl;
    }
};

Vector* _build_ud_slice_sorted_raw_2_sym() {
    Vector* res = new Vector[UDSliceSortedCoordinateLength];
    Permutation representant, target;
    for (int i = 0; i < UDSliceSortedClassCount; i++) {
        representant =
            UDSliceSortedCoordinateInverse(UDSliceSortedClass2Representant[i]);
        for (int j = 0; j < SymmetryLength; j++) {
            target = SymmetryConjugate(representant, j);
            res[UDSliceSortedCoordinate(target)].push_back(i * SymmetryLength +
                                                           j);
        }
    }
    return res;
}

// A Vector[ud slice sorted coordinate length], since each raw coord may have
// more than 1 possible sym coord
Vector* UDSliceSortedRaw2Sym = _build_ud_slice_sorted_raw_2_sym();

int* build_ud_slice_sorted_sym_2_raw() {
    int* res = new int[SymUDSliceSortedCoordinateLength];
    for (int i = 0; i < UDSliceSortedCoordinateLength; i++) {
        Vector syms = UDSliceSortedRaw2Sym[i];
        for (int j = 0; j < syms.size(); j++) {
            res[syms[j]] = i;
        }
    }
    return res;
}

int* UDSliceSortedSym2Raw = build_ud_slice_sorted_sym_2_raw();

inline int SymUDSliceSortedClass(int sym_ud_slice_sorted_coord) {
    return sym_ud_slice_sorted_coord >> 4;
}

inline int SymUDSliceSortedSymmetry(int sym_ud_slice_sorted_coord) {
    return sym_ud_slice_sorted_coord & 15;
}

// Inverts simultaneously two coordinates
Permutation UDSliceSortedEdgeOrientationInverse(int ud_slice_sorted_coord,
                                                int edge_orientation_coord) {
    Permutation res = UDSliceSortedCoordinateInverse(ud_slice_sorted_coord);
    // invert edge orientation manually (i.e. not using the function)
    int remainder = 0;
    int total = 0;
    for (int i = EdgeCubieLength - 2; i >= 0; i--) {
        remainder = edge_orientation_coord & 1;
        edge_orientation_coord = edge_orientation_coord >> 1;
        res.edges[i].orientation = remainder;
        total += remainder;
    }
    res.edges[EdgeCubieLength - 1].orientation = total & 1;
    return res;
}

int** _build_corner_orientation_conjugate() {
    int** res = new int*[CornerOrientationCoordinateLength];
    Permutation inv, result;
    for (int i = 0; i < CornerOrientationCoordinateLength; i++) {
        res[i] = new int[SymmetryLength];
        inv = CornerOrientationCoordinateInverse(i);
        for (int j = 0; j < SymmetryLength; j++) {
            result = SymmetryConjugate(inv, j);
            res[i][j] = CornerOrientationCoordinate(result);
        }
    }
    return res;
}

// int[corner orientation coord length][compatible symmetry length]. Returns the
// CornerOrientationCoordinate of the result
int** CornerOrientationConjugate = _build_corner_orientation_conjugate();

int*** _build_edge_orientation_conjugate() {
    int*** res = new int**[EdgeOrientationCoordinateLength];
    Permutation inv, result;
    for (int i = 0; i < EdgeOrientationCoordinateLength; i++) {
        res[i] = new int*[UDSliceSortedClassCount];
        for (int j = 0; j < UDSliceSortedClassCount; j++) {
            res[i][j] = new int[SymmetryLength];
            for (int k = 0; k < SymmetryLength; k++) {
                // Already infer we have "gone" so we want to "return" by
                // conjugating
                inv = UDSliceSortedEdgeOrientationInverse(
                    UDSliceSortedSym2Raw[(j << 4) | InverseSymmetryIndex[k]],
                    i);
                result = SymmetryConjugate(inv, k);
                res[i][j][k] = EdgeOrientationCoordinate(result);
            }
        }
    }
    return res;
}

// int[edge orientation coord length][ud slice sorted class count][compatible
// symmetry length]. Returns the EdgeOrientationCoordinate of the result. Only
// supports going "back and forth" between representant and
// symmetry-related-neighbor
int*** EdgeOrientationConjugate = _build_edge_orientation_conjugate();

#endif