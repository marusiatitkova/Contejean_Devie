#include <iostream>
#include <vector>
#include <set>

using namespace std;

enum State {
    ZERO,
    LESS
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

int evaluate(const vector<int>& equation, const vector<int>& x) {
    int sum = 0;
    for(unsigned int i = 0; i < equation.size(); i++) {
        sum += equation[i] * x[i];
    }
    return sum;
}

bool check_evaluate (const vector<vector<int>>& input, const vector<int>& ch, State state) {
    for (const auto& in : input) {
        if ((state == ZERO && evaluate(in, ch)) || (state == LESS && evaluate(in, ch) >= 0))
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

vector<vector<int>> contejean_devie(const vector<vector<int>>& input, int var_size, int num_ineq) {
    vector<vector<int>> basis;
    vector<vector<int>> basis_less;

    vector<vector<int>> q;

    vector<vector<int>> index_matrix(var_size, vector<int>(var_size, 0));
    vector<vector<int>> index_ineq(var_size);
    for (int i = 0; i < var_size; i++) {
        index_matrix[i][i] = 1;
        for (int e = 0; e < num_ineq; e++) {
            index_ineq[i].push_back(evaluate(input[e], index_matrix[i]));
        }
    }

    set<vector<int>> p(index_matrix.begin(), index_matrix.end());
    while (!p.empty()) {
        // adding to basis
        for (auto it = p.begin(); it != p.end();) {
            if (check_evaluate(input, *it, LESS) && check_basis(basis, *it))
                basis_less.push_back(*it);

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

    basis.insert( basis.end(), basis_less.begin(), basis_less.end() );
    return basis;
}

int main() {
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


    vector<vector<int>> res = contejean_devie(input_matrix, num_variables, num_inequalities);
    cout << "Result vectors: " << endl;
    for (const auto& r : res) {
        for (int v : r) {
            cout << v << " ";
        }
        cout << endl;
    }
    return 0;
}


/*
 * Test 1
-1 1 2 -3
-1 3 -2 -1
*/

//0 1 1 1
//0 0 0 1
//1 0 0 0
//0 0 1 1

/*
 * Test 2
3 2 -1 -2
*/

//0 1 0 1
//1 0 1 1
//0 0 0 1
//0 0 1 0

/*
 * Test 3
-1 0 0 0 0 1 0
0 1 0 0 1 0 -1
0 0 1 1 -1 -2 1
 */

//0 0 0 0 1 0 1
//1 0 1 1 0 1 0
//1 1 0 1 0 1 1
//1 1 1 0 0 1 1