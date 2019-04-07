#ifndef __HASH__
#define __HASH__

#include <string>
#include "permutation.cpp"

using namespace std;

const string _face_color[6]{"b", "r", "w", "g", "o", "y"};

string face2color(string face) {
    int index = 0;
    for (string i : {"U", "R", "F", "D", "L", "B"}) {
        if (i == face) {
            return _face_color[index];
        }
        index++;
    }
}

struct _cubie_face {
    int cubie;
    bool is_corner;
    bool is_center;
    bool is_edge;
    int rotation;
    _cubie_face(int cubie,
                bool is_corner,
                bool is_center,
                bool is_edge,
                int rotation)
        : cubie(cubie),
          is_corner(is_corner),
          is_center(is_center),
          is_edge(is_edge),
          rotation(rotation) {}
};

struct _anchor_color {
    string corners[8][3];
    string edges[12][2];
};

_anchor_color _build_anchor_color() {
    _anchor_color res;
    res.corners[0][0] = "b";
    res.corners[0][1] = "r";
    res.corners[0][2] = "w";
    res.corners[1][0] = "b";
    res.corners[1][1] = "w";
    res.corners[1][2] = "o";
    res.corners[2][0] = "g";
    res.corners[2][1] = "w";
    res.corners[2][2] = "r";
    res.corners[3][0] = "g";
    res.corners[3][1] = "o";
    res.corners[3][2] = "w";
    res.corners[4][0] = "b";
    res.corners[4][1] = "y";
    res.corners[4][2] = "r";
    res.corners[5][0] = "b";
    res.corners[5][1] = "o";
    res.corners[5][2] = "y";
    res.corners[6][0] = "g";
    res.corners[6][1] = "r";
    res.corners[6][2] = "y";
    res.corners[7][0] = "g";
    res.corners[7][1] = "y";
    res.corners[7][2] = "o";
    res.edges[0][0] = "b";
    res.edges[0][1] = "w";
    res.edges[1][0] = "g";
    res.edges[1][1] = "w";
    res.edges[2][0] = "b";
    res.edges[2][1] = "y";
    res.edges[3][0] = "g";
    res.edges[3][1] = "y";
    res.edges[4][0] = "b";
    res.edges[4][1] = "o";
    res.edges[5][0] = "b";
    res.edges[5][1] = "r";
    res.edges[6][0] = "g";
    res.edges[6][1] = "o";
    res.edges[7][0] = "g";
    res.edges[7][1] = "r";
    res.edges[8][0] = "w";
    res.edges[8][1] = "o";
    res.edges[9][0] = "y";
    res.edges[9][1] = "r";
    res.edges[10][0] = "y";
    res.edges[10][1] = "o";
    res.edges[11][0] = "w";
    res.edges[11][1] = "r";

    return res;
}

_anchor_color _anchor_hash = _build_anchor_color();

_cubie_face hashindex2cubieface(int hashindex) {
    switch (hashindex) {
        case 0:
            return _cubie_face(5, true, false, false, 0);
        case 1:
            return _cubie_face(2, false, false, true, 0);
        case 2:
            return _cubie_face(4, true, false, false, 0);
        case 3:
            return _cubie_face(4, false, false, true, 0);
        case 4:
            return _cubie_face(0, false, true, false, 0);
        case 5:
            return _cubie_face(5, false, false, true, 0);
        case 6:
            return _cubie_face(1, true, false, false, 0);
        case 7:
            return _cubie_face(0, false, false, true, 0);
        case 8:
            return _cubie_face(0, true, false, false, 0);
        case 9:
            return _cubie_face(5, true, false, false, 1);
        case 10:
            return _cubie_face(4, false, false, true, 1);
        case 11:
            return _cubie_face(1, true, false, false, 2);
        case 12:
            return _cubie_face(10, false, false, true, 1);
        case 13:
            return _cubie_face(4, false, true, false, 0);
        case 14:
            return _cubie_face(8, false, false, true, 1);
        case 15:
            return _cubie_face(7, true, false, false, 2);
        case 16:
            return _cubie_face(6, false, false, true, 1);
        case 17:
            return _cubie_face(3, true, false, false, 1);
        case 18:
            return _cubie_face(1, true, false, false, 1);
        case 19:
            return _cubie_face(0, false, false, true, 1);
        case 20:
            return _cubie_face(0, true, false, false, 2);
        case 21:
            return _cubie_face(8, false, false, true, 0);
        case 22:
            return _cubie_face(2, false, true, false, 0);
        case 23:
            return _cubie_face(11, false, false, true, 0);
        case 24:
            return _cubie_face(3, true, false, false, 2);
        case 25:
            return _cubie_face(1, false, false, true, 1);
        case 26:
            return _cubie_face(2, true, false, false, 1);
        case 27:
            return _cubie_face(0, true, false, false, 1);
        case 28:
            return _cubie_face(5, false, false, true, 1);
        case 29:
            return _cubie_face(4, true, false, false, 2);
        case 30:
            return _cubie_face(11, false, false, true, 1);
        case 31:
            return _cubie_face(1, false, true, false, 0);
        case 32:
            return _cubie_face(9, false, false, true, 1);
        case 33:
            return _cubie_face(2, true, false, false, 2);
        case 34:
            return _cubie_face(7, false, false, true, 1);
        case 35:
            return _cubie_face(6, true, false, false, 1);
        case 36:
            return _cubie_face(4, true, false, false, 1);
        case 37:
            return _cubie_face(2, false, false, true, 1);
        case 38:
            return _cubie_face(5, true, false, false, 2);
        case 39:
            return _cubie_face(9, false, false, true, 0);
        case 40:
            return _cubie_face(5, false, true, false, 0);
        case 41:
            return _cubie_face(10, false, false, true, 0);
        case 42:
            return _cubie_face(6, true, false, false, 2);
        case 43:
            return _cubie_face(3, false, false, true, 1);
        case 44:
            return _cubie_face(7, true, false, false, 1);
        case 45:
            return _cubie_face(3, true, false, false, 0);
        case 46:
            return _cubie_face(1, false, false, true, 0);
        case 47:
            return _cubie_face(2, true, false, false, 0);
        case 48:
            return _cubie_face(6, false, false, true, 0);
        case 49:
            return _cubie_face(3, false, true, false, 0);
        case 50:
            return _cubie_face(7, false, false, true, 0);
        case 51:
            return _cubie_face(7, true, false, false, 0);
        case 52:
            return _cubie_face(3, false, false, true, 0);
        case 53:
            return _cubie_face(6, true, false, false, 0);
        default:
            break;
    }
}

string Hash(Permutation p) {
    string res = "";
    for (int i = 0; i < 54; i++) {
        _cubie_face coord = hashindex2cubieface(i);
        if (coord.is_corner) {
            auto corner = p.corners[coord.cubie];
            auto anchors = _anchor_hash.corners[corner.replaced_by];
            int map[3]{0, 2, 1};
            switch (corner.orientation) {
                case I:
                    res = res + anchors[coord.rotation];
                    break;
                case G:
                    res = res + anchors[(coord.rotation + 2) % 3];
                    break;
                case G2:
                    res = res + anchors[(coord.rotation + 1) % 3];
                    break;
                case Re:
                    res = res + anchors[map[coord.rotation]];
                    break;
                case GRe:
                    res = res + anchors[map[(coord.rotation + 2) % 3]];
                    break;
                case G2Re:
                    res = res + anchors[map[(coord.rotation + 1) % 3]];
                    break;
                default:
                    break;
            }
        } else if (coord.is_center) {
            res = res + _face_color[coord.cubie];
        } else if (coord.is_edge) {
            auto corner = p.edges[coord.cubie];
            auto anchors = _anchor_hash.edges[corner.replaced_by];
            res = res + anchors[(coord.rotation + corner.orientation) % 2];
        }
    }
    return res;
}

int _corner_cubie_index(string name) {
    for (int i = 0; i < 8; i++) {
        if (CornerCubieName[i] == name) {
            return i;
        }
    }
}

int _edge_cubie_index(string name) {
    for (int i = 0; i < 12; i++) {
        if (EdgeCubieName[i] == name) {
            return i;
        }
    }
}

int _face_index(char name) {
    int index = 0;
    for (auto i : {'U', 'R', 'F', 'D', 'L', 'B'}) {
        if (i == name) {
            return index;
        }
        index++;
    }
}

struct _facelets_cube {
    char corners[8][3];
    char edges[12][2];
    char centers[6];
    _facelets_cube(string colors)
        : corners{{colors[8], colors[3 * 9], colors[2 * 9 + 2]},
                  {colors[6], colors[2 * 9 + 0], colors[9 + 2]},
                  {colors[5 * 9 + 2], colors[2 * 9 + 8], colors[3 * 9 + 6]},
                  {colors[5 * 9], colors[9 + 8], colors[2 * 9 + 6]},
                  {colors[2], colors[4 * 9], colors[3 * 9 + 2]},
                  {colors[0], colors[9 + 0], colors[4 * 9 + 2]},
                  {colors[5 * 9 + 8], colors[3 * 9 + 8], colors[4 * 9 + 6]},
                  {colors[5 * 9 + 6], colors[4 * 9 + 8], colors[9 + 6]}},
          edges{{colors[7], colors[2 * 9 + 1]},
                {colors[5 * 9 + 1], colors[2 * 9 + 7]},
                {colors[1], colors[4 * 9 + 1]},
                {colors[5 * 9 + 7], colors[4 * 9 + 7]},
                {colors[3], colors[9 + 1]},
                {colors[5], colors[3 * 9 + 1]},
                {colors[5 * 9 + 3], colors[9 + 7]},
                {colors[5 * 9 + 5], colors[3 * 9 + 7]},
                {colors[2 * 9 + 3], colors[9 + 5]},
                {colors[4 * 9 + 3], colors[3 * 9 + 5]},
                {colors[4 * 9 + 5], colors[9 + 3]},
                {colors[2 * 9 + 5], colors[3 * 9 + 3]}},
          centers{colors[4],         colors[3 * 9 + 4], colors[2 * 9 + 4],
                  colors[5 * 9 + 4], colors[9 + 4],     colors[4 * 9 + 4]} {}
};

bool has(char array[], int length, char element) {
    for (int i = 0; i < length; i++) {
        if (array[i] == element) {
            return true;
        }
    }
    return false;
}

int indexOf(char array[], int length, char element) {
    for (int i = 0; i < length; i++) {
        if (array[i] == element) {
            return i;
        }
    }
    return -1;
}

Permutation Hash2Permutation(string hash) {
    _facelets_cube cube{hash};
    Permutation res;
    for (int i = 0; i < 8; i++) {
        res.corners[i].replaced_by = _corner_cubie_index(
            (string)(has(cube.corners[i], 3, cube.centers[_face_index('U')])
                         ? "U"
                         : "D") +
            (has(cube.corners[i], 3, cube.centers[_face_index('R')]) ? "R"
                                                                     : "L") +
            (has(cube.corners[i], 3, cube.centers[_face_index('F')]) ? "F"
                                                                     : "B"));
        res.corners[i].orientation =
            indexOf(cube.corners[i], 3,
                    cube.centers[_face_index(
                        CornerCubieName[res.corners[i].replaced_by][0])]);
    }
    for (int i = 0; i < 12; i++) {
        string replaced_by = "";
        for (auto face : {'U', 'D', 'L', 'R', 'F', 'B'}) {
            if (has(cube.edges[i], 2, cube.centers[_face_index(face)])) {
                replaced_by = replaced_by + face;
            }
        }
        res.edges[i].replaced_by = _edge_cubie_index(replaced_by);
        char anchor;
        if (replaced_by[0] == 'L' || replaced_by[0] == 'R') {
            anchor = cube.centers[_face_index(replaced_by[1])];
        } else {
            anchor = cube.centers[_face_index(replaced_by[0])];
        }
        res.edges[i].orientation = cube.edges[i][0] == anchor ? 0 : 1;
    }
    return res;
}

#endif