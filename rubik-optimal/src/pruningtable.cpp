#ifndef __PRUNINGTABLE__
#define __PRUNINGTABLE__
#include <chrono>
#include <fstream>
#include <queue>
#include "coordinate.cpp"
#include "movetable.cpp"

struct Instance {
    int sym_ud_slice_sorted_coord;
    int corner_orientation_coord;
    int edge_orientation_coord;
    int depth;
    Instance(int sym_ud_slice_sorted_coord,
             int corner_orientation_coord,
             int edge_orientation_coord,
             int depth)
        : sym_ud_slice_sorted_coord(sym_ud_slice_sorted_coord),
          corner_orientation_coord(corner_orientation_coord),
          edge_orientation_coord(edge_orientation_coord),
          depth(depth) {}
};

const int MapEntry[3] = {3, 1, 2};

const int IncMod3[3] = {1, 2, 0};

const int DecMod3[3] = {2, 0, 1};

const int MapEntryDecMod3[3] = {2, 3, 1};

enum Entry { Empty, PlusOneMod3, MinusOneMod3, ZeroMod3 };

struct PruningTable {
    char*** _table;

    ~PruningTable() {
        if (_table == nullptr) {
            return;
        }
        for (int i = 0; i < UDSliceSortedClassCount; i++) {
            for (int j = 0; j < CornerOrientationCoordinateLength; j++) {
                delete[] _table[i][j];
            }
            delete[] _table[i];
        }
        delete[] _table;
    }

    // Beware: *ud_slice_sorted_coord becomes a class index, not a full coord
    inline void reduce_to_representant(int* ud_slice_sorted_coord,
                                       int* edge_orientation_coord,
                                       int* corner_orientation_coord) const {
        const int ud_class = SymUDSliceSortedClass(*ud_slice_sorted_coord);
        const int inv_symmetry = InverseSymmetryIndex[SymUDSliceSortedSymmetry(
            *ud_slice_sorted_coord)];
        *ud_slice_sorted_coord = ud_class;
        *edge_orientation_coord =
            EdgeOrientationConjugate[*edge_orientation_coord][ud_class]
                                    [inv_symmetry];
        *corner_orientation_coord =
            CornerOrientationConjugate[*corner_orientation_coord][inv_symmetry];
    }

    // result is one of {0, 1, 2, 3}. 0 means the entry is empty
    inline int get(int ud_slice_sorted_class_index,
                   int edge_orientation_coord,
                   int corner_orientation_coord) const {
        const int outer = edge_orientation_coord >> 2;
        const int inner = edge_orientation_coord & 3;
        return (_table[ud_slice_sorted_class_index][corner_orientation_coord]
                      [outer] >>
                (inner << 1)) &
               3;
    }

    // handles conjugation to UDSliceSorted representant automatically (hence
    // "simplified")
    inline int get_simpl(int ud_slice_sorted_coord,
                         int edge_orientation_coord,
                         int corner_orientation_coord) const {
        reduce_to_representant(&ud_slice_sorted_coord, &edge_orientation_coord,
                               &corner_orientation_coord);
        return get(ud_slice_sorted_coord, edge_orientation_coord,
                   corner_orientation_coord);
    }

    // result is the real lower-bound-depth (not mod 3)
    inline int get_real(int ud_slice_sorted_class_index,
                        int edge_orientation_coord,
                        int corner_orientation_coord) const {
        int depth = 0, depthMod3, temp_edge, temp_corner, temp_sym_coord;
        depthMod3 = get(ud_slice_sorted_class_index, edge_orientation_coord,
                        corner_orientation_coord);
        depthMod3 = depthMod3 == 3 ? 0 : depthMod3;
        while (ud_slice_sorted_class_index != 0 ||
               edge_orientation_coord != 0 || corner_orientation_coord != 0) {
            for (int m = 0; m < CanonicalPermutationLength; m++) {
                temp_sym_coord = SymUDSliceSortedRepresentantMove
                    [ud_slice_sorted_class_index][m];
                temp_edge = EdgeOrientationMove[edge_orientation_coord][m];
                temp_corner =
                    CornerOrientationMove[corner_orientation_coord][m];
                reduce_to_representant(&temp_sym_coord, &temp_edge,
                                       &temp_corner);
                if (get(temp_sym_coord, temp_edge, temp_corner) ==
                    MapEntryDecMod3[depthMod3]) {
                    ud_slice_sorted_class_index = temp_sym_coord;
                    edge_orientation_coord = temp_edge;
                    corner_orientation_coord = temp_corner;
                    depth++;
                    depthMod3 = DecMod3[depthMod3];
                    break;
                }
            }
        }
        return depth;
    }

    inline int get_real_simpl(int ud_slice_sorted_coord,
                              int edge_orientation_coord,
                              int corner_orientation_coord) const {
        reduce_to_representant(&ud_slice_sorted_coord, &edge_orientation_coord,
                               &corner_orientation_coord);
        return get_real(ud_slice_sorted_coord, edge_orientation_coord,
                        corner_orientation_coord);
    }

    // only works once. value must be one of {0, 1, 2, 3}
    inline void set(int ud_slice_sorted_class_index,
                    int edge_orientation_coord,
                    int corner_orientation_coord,
                    int value) {
        const int outer = edge_orientation_coord >> 2;
        const int inner = edge_orientation_coord & 3;
        _table[ud_slice_sorted_class_index][corner_orientation_coord][outer] |=
            (value << (inner << 1));
    }

    void allocate() {
        cout << "started allocating" << endl;
        // 2 bits (1/4 of an int) per effective entry
        _table = new char**[UDSliceSortedClassCount];
        for (int i = 0; i < UDSliceSortedClassCount; i++) {
            _table[i] = new char*[CornerOrientationCoordinateLength];
            for (int j = 0; j < CornerOrientationCoordinateLength; j++) {
                _table[i][j] = new char[EdgeOrientationCoordinateLength >> 2]{
                    0};  // divide by four (four entries per byte)
            }
        }
        cout << "finished allocating" << endl;
    }

    void deallocate() {
        for (int i = 0; i < UDSliceSortedClassCount; i++) {
            for (int j = 0; j < CornerOrientationCoordinateLength; j++) {
                delete[] _table[i][j];
            }
            delete[] _table[i];
        }
        delete[] _table;
    }

    void build() {
        const auto start_time = chrono::system_clock().now();
        const unsigned int all = UDSliceSortedClassCount *
                                 CornerOrientationCoordinateLength *
                                 EdgeOrientationCoordinateLength;
        unsigned int visited_count = 0;
        Instance current(0, 0, 0, 0);
        int next_sym_ud, next_corner, next_edge, next_depth = 0;
        int equiv_sym_ud, equiv_corner, equiv_edge, equiv_ud_class,
            equiv_symmetry;
        Vector* equivalent_list;
        int count_per_depth[21]{0};

        // Process first entry (and its equivalents)
        equivalent_list =
            &UDSliceSortedRaw2Sym
                [UDSliceSortedSym2Raw[current.sym_ud_slice_sorted_coord]];
        for (int i = 0; i < equivalent_list->size(); i++) {
            equiv_sym_ud = (*equivalent_list)[i];
            equiv_symmetry = SymUDSliceSortedSymmetry(equiv_sym_ud);
            equiv_ud_class = SymUDSliceSortedClass(equiv_sym_ud);
            equiv_corner = CornerOrientationConjugate
                [current.corner_orientation_coord]
                [InverseSymmetryIndex[equiv_symmetry]];
            equiv_edge =
                EdgeOrientationConjugate[current.edge_orientation_coord]
                                        [equiv_ud_class]
                                        [InverseSymmetryIndex[equiv_symmetry]];
            if (get(equiv_ud_class, equiv_edge, equiv_corner) == Empty) {
                set(equiv_ud_class, equiv_edge, equiv_corner,
                    MapEntry[next_depth]);
                visited_count++;
                count_per_depth[0]++;
            }
        }

        int real_current_depth = 0, real_next_depth, current_depth = 0,
            map_entry_current_depth, map_entry_next_depth, equiv_size;
        while (visited_count < all) {
            map_entry_current_depth = MapEntry[current_depth];
            next_depth = IncMod3[current_depth];
            map_entry_next_depth = MapEntry[next_depth];
            real_next_depth = real_current_depth + 1;
            cout << "count_per_depth[" << real_current_depth
             << "] == " << count_per_depth[real_current_depth] << endl;
            cout << "real_current_depth == " << real_current_depth << endl;
            for (int i = 0; i < UDSliceSortedClassCount; i++) {
                for (int j = 0; j < EdgeOrientationCoordinateLength; j++) {
                    for (int k = 0; k < CornerOrientationCoordinateLength;
                         k++) {
                        if (get(i, j, k) != map_entry_current_depth) {
                            continue;
                        }
                        // Assuming the "front" has already been horizontally
                        // searched
                        // (symmetries), just generate each move and process it
                        // along with its symmetries
                        for (int m = 0; m < CanonicalPermutationLength; m++) {
                            next_sym_ud =
                                SymUDSliceSortedRepresentantMove[i][m];
                            next_corner = CornerOrientationMove[k][m];
                            next_edge = EdgeOrientationMove[j][m];

                            // class is the same for all equivalents
                            equiv_ud_class = SymUDSliceSortedClass(next_sym_ud);

                            equivalent_list =
                                &(UDSliceSortedRaw2Sym
                                      [UDSliceSortedSym2Raw[next_sym_ud]]);
                            equiv_size = equivalent_list->size();
                            for (int eq = 0; eq < equiv_size; eq++) {
                                equiv_sym_ud = (*equivalent_list)[eq];
                                equiv_symmetry =
                                    SymUDSliceSortedSymmetry(equiv_sym_ud);
                                equiv_corner = CornerOrientationConjugate
                                    [next_corner]
                                    [InverseSymmetryIndex[equiv_symmetry]];
                                equiv_edge = EdgeOrientationConjugate
                                    [next_edge][equiv_ud_class]
                                    [InverseSymmetryIndex[equiv_symmetry]];
                                if (get(equiv_ud_class, equiv_edge,
                                        equiv_corner) == Empty) {
                                    set(equiv_ud_class, equiv_edge,
                                        equiv_corner, map_entry_next_depth);
                                    visited_count++;
                                    count_per_depth[real_next_depth]++;
                                }
                            }
                        }
                    }
                }
            }
            current_depth = next_depth;
            real_current_depth = real_next_depth;
        }
        cout << (chrono::system_clock().now().time_since_epoch() -
                 start_time.time_since_epoch())
                    .count()
             << endl;
    }

    void save_to_file(string filename) {
        ofstream output(filename);
        for (int i = 0; i < UDSliceSortedClassCount; i++) {
            for (int j = 0; j < CornerOrientationCoordinateLength; j++) {
                output.write(_table[i][j],
                             EdgeOrientationCoordinateLength >> 2);
            }
        }
        output.close();
    }

    void load_from_file(string filename) {
        ifstream input(filename);
        for (int i = 0; i < UDSliceSortedClassCount; i++) {
            for (int j = 0; j < CornerOrientationCoordinateLength; j++) {
                input.read(_table[i][j], EdgeOrientationCoordinateLength >> 2);
            }
        }
        input.close();
    }
};

int** _build_relative_pruning() {
    int signal_mod[3]{0, 1, -1};
    int** res = new int*[20];
    for (int i = 0; i < 20; i++) {
        res[i] = new int[4];
    }
    for (int i = 0; i < 20; i++) {
        for (int j = 1; j < 4; j++) {
            int difference = (j - i) % 3;
            difference += (difference < 0 ? 3 : 0);
            res[i][j] = i + signal_mod[difference];
        }
    }

    return res;
}

// int[pruning value = 0,1,...,19][new pruning = 1,2,3]. Returns the real
// pruning value
int** RelativePruning = _build_relative_pruning();

#endif
