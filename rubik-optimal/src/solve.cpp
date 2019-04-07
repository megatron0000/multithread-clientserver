#ifndef __SOLVE__
#define __SOLVE__

#include <string>
#include <vector>
#include "permutation.cpp"
#include "pruningtable.cpp"

struct CubeState {
    /* next move */
    int exponent = 0;
    int axis = U;
    /* coords */
    int ud_coord;
    int fb_coord;
    int lr_coord;
    int ud_corner_orientation;
    int fb_corner_orientation;
    int lr_corner_orientation;
    int ud_edge_orientation;
    int fb_edge_orientation;
    int lr_edge_orientation;
    int ud_corner_permutation;
    /* pruning values */
    int ud_pruning;
    int fb_pruning;
    int lr_pruning;
    /* symmetry-discarding auxiliary */
    uint64_t equal_by_sequence;
};

struct CubeSolution {
    vector<Permutation> moves;
    vector<int> move_indexes;
    string move_names;
    CubeSolution(int length)
        : moves(vector<Permutation>{}),
          move_indexes(vector<int>{}),
          length(length),
          move_names{""} {}
    int length;
};

int** _build_axis_exponent_2_move() {
    int** res = new int*[6];
    for (int axis = 0; axis < 6; axis++) {
        res[axis] = new int[4];
        for (int exponent = 1; exponent < 4; exponent++) {
            res[axis][exponent] = axis + 6 * (exponent - 1);
        }
    }
    return res;
}

// int[axis=0,1,2,3,4,5][exponent=1,2,3]. returns a CanonicalPermutationIndex
int** AxisExponent2Move = _build_axis_exponent_2_move();

int* _build_canonical_permutation_discover_order() {
    int* res = new int[CanonicalPermutationLength];

    for (int i = 0; i < CanonicalPermutationLength; i++) {
        if (i < 6) {
            res[i] = 3 * i;
        } else if (i < 12) {
            res[i] = 3 * (i - 6) + 1;
        } else {
            res[i] = 3 * (i - 12) + 2;
        }
    }
    return res;
}

int* CanonicalPermutationDiscoverOrder =
    _build_canonical_permutation_discover_order();

uint64_t* _build_equal_by() {
    uint64_t* res = new uint64_t[CanonicalPermutationLength];
    uint64_t unit = 1UL;
    for (int i = 0; i < CanonicalPermutationLength; i++) {
        res[i] = 0UL;
        for (uint64_t s = 0UL; s < FullSymmetryLength; s++) {
            if (FullCanonicalPermutationConjugate[i][s] == i) {
                res[i] |= (unit << s);
            }
        }
    }
    return res;
}

// EqualBy[m] gives a 64bit number, of which the first 48 interest. For each bit
// i, if i==1, then the symmetry conjugate of m by FullSymmetry[i] is equal to m
uint64_t* EqualBy = _build_equal_by();

uint64_t* _build_greater_by() {
    uint64_t* res = new uint64_t[CanonicalPermutationLength];
    uint64_t unit = 1UL;
    for (int i = 0; i < CanonicalPermutationLength; i++) {
        res[i] = 0UL;
        for (uint64_t s = 0UL; s < FullSymmetryLength; s++) {
            if (CanonicalPermutationDiscoverOrder
                    [FullCanonicalPermutationConjugate[i][s]] <
                CanonicalPermutationDiscoverOrder[i]) {
                res[i] |= (unit << s);
            }
        }
    }
    return res;
}

// GreaterBy[m] gives a 64bit number, of which the first 48 interest. For each
// bit i, if i==1, then the symmetry conjugate of m by FullSymmetry[i] is
// smaller than m
uint64_t* GreaterBy = _build_greater_by();

enum ShouldIncrease { Axis, Exponent, Nothing };

struct ExpectedToIncreaseSomething {};

const int fb_transform = FullSymmetryIndex(SymmetryS_R4i);

const int lr_transform = FullSymmetryIndex(
    Permutation::mult(SymmetryS_R4i, FundamentalSymmetry[S_U4]));

struct DidNotSolveWithin20Moves {};

struct CubeSolver {
    CubeState states[30];
    CubeState* current = &states[0];
    int boundary_depth = 0;
    int remaining_depth = 0;
    PruningTable* table;
    uint64_t original_symmetries;

    CubeSolver(PruningTable* table) : table(table) {}

    inline void reinitialize(Permutation& p) {
        boundary_depth = 0;
        remaining_depth = 0;
        original_symmetries = 0;

        current = &states[0];
        current->equal_by_sequence = 0UL;
        for (uint64_t i = 0UL; i < FullSymmetryLength; i++) {
            if (Permutation::equals(p, FullSymmetryConjugate(p, i))) {
                current->equal_by_sequence |= (1UL << i);
            }
        }
        current->exponent = 0;
        current->axis = U;
        current->ud_coord = SymUDSliceSortedCoordinate(p);
        // current->ud_class = SymUDSliceSortedClass(current->ud_coord);
        // current->ud_symmetry = SymUDSliceSortedSymmetry(current->ud_coord);
        current->fb_coord = UDSliceSortedRaw2Sym[FBSliceSortedCoordinate(p)][0];
        // current->fb_class = SymUDSliceSortedClass(current->fb_coord);
        // current->fb_symmetry = SymUDSliceSortedSymmetry(current->fb_coord);
        current->lr_coord = UDSliceSortedRaw2Sym[LRSliceSortedCoordinate(p)][0];
        // current->lr_class = SymUDSliceSortedClass(current->lr_coord);
        // current->lr_symmetry = SymUDSliceSortedSymmetry(current->lr_coord);
        current->ud_edge_orientation = EdgeOrientationCoordinate(p);
        current->fb_edge_orientation = FBEdgeOrientationCoordinate(p);
        current->lr_edge_orientation = LREdgeOrientationCoordinate(p);
        current->ud_corner_orientation = CornerOrientationCoordinate(p);
        current->fb_corner_orientation = FBCornerOrientationCoordinate(p);
        current->lr_corner_orientation = LRCornerOrientationCoordinate(p);
        current->ud_corner_permutation = CornerPermutationCoordinate(p);
        current->ud_pruning = table->get_real_simpl(
            current->ud_coord, current->ud_edge_orientation,
            current->ud_corner_orientation);
        current->fb_pruning = table->get_real_simpl(
            current->fb_coord, current->fb_edge_orientation,
            current->fb_corner_orientation);
        current->lr_pruning = table->get_real_simpl(
            current->lr_coord, current->lr_edge_orientation,
            current->lr_corner_orientation);
    }

    // true iff the exponent was not already at maximum (3)
    inline bool increase_exponent() {
        if (current->exponent == 3) {
            return false;
        }
        current->exponent++;
        return true;
    }

    // true iff the axis was not already at maximum (B). Depends heavily on the
    // order defined by CanonicalPermutationIndex
    inline bool increase_axis() {
        if (current->axis == B) {
            return false;
        }
        current->axis++;
        current->exponent = 1;
        return true;
    }

    inline bool is_solved(CubeState* target) {
        return target->ud_corner_permutation == 0 &&
               target->ud_corner_orientation == 0 &&
               target->ud_edge_orientation == 0 &&
               /* target->ud_class == 0 */ target->ud_coord < SymmetryLength &&
               /* target->lr_class == 0 */ target->lr_coord < SymmetryLength &&
               /* target->fb_class == 0 */ target->fb_coord < SymmetryLength;
    }

    inline bool has_discardable_neighbour_axis() {
        if (boundary_depth == remaining_depth) {
            return false;
        }
        CubeState* previous = current - 1;
        // avoid UU, U2U, etc. and avoid DU, D2U, D2U2, etc.
        return previous->axis == current->axis ||
               current->axis + 3 == previous->axis;
    }

    inline void execute_move(CubeState* recipient, int move) {
        int fb_move = FullCanonicalPermutationConjugate[move][fb_transform];
        int lr_move = FullCanonicalPermutationConjugate[move][lr_transform];

        recipient->ud_coord = SymUDSliceSortedMove(current->ud_coord, move);

        recipient->fb_coord = SymUDSliceSortedMove(current->fb_coord, fb_move);

        recipient->lr_coord = SymUDSliceSortedMove(current->lr_coord, lr_move);

        recipient->ud_corner_permutation =
            CornerPermutationMove[current->ud_corner_permutation][move];

        recipient->ud_edge_orientation =
            EdgeOrientationMove[current->ud_edge_orientation][move];

        recipient->fb_edge_orientation =
            EdgeOrientationMove[current->fb_edge_orientation][fb_move];

        recipient->lr_edge_orientation =
            EdgeOrientationMove[current->lr_edge_orientation][lr_move];

        recipient->ud_corner_orientation =
            CornerOrientationMove[current->ud_corner_orientation][move];

        recipient->fb_corner_orientation =
            CornerOrientationMove[current->fb_corner_orientation][fb_move];

        recipient->lr_corner_orientation =
            CornerOrientationMove[current->lr_corner_orientation][lr_move];

        recipient->ud_pruning =
            RelativePruning[current->ud_pruning][table->get_simpl(
                recipient->ud_coord, recipient->ud_edge_orientation,
                recipient->ud_corner_orientation)];

        recipient->fb_pruning =
            RelativePruning[current->fb_pruning][table->get_simpl(
                recipient->fb_coord, recipient->fb_edge_orientation,
                recipient->fb_corner_orientation)];

        recipient->lr_pruning =
            RelativePruning[current->lr_pruning][table->get_simpl(
                recipient->lr_coord, recipient->lr_edge_orientation,
                recipient->lr_corner_orientation)];
    }

    inline void solution_innerloop(Permutation& p) {
        reinitialize(p);
        uint64_t zero = 0UL;
        int move;
        CubeState* next;
        ShouldIncrease should_increase = Exponent, temp_should_increase;
        while (true) {
            while (should_increase != ShouldIncrease::Nothing) {
                temp_should_increase = should_increase;
                should_increase = ShouldIncrease::Nothing;
                switch (temp_should_increase) {
                    case Axis:
                        if (increase_axis()) {
                            if (has_discardable_neighbour_axis()) {
                                should_increase = Axis;
                                continue;
                            } else {
                                current->exponent = 1;
                            }
                        } else if (boundary_depth == remaining_depth) {
                            boundary_depth++;
                            remaining_depth++;
                            if (boundary_depth == 20) {
                                // throw DidNotSolveWithin20Moves();
                            }
                            current->axis = U;
                            current->exponent = 1;
                        } else {
                            current--;
                            remaining_depth++;
                            should_increase = Exponent;
                            continue;
                        }
                        break;
                    case Exponent:
                        if (!increase_exponent()) {
                            should_increase = Axis;
                            continue;
                        }
                        break;
                    default:
                        // throw ExpectedToIncreaseSomething();
                        break;
                }
                move = AxisExponent2Move[current->axis][current->exponent];
                // symmetry check
                if ((current->equal_by_sequence & GreaterBy[move]) != zero) {
                    should_increase = Exponent;
                    continue;
                }
                next = current + 1;
                execute_move(next, move);
                // try to prune
                if (remaining_depth > 0 &&
                    next->ud_pruning == next->lr_pruning &&
                    next->ud_pruning == next->fb_pruning) {
                    if (next->ud_pruning > remaining_depth) {
                        should_increase = Axis;
                        continue;
                    } else if (next->ud_pruning > remaining_depth - 1) {
                        should_increase = Exponent;
                        continue;
                    }
                } else {
                    if (next->ud_pruning > remaining_depth + 1 ||
                        next->fb_pruning > remaining_depth + 1 ||
                        next->lr_pruning > remaining_depth + 1) {
                        should_increase = Axis;
                        continue;
                    } else if (next->ud_pruning > remaining_depth ||
                               next->fb_pruning > remaining_depth ||
                               next->lr_pruning > remaining_depth) {
                        should_increase = Exponent;
                        continue;
                    }
                }
            }
            if (remaining_depth == 0) {
                if (is_solved(next)) {
                    break;
                } else {
                    should_increase = Exponent;
                    continue;
                }
            } else {
                remaining_depth--;
                next->equal_by_sequence =
                    current->equal_by_sequence & EqualBy[move];
                next->axis = U;
                next->exponent = 0;
                current = next;
                should_increase = Exponent;
                if (has_discardable_neighbour_axis()) {
                    next->exponent = 1;
                    should_increase = Axis;
                }
                continue;
            }
        }
    }

    CubeSolution solve(Permutation& cube) {
        if (Permutation::equals(cube, Permutation::identity())) {
            CubeSolution solution{0};
            return solution;
        }
        solution_innerloop(cube);
        int length = 0;
        for (CubeState* i = &states[0]; i <= current; i++) {
            length++;
        }
        CubeSolution solution{length};
        int index = 0;
        for (CubeState* i = &states[0]; i <= current; i++) {
            solution.move_indexes.push_back(
                AxisExponent2Move[i->axis][i->exponent]);
            solution.moves.push_back(
                CanonicalPermutation[solution.move_indexes[index]]);
            solution.move_names =
                solution.move_names +
                CanonicalPermutationName[solution.move_indexes[index]] + " ";
            index++;
        }
        return solution;
    }
};

#endif