#include <thread>
#include "assert.h"
#include "coordinate.cpp"
#include "hash.cpp"
#include "permutation.cpp"
#include "pruningtable.cpp"
#include "solve.cpp"
#include "symmetry.cpp"

void test_permutation() {
    assert(Permutation::equals(
        Permutation::mult(CanonicalPermutation[U], CanonicalPermutation[D]),
        Permutation::mult(CanonicalPermutation[D], CanonicalPermutation[U])));
    assert(Permutation::equals(
        Permutation::mult(CanonicalPermutation[R], CanonicalPermutation[L]),
        Permutation::mult(CanonicalPermutation[L], CanonicalPermutation[R])));
    assert(Permutation::equals(
        Permutation::mult(CanonicalPermutation[B], CanonicalPermutation[F]),
        Permutation::mult(CanonicalPermutation[F], CanonicalPermutation[B])));
    assert(Permutation::equals(
        Permutation::mult_vector({CanonicalPermutation[U],
                                  CanonicalPermutation[D],
                                  CanonicalPermutation[U]}),
        Permutation::mult_vector({CanonicalPermutation[U],
                                  CanonicalPermutation[U],
                                  CanonicalPermutation[D]})));
    assert(Permutation::equals(
        Permutation::mult_vector(
            {CanonicalPermutation[R], CanonicalPermutation[R],
             CanonicalPermutation[R], CanonicalPermutation[R]}),
        Permutation::identity()));
    assert(
        Permutation::equals(Permutation::mult_vector({CanonicalPermutation[R],
                                                      CanonicalPermutation[R]}),
                            CanonicalPermutation[R2]));

    Permutation p = Permutation::identity();
    assert(Permutation::edge_permutation_is_even(p));
    p = Permutation::mult(p, CanonicalPermutation[U]);
    assert(!Permutation::edge_permutation_is_even(p));
    p = Permutation::mult(p, CanonicalPermutation[D]);
    assert(Permutation::edge_permutation_is_even(p));
}

void test_coordinate() {
    Permutation perm = Permutation::identity();
    assert(CornerPermutationCoordinate(perm) == 0);
    assert(CornerOrientationCoordinate(perm) == 0);
    assert(EdgePermutationCoordinate(perm) == 0);
    assert(EdgeOrientationCoordinate(perm) == 0);
    assert(UDSliceCoordinate(perm) == 0);
    assert(UDSliceSortedCoordinate(perm) == 0);
    assert(SymUDSliceSortedCoordinate(perm) == 0);

    for (int i = 0; i < 2186; i++) {
        perm = CornerOrientationCoordinateInverse(i);
        assert(CornerOrientationCoordinate(perm) == i);
    }

    for (int i = 0; i < 2047; i++) {
        perm = EdgeOrientationCoordinateInverse(i);
        assert(EdgeOrientationCoordinate(perm) == i);
    }

    for (int i = 0; i < 11879; i++) {
        perm = UDSliceSortedCoordinateInverse(i);
        assert(UDSliceSortedCoordinate(perm) == i);
    }

    for (int i = 0; i < CornerPermutationCoordinateLength; i++) {
        perm = CornerPermutationCoordinateInverse(i);
        assert(CornerPermutationCoordinate(perm) == i);
    }

    for (int i = 0; i < FBSliceSortedCoordinateLength; i++) {
        perm = FBSliceSortedCoordinateInverse(i);
        assert(FBSliceSortedCoordinate(perm) == i);
    }

    for (int i = 0; i < LRSliceSortedCoordinateLength; i++) {
        perm = LRSliceSortedCoordinateInverse(i);
        assert(LRSliceSortedCoordinate(perm) == i);
    }

    for (int i = 0; i < SymUDSliceSortedCoordinateLength; i++) {
        int raw = UDSliceSortedSym2Raw[i];
        perm = UDSliceSortedCoordinateInverse(raw);
        int sym = SymUDSliceSortedCoordinate(perm);
        Vector syms = UDSliceSortedRaw2Sym[raw];
        bool found = false;
        for (int j = 0; j < syms.size(); j++) {
            if (syms[j] == sym) {
                found = true;
                break;
            }
        }
        assert(found == true);
    }

    // Already tested. Working.
    /* for (int i = 0; i < 11879; i++) {
        for (int j = 0; j < 2047; j++) {
            perm = UDSliceSortedEdgeOrientationInverse(i, j);
            assert(UDSliceSortedCoordinate(perm) == i);
            assert(EdgeOrientationCoordinate(perm) == j);
        }
    } */
}

void test_symmetry() {
    // S_URF3 is not available under "Symmetry" constant
    assert(Permutation::equals(
        Permutation::mult_vector({FundamentalSymmetry[S_URF3],
                                  FundamentalSymmetry[S_URF3],
                                  FundamentalSymmetry[S_URF3]}),
        Permutation::identity()));
    assert(
        Permutation::equals(Permutation::mult(Symmetry[S_F2], Symmetry[S_F2]),
                            Permutation::identity()));
    assert(Permutation::equals(
        Permutation::mult_vector(
            {Symmetry[S_U4], Symmetry[S_U4], Symmetry[S_U4], Symmetry[S_U4]}),
        Permutation::identity()));
    assert(
        Permutation::equals(Permutation::mult(Symmetry[S_LR2], Symmetry[S_LR2]),
                            Permutation::identity()));

    assert(Permutation::equals(Permutation::identity(),
                               Permutation::mult(SymmetryS_R4, SymmetryS_R4i)));

    for (int i = 0; i < SymmetryLength; i++) {
        assert(SymmetryIndex(Symmetry[i]) == i);
        assert(Permutation::equals(
            Permutation::identity(),
            Permutation::mult(Symmetry[i], InverseSymmetry[i])));
        assert(Permutation::equals(
            Permutation::identity(),
            Permutation::mult(Symmetry[i], Symmetry[InverseSymmetryIndex[i]])));
        for (int j = i + 1; j < SymmetryLength; j++) {
            assert(!Permutation::equals(Symmetry[i], Symmetry[j]));
        }
        for (int j = 0; j < SymmetryLength; j++) {
            assert(SymmetryMult[i][j] ==
                   SymmetryIndex(Permutation::mult(Symmetry[i], Symmetry[j])));
        }

        for (int m = 0; m < CanonicalPermutationLength; m++) {
            assert(Permutation::equals(
                CanonicalPermutation[CanonicalPermutationConjugate[m][i]],
                SymmetryConjugate(CanonicalPermutation[m], i)));
        }
    }

    for (int i = 0; i < FullSymmetryLength; i++) {
        assert(FullSymmetryIndex(FullSymmetry[i]) == i);
        assert(Permutation::equals(
            Permutation::identity(),
            Permutation::mult(FullSymmetry[i], FullInverseSymmetry[i])));
        assert(Permutation::equals(
            Permutation::identity(),
            Permutation::mult(FullSymmetry[i],
                              FullSymmetry[FullInverseSymmetryIndex[i]])));
        for (int j = i + 1; j < FullSymmetryLength; j++) {
            assert(!Permutation::equals(FullSymmetry[i], FullSymmetry[j]));
        }
        for (int m = 0; m < CanonicalPermutationLength; m++) {
            assert(Permutation::equals(
                CanonicalPermutation[FullCanonicalPermutationConjugate[m][i]],
                FullSymmetryConjugate(CanonicalPermutation[m], i)));
        }
    }
}

void test_move_table() {
    Permutation p, res, q, r;
    for (int i = 0; i < SymUDSliceSortedCoordinateLength; i++) {
        p = UDSliceSortedCoordinateInverse(UDSliceSortedSym2Raw[i]);
        for (int m = 0; m < CanonicalPermutationLength; m++) {
            res = Permutation::mult(p, CanonicalPermutation[m]);
            assert(UDSliceSortedCoordinate(res) ==
                   UDSliceSortedSym2Raw[SymUDSliceSortedMove(i, m)]);
            if ((i & 15) == IdentitySymmetryIndex) {
                assert(UDSliceSortedCoordinate(res) ==
                       UDSliceSortedSym2Raw
                           [SymUDSliceSortedRepresentantMove[i >> 4][m]]);
            }
        }
    }

    for (int i = 0; i < CornerOrientationCoordinateLength; i++) {
        p = CornerOrientationCoordinateInverse(i);
        for (int m = 0; m < CanonicalPermutationLength; m++) {
            res = Permutation::mult(p, CanonicalPermutation[m]);
            assert(CornerOrientationCoordinate(res) ==
                   CornerOrientationMove[i][m]);
        }
    }

    for (int i = 0; i < EdgeOrientationCoordinateLength; i++) {
        p = EdgeOrientationCoordinateInverse(i);
        for (int m = 0; m < CanonicalPermutationLength; m++) {
            res = Permutation::mult(p, CanonicalPermutation[m]);
            assert(EdgeOrientationCoordinate(res) == EdgeOrientationMove[i][m]);
        }
    }

    for (int i = 0; i < CornerPermutationCoordinateLength; i++) {
        p = CornerPermutationCoordinateInverse(i);
        for (int m = 0; m < CanonicalPermutationLength; m++) {
            res = Permutation::mult(p, CanonicalPermutation[m]);
            assert(CornerPermutationCoordinate(res) ==
                   CornerPermutationMove[i][m]);
        }
    }

    for (int i = 0; i < FBSliceSortedCoordinateLength; i++) {
        p = FBSliceSortedCoordinateInverse(i);
        for (int m = 0; m < CanonicalPermutationLength; m++) {
            res = Permutation::mult(p, CanonicalPermutation[m]);
            assert(FBSliceSortedCoordinate(res) == FBSliceSortedMove[i][m]);
        }
    }

    for (int i = 0; i < LRSliceSortedCoordinateLength; i++) {
        p = LRSliceSortedCoordinateInverse(i);
        for (int m = 0; m < CanonicalPermutationLength; m++) {
            res = Permutation::mult(p, CanonicalPermutation[m]);
            assert(LRSliceSortedCoordinate(res) == LRSliceSortedMove[i][m]);
        }
    }

    // Exercise understanding of symmetry conjugation
    Permutation move;
    for (int i = 0; i < FBSliceSortedCoordinateLength; i++) {
        p = FBSliceSortedCoordinateInverse(i);
        q = Permutation::mult_vector({SymmetryS_R4i, p, SymmetryS_R4});
        for (int m = 0; m < CanonicalPermutationLength; m++) {
            move = Permutation::mult_vector(
                {SymmetryS_R4i, CanonicalPermutation[m], SymmetryS_R4});
            r = Permutation::mult(q, move);
            res = Permutation::mult_vector({SymmetryS_R4, r, SymmetryS_R4i});
            assert(FBSliceSortedMove[i][m] == FBSliceSortedCoordinate(res));
        }
    }
}

void test_pruning_table() {
    PruningTable table;
    table.allocate();
    // 16 is arbitrary
    for (int i = 0; i < 16; i++) {
        assert(table.get(0, 0, i) == Empty);
        table.set(0, 0, i, i % 3 == 0 ? 3 : i % 3);
    }
    for (int i = 0; i < 16; i++) {
        assert(table.get(0, 0, i) == (i % 3 == 0 ? 3 : i % 3));
    }
    for (int i = 0; i < 16; i++) {
        assert(table.get(1, 0, i) == Empty);
        table.set(1, 0, i, i % 3 == 0 ? 3 : i % 3);
    }
    for (int i = 0; i < 16; i++) {
        assert(table.get(1, 0, i) == (i % 3 == 0 ? 3 : i % 3));
    }
    table.deallocate();
    table.allocate();
    table.load_from_file("pruning_table.bin");
    assert(table.get_real(0, 0, 0) == 0);
    int corner = CornerOrientationMove[0][R];
    int edge = EdgeOrientationMove[0][R];
    int ud = SymUDSliceSortedMove(0, R);
    table.reduce_to_representant(&ud, &edge, &corner);
    assert(table.get_real(ud, edge, corner) == 1);
}

void test_hash() {
    for (int m = 0; m < CanonicalPermutationLength; m++) {
        assert(Permutation::equals(
            CanonicalPermutation[m],
            Hash2Permutation(Hash(CanonicalPermutation[m]))));
        for (int m2 = 0; m2 < CanonicalPermutationLength; m2++) {
            Permutation p = Permutation::mult(CanonicalPermutation[m],
                                              CanonicalPermutation[m2]);
            assert(Permutation::equals(p, Hash2Permutation(Hash(p))));
        }
    }
}

void test_all() {
    test_permutation();
    test_coordinate();
    test_symmetry();
    test_pruning_table();
    test_move_table();
    test_hash();
}

bool stripequals(string text, int start, string target) {
    for (int i = start; i < start + target.length(); i++) {
        if (text[i] != target[i - start]) {
            return false;
        }
    }
    return true;
}

// start_index is updated to allow continuation of parsing in other functions
vector<int> parse_moves(string requested_moves, int* start_index) {
    vector<int> moves;
    int i;
    for (i = *start_index; i < requested_moves.length(); i++) {
        int index = 0;
        if (requested_moves[i] == ' ') {
            continue;
        }
        if (requested_moves[i] == ';') {
            i++;
            break;
        }
        for (int m = 0; m < 6; m++) {
            if (CanonicalPermutationName[m][0] == requested_moves[i]) {
                index = m;
                break;
            }
        }
        if (i + 1 < requested_moves.length()) {
            switch (requested_moves[i + 1]) {
                case 'i':
                    i++;
                    index += 12;
                    break;
                case '2':
                    i++;
                    index += 6;
                    break;
                default:
                    break;
            }
        }
        moves.push_back(index);
    }
    *start_index = i;
    return moves;
}

bool is_lowercase(char a) {
    return a < 'a' >= 0 && a - 'z' <= 0;
}

void skip_space(string text, int* start_index) {
    while (text[*start_index] == ' ') {
        (*start_index)++;
    }
}

string parse_hash(string text, int* start_index) {
    string res;
    int i;
    for (i = *start_index; text[i] != ';' && i < text.length(); i++) {
        skip_space(text, &i);
        if (text[i] != ';') {
            res.push_back(text[i]);
        }
    }
    *start_index = i == text.length() ? i : i + 1;
    return res;
}

void solve_loop() {
    PruningTable table;
    table.allocate();
    table.load_from_file("pruning_table.bin");

    CubeSolver solver{&table};

    cout << "Started solving" << endl;

    while (true) {
        Permutation p;
        string requested_moves;
        vector<int> moves{};
        CubeSolution solution{0};
        getline(cin, requested_moves);
        int start = 0;
        if (stripequals(requested_moves, 0, "hash_solve")) {
            start = ((string) "hash_solve").length();
            string hash = parse_hash(requested_moves, &start);
            cout << "recognized permutation: " << hash << endl;
            p = Hash2Permutation(hash);
            solution = solver.solve(p);
            for (int i = 0; i < solution.length; i++) {
                p = Permutation::mult(p, solution.moves[i]);
            }
            cout << "solution: " << solution.move_names << endl << endl;

        }
        // not lowercase means move input
        else if (stripequals(requested_moves, 0, "move_solve")) {
            start = ((string) "move_solve").length();
            moves = parse_moves(requested_moves, &start);
            cout << "recognized permutation: ";
            for (int i = 0; i < moves.size(); i++) {
                cout << CanonicalPermutationName[moves[i]] << " ";
            }
            cout << endl;

            p = Permutation::identity();
            for (int i = 0; i < moves.size(); i++) {
                p = Permutation::mult(p, CanonicalPermutation[moves[i]]);
            }
            solution = solver.solve(p);
            for (int i = 0; i < solution.length; i++) {
                p = Permutation::mult(p, solution.moves[i]);
            }
            cout << solution.move_names << endl << endl;
            moves.clear();
        } else if (stripequals(requested_moves, 0, "hash")) {
            start = ((string) "hash").length();
            moves = parse_moves(requested_moves, &start);
            cout << "recognized permutation: ";
            for (int i = 0; i < moves.size(); i++) {
                cout << CanonicalPermutationName[moves[i]] << " ";
            }
            cout << endl;

            p = Permutation::identity();
            for (int i = 0; i < moves.size(); i++) {
                p = Permutation::mult(p, CanonicalPermutation[moves[i]]);
            }
            cout << "hash is: " << Hash(p) << endl << endl;
        }
    }
}

int main() {
    // test_hash();
    // test_symmetry();
    solve_loop();
    return 0;
}
