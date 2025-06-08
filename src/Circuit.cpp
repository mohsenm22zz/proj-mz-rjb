#include "Circuit.h"
#include <algorithm>
#include <vector>
#include <string>
Circuit::Circuit() : delta_t(1e-6) {
}

Node *Circuit::findNode(const string &find_from_name) {
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].name == find_from_name) {
            return &nodes[i];
        }
    }
    return nullptr;
}

Node *Circuit::findNodeByNum(int num_to_find) {
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (nodes[i].num == num_to_find) {
            return &nodes[i];
        }
    }
    return nullptr;
}

Resistor *Circuit::findResistor(const string &find_from_name) {
    for (size_t i = 0; i < resistors.size(); ++i) {
        if (resistors[i].name == find_from_name) {
            return &resistors[i];
        }
    }
    return nullptr;
}

Capacitor *Circuit::findCapacitor(const string &find_from_name) {
    for (size_t i = 0; i < capacitors.size(); ++i) {
        if (capacitors[i].name == find_from_name) {
            return &capacitors[i];
        }
    }
    return nullptr;
}

Inductor *Circuit::findInductor(const string &find_from_name) {
    for (size_t i = 0; i < inductors.size(); ++i) {
        if (inductors[i].name == find_from_name) {
            return &inductors[i];
        }
    }
    return nullptr;
}

Diode *Circuit::findDiode(const string &find_from_name) {
    for (size_t i = 0; i < diodes.size(); ++i) {
        if (diodes[i].name == find_from_name) {
            return &diodes[i];
        }
    }
    return nullptr;
}

CurrentSource *Circuit::findCurrentSource(const string &find_from_name) {
    for (size_t i = 0; i < currentSources.size(); ++i) {
        if (currentSources[i].name == find_from_name) {
            return &currentSources[i];
        }
    }
    return nullptr;
}

VoltageSource *Circuit::findVoltageSource(const string &find_from_name) {
    for (size_t i = 0; i < voltageSources.size(); ++i) {
        if (voltageSources[i].name == find_from_name) {
            return &voltageSources[i];
        }
    }
    return nullptr;
}

/// Helper to check if a node name signifies a ground node
bool Circuit::isNodeNameGround(const string &node_name) const {
    if (node_name == "0" || node_name == "GND") return true;
    for (const auto &gnd_name: groundNodeNames) {
        if (node_name == gnd_name) {
            return true;
        }
    }
    return false;
}

int Circuit::getNodeMatrixIndex(const Node *target_node_ptr) const {
    if (!target_node_ptr || target_node_ptr->isGround) {/// gnd
        return -1;
    }
    int matrix_idx = 0;
    for (const Node &n_in_list: nodes) {
        if (!n_in_list.isGround) {
            if (&n_in_list == target_node_ptr) {
                return matrix_idx;
            }
            matrix_idx++;
        }
    }
    return -2;/// error
}

/// Counts the number of non-ground nodes in the circuit.
int Circuit::countNonGroundNodes() const {
    int count = 0;
    for (const auto &node: nodes) {
        if (!node.isGround) {
            count++;
        }
    }
    return count;
}

vector<vector<double>> Circuit::G() {/// ماتریس هدایت و خازن (سلف ها متفاوت هستند)
    /// خازن ها با C/Delta_t
    int num_non_gnd_nodes = countNonGroundNodes();
    if (num_non_gnd_nodes == 0) {
        return {};
    }

    vector<vector<double>> g_matrix(num_non_gnd_nodes, vector<double>(num_non_gnd_nodes, 0.0));

    for (const auto &res: resistors) {
        if (res.resistance == 0) continue;
        double conductance = 1.0 / res.resistance;
        int idx1 = getNodeMatrixIndex(res.node1);
        int idx2 = getNodeMatrixIndex(res.node2);

        if (idx1 != -1) {
            g_matrix[idx1][idx1] += conductance;
        }
        if (idx2 != -1) {
            g_matrix[idx2][idx2] += conductance;
        }
        if (idx1 != -1 && idx2 != -1) {
            g_matrix[idx1][idx2] -= conductance;
            g_matrix[idx2][idx1] -= conductance;
        }
    }

    if (delta_t == 0) {
        /// error ya DC analysis
    } else {
        for (const auto &cap: capacitors) {
            double equiv_conductance = cap.capacitance / delta_t;
            int idx1 = getNodeMatrixIndex(cap.node1);
            int idx2 = getNodeMatrixIndex(cap.node2);

            if (idx1 != -1) {
                g_matrix[idx1][idx1] += equiv_conductance;
            }
            if (idx2 != -1) {
                g_matrix[idx2][idx2] += equiv_conductance;
            }
            if (idx1 != -1 && idx2 != -1) {
                g_matrix[idx1][idx2] -= equiv_conductance;
                g_matrix[idx2][idx1] -= equiv_conductance;
            }
        }
    }
    return g_matrix;
}

vector<vector<double>> Circuit::B() {/// برای منابع ولتاژ و سلف ها
    int num_non_gnd_nodes = countNonGroundNodes();
    int num_voltage_sources = voltageSources.size();
    int num_inductors = inductors.size();
    int m_vars = num_voltage_sources + num_inductors;

    if (num_non_gnd_nodes == 0 && m_vars == 0) return {};
    if (m_vars == 0) return vector<vector<double>>(num_non_gnd_nodes, vector<double>(0)); // B has 0 columns

    vector<vector<double>> b_matrix(num_non_gnd_nodes, vector<double>(m_vars, 0.0));

    for (int j = 0; j < num_voltage_sources; ++j) {
        const auto &vs = voltageSources[j];
        int p_node_idx = getNodeMatrixIndex(vs.node1); // vs.p_node is Node*
        int n_node_idx = getNodeMatrixIndex(vs.node2); // vs.n_node is Node*

        if (p_node_idx != -1) {
            b_matrix[p_node_idx][j] = 1.0;
        }
        if (n_node_idx != -1) {
            b_matrix[n_node_idx][j] = -1.0;
        }
    }

    for (int k = 0; k < num_inductors; ++k) {
        const auto &ind = inductors[k];
        int idx1 = getNodeMatrixIndex(ind.node1);
        int idx2 = getNodeMatrixIndex(ind.node2);
        int inductor_current_col = num_voltage_sources + k;

        if (idx1 != -1) {
            b_matrix[idx1][inductor_current_col] = 1.0;
        }
        if (idx2 != -1) {
            b_matrix[idx2][inductor_current_col] = -1.0;
        }
    }
    return b_matrix;
}

vector<vector<double>> Circuit::C() {/// ترانهاده B
    vector<vector<double>> b_mat = B();
    if (b_mat.empty()) {
        int m_vars = voltageSources.size() + inductors.size();
        if (m_vars == 0) return {};
        return vector<vector<double>>(m_vars, vector<double>(0));
    }
    if (b_mat[0].empty() && !b_mat.empty()) {
        return vector<vector<double>>(0, vector<double>(b_mat.size()));
    }

    int n_rows_b = b_mat.size();
    int n_cols_b = b_mat[0].size();

    vector<vector<double>> c_matrix(n_cols_b, vector<double>(n_rows_b, 0.0));

    for (int i = 0; i < n_cols_b; ++i) {
        for (int j = 0; j < n_rows_b; ++j) {
            c_matrix[i][j] = b_mat[j][i];
        }
    }
    return c_matrix;
}

vector<vector<double>> Circuit::D() {/// منابع وابسته و سلف ها
    int num_voltage_sources = voltageSources.size();
    int num_inductors = inductors.size();
    int m_vars = num_voltage_sources + num_inductors;

    if (m_vars == 0) return {};

    vector<vector<double>> d_matrix(m_vars, vector<double>(m_vars, 0.0));
    if (delta_t != 0) {
        for (int k = 0; k < num_inductors; ++k) {
            int inductor_var_idx = num_voltage_sources + k;
            d_matrix[inductor_var_idx][inductor_var_idx] = -(inductors[k].inductance / delta_t);
        }
    } else {
    }
    return d_matrix;
}

vector<double> Circuit::J() {/// منابع جریان و بخش خازنی وابسته به ولتاژ قبلی
    int num_non_gnd_nodes = countNonGroundNodes();
    if (num_non_gnd_nodes == 0) {
        return {};
    }

    vector<double> j_vector(num_non_gnd_nodes, 0.0);

    for (const auto &cs: currentSources) {
        int p_node_idx = getNodeMatrixIndex(cs.node1);
        int n_node_idx = getNodeMatrixIndex(cs.node2);
        if (n_node_idx != -1) {
            j_vector[n_node_idx] += cs.value;
        }
        if (p_node_idx != -1) {
            j_vector[p_node_idx] -= cs.value;
        }
    }

    if (delta_t != 0) {
        for (const auto &cap: capacitors) {
            double cap_rhs_term = (cap.capacitance / delta_t) * cap.prevVoltage;

            int idx1 = getNodeMatrixIndex(cap.node1);
            int idx2 = getNodeMatrixIndex(cap.node2);
            if (idx1 != -1) {
                j_vector[idx1] += cap_rhs_term;
            }
            if (idx2 != -1) {
                j_vector[idx2] -= cap_rhs_term;
            }
        }
    }
    return j_vector;
}

vector<double> Circuit::E() { /// مقادیر منابع ولتاژ و بخش سلفی وابسته به جریان قبلی
    int num_voltage_sources = voltageSources.size();
    int num_inductors = inductors.size();
    int m_vars = num_voltage_sources + num_inductors;

    if (m_vars == 0) return {};

    vector<double> e_vector(m_vars, 0.0);
    for (int j = 0; j < num_voltage_sources; ++j) {
        e_vector[j] = voltageSources[j].value;
    }
    if (delta_t != 0) {
        for (int k = 0; k < num_inductors; ++k) {
            int inductor_row = num_voltage_sources + k;
            e_vector[inductor_row] = -(inductors[k].inductance / delta_t) * inductors[k].prevCurrent;
        }
    } else {
    }
    return e_vector;
}

/// Av=x

void Circuit::set_MNA_A() {
    vector<vector<double>> g_mat = G();
    vector<vector<double>> b_mat = B();
    vector<vector<double>> c_mat = C();
    vector<vector<double>> d_mat = D();

    int n = g_mat.size();
    int m = 0;

    if (!b_mat.empty() && !b_mat[0].empty()) {
        m = b_mat[0].size();
    } else if (!c_mat.empty()) {
        m = c_mat.size();
    } else if (!d_mat.empty()) {
        m = d_mat.size();
    }


    if (n == 0 && m == 0) {
        MNA_A.clear();
        return;
    }

    MNA_A.assign(n + m, vector<double>(n + m, 0.0));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i < g_mat.size() && j < g_mat[i].size()) {
                MNA_A[i][j] = g_mat[i][j];
            }
        }
    }

    if (m > 0 && !b_mat.empty()) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (i < b_mat.size() && j < b_mat[i].size()) {
                    MNA_A[i][n + j] = b_mat[i][j];
                }
            }
        }
    }

    if (m > 0 && !c_mat.empty()) {
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i < c_mat.size() && j < c_mat[i].size()) {
                    MNA_A[n + i][j] = c_mat[i][j];
                }
            }
        }
    }
    if (m > 0 && !d_mat.empty()) {
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < m; ++j) {
                if (i < d_mat.size() && j < d_mat[i].size()) {
                    MNA_A[n + i][n + j] = d_mat[i][j];
                }
            }
        }
    }
}

void Circuit::set_MNA_x() {
    vector<double> j_vec = J();
    vector<double> e_vec = E();

    int n = j_vec.size();
    int m = e_vec.size();

    if (n == 0 && m == 0) {
        MNA_v.clear();
        return;
    }

    MNA_v.assign(n + m, 0.0);

    for (int i = 0; i < n; ++i) {
        MNA_v[i] = j_vec[i];
    }
    for (int i = 0; i < m; ++i) {
        MNA_v[n + i] = e_vec[i];
    }
}

void Circuit::set_MNA_v() {
    int n_vars = countNonGroundNodes();
    int m_vars = voltageSources.size() + inductors.size();

    if (n_vars == 0 && m_vars == 0) {
        MNA_x.clear();
        return;
    }
    MNA_x.assign(n_vars + m_vars, 0.0);
}

void Circuit::addNode(const string &name) {
    if (!findNode(name)) {
        Node newNode;
        newNode.name = name;
        nodes.push_back(newNode);
    }
}

void Circuit::setDeltaT() {
    delta_t = Tstep/10;
}

void Circuit::updateComponentStates() {
    for (auto &cap: capacitors) {
        double v1_new = cap.node1 ? cap.node1->voltage : 0.0;
        double v2_new = cap.node2 ? cap.node2->voltage : 0.0;
        if (cap.node1 && cap.node1->isGround) v1_new = 0.0;
        if (cap.node2 && cap.node2->isGround) v2_new = 0.0;
        cap.prevVoltage = v1_new - v2_new;
    }
    for (auto &ind: inductors) {
        ind.prevCurrent = ind.current;
    }
}
