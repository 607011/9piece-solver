const SIZE = 3 * 3;
const NUM_ORIENTATIONS = 4;
const NUM_EDGES = 4;
const TOP = 0;
const RIGHT = 1;
const BOTTOM = 2;
const LEFT = 3;
const DIRECTIONS = [ RIGHT, LEFT, TOP, RIGHT, RIGHT, BOTTOM, BOTTOM, LEFT, LEFT ];

function clamped(i, size) {
    return (size + i) % size;
}

class Puzzle {
    constructor(o) {
        this.pieces = o.pieces ? [...o.pieces] : [];
        this.solution = o.solution ? [...o.solution] : [];
    }

    will_edge_fit(a, b) {
        const av = a.piece[clamped(a.edge - a.rot, NUM_EDGES)];
        const bv = b.piece[clamped(b.edge - b.rot, NUM_EDGES)];
        return (av + bv) == 0;
    }

    place_piece_at(k, piece) {
        this.solution[k] = piece;
    }

    idx(i) {
        return this.solution[clamped(i, SIZE)].idx;
    }

    rot(i) {
        return this.solution[clamped(i, SIZE)].rot;
    }

    will_fit(k, current_piece_idx, rot0) {
        if (k == 0) {
            return true;
        }
        const piece0 = this.pieces[current_piece_idx];
        const piece1 = this.pieces[this.idx(k - 1)];
        const rot1 = this.rot(k - 1);
        const edge0 = DIRECTIONS[k];
        const edge1 = (NUM_EDGES + edge0 - 2) % NUM_EDGES;
        const fits = this.will_edge_fit(
            { piece: piece0, rot: rot0, edge: edge0 },
            { piece: piece1, rot: rot1, edge: edge1 });
        switch (k) {
            case 3:
                return fits && this.will_edge_fit(
                    { piece: this.pieces[current_piece_idx], rot: rot0, edge: TOP },
                    { piece: this.pieces[this.idx(0)], rot: this.rot(0), edge: BOTTOM });
            case 5:
                return fits && this.will_edge_fit(
                    { piece: this.pieces[current_piece_idx], rot: rot0, edge: RIGHT },
                    { piece: this.pieces[this.idx(0)], rot: this.rot(0), edge: LEFT });
            case 7:
                return fits && this.will_edge_fit(
                    { piece: this.pieces[current_piece_idx], rot: rot0, edge: BOTTOM },
                    { piece: this.pieces[this.idx(0)], rot: this.rot(0), edge: TOP });
            case 8:
                return fits && this.will_edge_fit(
                    { piece: this.pieces[current_piece_idx], rot: rot0, edge: BOTTOM },
                    { piece: this.pieces[this.idx(1)], rot: this.rot(1), edge: TOP });
            default:
                break;
        }
        return fits;
    }
}


class Solver {
    constructor(pieces) {
        this.puzzle = new Puzzle({pieces});
        this.solutions = [];
        this.num_calls_at_level = [];
    }

    _solve(k, current_puzzle, available_pieces) {
        ++this.num_calls_at_level[k];
        if (k == SIZE) {
            this.solutions.push(current_puzzle.solution);
            return;
        }
        for (let idx = 0; idx < available_pieces.length; ++idx) {
            const next_piece_idx = available_pieces[idx];
            for (let rot = 0; rot < NUM_ORIENTATIONS; ++rot) {
                if (current_puzzle.will_fit(k, next_piece_idx, rot)) {
                    const next_puzzle = new Puzzle(current_puzzle);
                    next_puzzle.place_piece_at(k, { idx: next_piece_idx, rot: rot });
                    const remaining_pieces = [...available_pieces];
                    remaining_pieces.splice(idx, 1);
                    this._solve(k + 1, next_puzzle, remaining_pieces);
                }
                if (k == 0) {
                    break;
                }
            }
        }
    }

    total_tries() {
        return this.num_calls_at_level.reduce((p, x) => p + x, 0);
    }

    solve() {
        const all_pieces = Array.apply(null, Array(SIZE)).map(function (x, i) { return i; });
        this.solutions = [];
        this.num_calls_at_level = Array(SIZE + 1).fill(0);
        this._solve(0, this.puzzle, all_pieces);
    }

}
