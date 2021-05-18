#include <iostream>
#include <vector>
#include <set>
#include <tuple>
#include <unordered_map>

using namespace std;

struct  hash_pair {
    template<class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

// Petri net conditions from arcs {place, transition} -> {places...}
typedef  unordered_map<pair<int, int>, vector<int>, hash_pair> Condition;

enum State {
    ZERO,
    LESS,
    GREATER
};

bool operator< (const vector<int>& lhs, const vector<int>& rhs) {
    for (unsigned int i = 0; i < lhs.size(); i++) {
        if (lhs[i] > rhs[i])
            return false;
    }
    return true;
}

vector<int> operator+ (const vector<int>& lhs, const vector<int>& rhs) {
    vector<int> res(lhs.size());
    for (unsigned int i = 0; i < lhs.size(); i++) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

//
int evaluate(const vector<int>& equation, const vector<int>& x) {
    int sum = 0;
    for(unsigned int i = 0; i < equation.size(); i++) {
        sum += equation[i] * x[i];
    }
    return sum;
}

bool check_evaluate (const vector<vector<int>>& input, const vector<int>& ch, State state) {
    for (const auto& in : input) {
        // if ZERO and !=0 --> true
        if ((state == ZERO && evaluate(in, ch)) || (state == LESS && evaluate(in, ch) >= 0) || (state == GREATER && evaluate(in, ch) < 0))
            return false;
    }
    return true;
}

bool check_basis (const vector<vector<int>>& basis, const vector<int>& check) {
    for (const auto& b : basis) {
        if (b < check) {
            return false;
        }
    }
    return true;
}

// for set {0, 1}
bool check_range (const vector<int>& check) {
    for (int c : check)
        if (c > 1)
            return false;
    return true;
}

vector<vector<int>> contejean_devie(const vector<vector<int>>& input, int var_size, int num_ineq, State state) {
    vector<vector<int>> basis;
    vector<vector<int>> basis_state;

    vector<vector<int>> q;

    vector<vector<int>> index_matrix(var_size, vector<int>(var_size, 0));
    vector<vector<int>> index_ineq(var_size);
    for (int i = 0; i < var_size; i++) {
        index_matrix[i][i] = 1;
        //evaluate a(x) where x is index vector
        for (int e = 0; e < num_ineq; e++) {
            index_ineq[i].push_back(evaluate(input[e], index_matrix[i]));
        }
    }

    set<vector<int>> p(index_matrix.begin(), index_matrix.end());
    while (!p.empty()) {
        // adding to basis
        for (auto it = p.begin(); it != p.end();) {
            //doesn't affect algorithm, just add to answer
            if (check_evaluate(input, *it, state) && check_basis(basis, *it))
                basis_state.push_back(*it);

            if (check_evaluate(input, *it, ZERO)) {
                basis.push_back(*it);
                it = p.erase(it);
            } else
                it++;
        }

        //constructing q
        q.clear();
        for (const auto& eq : p) {
            if (check_basis(basis, eq))
                q.push_back(eq);
        }

        //creating new p
        p.clear();
        for (const auto& eq : q) {
            vector<int> q_ineq(num_ineq);
            for (int i = 0; i < num_ineq; i++) {
                q_ineq[i] = evaluate(input[i], eq);
            }
            for (int j = 0; j < var_size; j++) {
                vector<int> new_p = eq + index_matrix[j];
                if (evaluate(q_ineq, index_ineq[j]) < 0 && check_range(new_p)) {
                    p.emplace(new_p);
                }
            }
        }
    }

    basis.insert(basis.end(), basis_state.begin(), basis_state.end() );
    return basis;
}

vector<vector<int>> createConditionMatrixForPetriNet(const vector<int>& places, ,const vector<int>& transitions, const vector<tuple<int, int, int>>& arcs) {
    Condition conditions;
    for (const auto& arc : arcs) {
        conditions[{get<0>(arc), get<1>(arc)}].push_back(get<2>(arc));
    }

    vector<vector<int>> inequalities(conditions.size(), vector<int>(places.size(), 0));
    //places in key -1, places in values 1
    int i = 0;
    for(const auto& cond : conditions) {
        inequalities[i][cond.first.first - 1] = -1;
        for (const auto& v : cond.second) {
            inequalities[i][v - 1] = 1;
        }
        i++;
    }
    return inequalities;
}


int main() {
    //LABA 2
    vector<int> places = {1, 2, 3, 4};
    vector<int> transitions = {1, 2, 3, 4, 5};
    //place - transition - place
    vector<tuple<int, int, int>> arcs = {{1, 2, 3},
                                         {1, 3, 4},
                                         {2, 1, 1},
                                         {2, 1, 3},
                                         {3, 4, 2},
                                         {4, 4, 2}};

    vector<vector<int>> inequalities = createConditionMatrixForPetriNet(places, transitions, arcs);
    vector<vector<int>> res = contejean_devie(inequalities, inequalities[0].size(), inequalities.size(), GREATER);
    cout << "Result vectors: " << endl;
    for (const auto& r : res) {
        for (int v : r) {
            cout << v << " ";
        }
        cout << endl;
    }

    //LABA 1

    int num_inequalities, num_variables = 0;
    cout << "Enter number of inequalities in the system: " << endl;
    cin >> num_inequalities;
    cout << "Enter number of variables in each inequality: " << endl;
    cin >> num_variables;
    vector<vector<int>> input_matrix (num_inequalities, vector<int>(num_variables));
    cout << "Enter coefficients: " << endl;
    for (int i = 0; i < num_inequalities; i++) {
        for (int j = 0; j < num_variables; j++) {
            cin >> input_matrix[i][j];
        }
    }

    cout << "Check information " << endl << "----------------------------"<< endl
    << "Num_ineq: " << num_inequalities << endl
    << "Num_var: " << num_variables << endl << "Input matrix: " << endl;
    for (int i = 0; i < num_inequalities; i++) {
        for (int j = 0; j < num_variables; j++) {
            cout << input_matrix[i][j] << " ";
        }
        cout << endl;
    }

    vector<vector<int>> test = {{-1, 1, 2, -3},
                                 {-1, 3, -2, -1}};


    vector<vector<int>> resLab1 = contejean_devie(input_matrix, num_variables, num_inequalities, LESS);
    cout << "Result vectors: " << endl;
    for (const auto& r : resLab1) {
        for (int v : r) {
            cout << v << " ";
        }
        cout << endl;
    }
    return 0;
}


/*
 * Test 1 for LESS
-1 1 2 -3
-1 3 -2 -1
*/

//0 1 1 1
//0 0 0 1
//1 0 0 0
//0 0 1 1

/*
 * Test 2 for LESS
3 2 -1 -2
*/

//0 1 0 1
//1 0 1 1
//0 0 0 1
//0 0 1 0

/*
 * Test 3 for LESS
-1 0 0 0 0 1 0
0 1 0 0 1 0 -1
0 0 1 1 -1 -2 1
 */

//0 0 0 0 1 0 1
//1 0 1 1 0 1 0
//1 1 0 1 0 1 1
//1 1 1 0 0 1 1

/*
 * Test 4 for GREATER
-1 1 0 0 0 0 0
-1 0 1 0 0 0 0
1 -1 0 0 1 0 0
1 0 -1 0 0 0 1
0 0 0 1 -1 0 0
0 0 0 0 0 1 -1
0 1 0 -1 0 0 0
0 0 1 0 0 -1 0
 */

//0 0 1 0 0 1 1
//0 1 0 1 1 0 0
//1 1 1 0 0 0 0
//1 1 1 0 0 1 0
//1 1 1 1 0 0 0
//1 1 1 0 0 1 1
//1 1 1 1 0 1 0
//1 1 1 1 1 0 0
//1 1 1 1 0 1 1
//1 1 1 1 1 1 0
//1 1 1 1 1 1 1

/*
 * TEST 5 for GREATER
-1 0 1 0
-1 0 0 1
1 -1 1 0
0 1 -1 0
0 1 0 -1
 * */

//0 1 1 0
//0 1 1 1
//1 1 1 1