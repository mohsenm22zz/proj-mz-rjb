#include "Circuit.h"
#include <algorithm>
#include <vector>
#include <string>

Circuit::Circuit() : delta_t(0) {} // اگر delta_t صفر باشد، تحلیل DC و در غیر این صورت گذرا است.

void Circuit::addNode(const string &name) {
    if (!findNode(name)) {
        Node newNode;
        newNode.name = name;
        nodes.push_back(newNode);
    }
}

Node* Circuit::findNode(const string& find_from_name) {
    for (auto& node : nodes) {
        if (node.name == find_from_name) {
            return &node;
        }
    }
    return nullptr;
}

Node* Circuit::findOrCreateNode(const string& name) {
    Node* node = findNode(name);
    if (node) {
        return node;
    }
    addNode(name);
    return &nodes.back();
}

// ... سایر توابع find و delete بدون تغییر باقی می‌مانند ...
Resistor* Circuit::findResistor(const string& find_from_name) {
    for (auto& res : resistors) { if (res.name == find_from_name) return &res; }
    return nullptr;
}

Capacitor* Circuit::findCapacitor(const string& find_from_name) {
    for (auto& cap : capacitors) { if (cap.name == find_from_name) return &cap; }
    return nullptr;
}

Inductor* Circuit::findInductor(const string& find_from_name) {
    for (auto& ind : inductors) { if (ind.name == find_from_name) return &ind; }
    return nullptr;
}

Diode* Circuit::findDiode(const string& find_from_name) {
    for (auto& d : diodes) { if (d.name == find_from_name) return &d; }
    return nullptr;
}

CurrentSource* Circuit::findCurrentSource(const string& find_from_name) {
    for (auto& cs : currentSources) { if (cs.name == find_from_name) return &cs; }
    return nullptr;
}

VoltageSource* Circuit::findVoltageSource(const string& find_from_name) {
    for (auto& vs : voltageSources) { if (vs.name == find_from_name) return &vs; }
    return nullptr;
}

bool Circuit::deleteResistor(const string& name) {
    auto it = remove_if(resistors.begin(), resistors.end(), [&](const Resistor& r){ return r.name == name; });
    if (it != resistors.end()) { resistors.erase(it, resistors.end()); return true; }
    return false;
}

bool Circuit::deleteCapacitor(const string& name) {
    auto it = remove_if(capacitors.begin(), capacitors.end(), [&](const Capacitor& c){ return c.name == name; });
    if (it != capacitors.end()) { capacitors.erase(it, capacitors.end()); return true; }
    return false;
}

bool Circuit::deleteInductor(const string& name) {
    auto it = remove_if(inductors.begin(), inductors.end(), [&](const Inductor& i){ return i.name == name; });
    if (it != inductors.end()) { inductors.erase(it, inductors.end()); return true; }
    return false;
}

bool Circuit::deleteDiode(const string& name) {
    auto it = remove_if(diodes.begin(), diodes.end(), [&](const Diode& d){ return d.name == name; });
    if (it != diodes.end()) { diodes.erase(it, diodes.end()); return true; }
    return false;
}

bool Circuit::deleteVoltageSource(const string& name) {
    auto it = remove_if(voltageSources.begin(), voltageSources.end(), [&](const VoltageSource& vs){ return vs.name == name; });
    if (it != voltageSources.end()) { voltageSources.erase(it, voltageSources.end()); return true; }
    return false;
}

bool Circuit::deleteCurrentSource(const string& name) {
    auto it = remove_if(currentSources.begin(), currentSources.end(), [&](const CurrentSource& cs){ return cs.name == name; });
    if (it != currentSources.end()) { currentSources.erase(it, currentSources.end()); return true; }
    return false;
}

/**
 * @brief ماتریس G (ماتریس هدایت) را می‌سازد.
 * این ماتریس شامل اثرات مقاومتی و مدل هدایت معادل خازن‌ها است.
 */
vector<vector<double>> Circuit::G() {
    int num_non_gnd_nodes = countNonGroundNodes();
    vector<vector<double>> g_matrix(num_non_gnd_nodes, vector<double>(num_non_gnd_nodes, 0.0));

    // اثر مقاومت‌ها
    for (const auto& res : resistors) {
        if (res.resistance == 0) continue;
        double conductance = 1.0 / res.resistance;
        int idx1 = getNodeMatrixIndex(res.node1);
        int idx2 = getNodeMatrixIndex(res.node2);

        if (idx1 != -1) g_matrix[idx1][idx1] += conductance;
        if (idx2 != -1) g_matrix[idx2][idx2] += conductance;
        if (idx1 != -1 && idx2 != -1) {
            g_matrix[idx1][idx2] -= conductance;
            g_matrix[idx2][idx1] -= conductance;
        }
    }

    // اثر خازن‌ها (مدل اویلر پسرو)
    if (delta_t > 0) {
        for (const auto& cap : capacitors) {
            // خازن با یک رسانایی معادل C/h مدل می‌شود.
            double equiv_conductance = cap.capacitance / delta_t;
            int idx1 = getNodeMatrixIndex(cap.node1);
            int idx2 = getNodeMatrixIndex(cap.node2);

            if (idx1 != -1) g_matrix[idx1][idx1] += equiv_conductance;
            if (idx2 != -1) g_matrix[idx2][idx2] += equiv_conductance;
            if (idx1 != -1 && idx2 != -1) {
                g_matrix[idx1][idx2] -= equiv_conductance;
                g_matrix[idx2][idx1] -= equiv_conductance;
            }
        }
    }
    return g_matrix;
}

/**
 * @brief ماتریس B را می‌سازد.
 * این ماتریس اثر جریان‌های نامعلوم (از منابع ولتاژ و سلف‌ها) را در معادلات KCL وارد می‌کند.
 */
vector<vector<double>> Circuit::B() {
    int num_non_gnd_nodes = countNonGroundNodes();
    int m_vars = voltageSources.size() + inductors.size();
    if (m_vars == 0) return {};

    vector<vector<double>> b_matrix(num_non_gnd_nodes, vector<double>(m_vars, 0.0));

    // اثر منابع ولتاژ
    for (size_t j = 0; j < voltageSources.size(); ++j) {
        int p_node_idx = getNodeMatrixIndex(voltageSources[j].node1); // گره مثبت
        int n_node_idx = getNodeMatrixIndex(voltageSources[j].node2); // گره منفی
        if (p_node_idx != -1) b_matrix[p_node_idx][j] = 1.0;  // جریان از گره مثبت خارج می‌شود
        if (n_node_idx != -1) b_matrix[n_node_idx][j] = -1.0; // و به گره منفی وارد می‌شود
    }

    // اثر سلف‌ها
    for (size_t k = 0; k < inductors.size(); ++k) {
        int inductor_current_col = voltageSources.size() + k;
        int idx1 = getNodeMatrixIndex(inductors[k].node1);
        int idx2 = getNodeMatrixIndex(inductors[k].node2);
        if (idx1 != -1) b_matrix[idx1][inductor_current_col] = 1.0;
        if (idx2 != -1) b_matrix[idx2][inductor_current_col] = -1.0;
    }
    return b_matrix;
}

/**
 * @brief ماتریس C را می‌سازد که ترانهاده ماتریس B است.
 */
vector<vector<double>> Circuit::C() {
    vector<vector<double>> b_mat = B();
    if (b_mat.empty() || b_mat[0].empty()) return {};

    int n_rows_b = b_mat.size();
    int n_cols_b = b_mat[0].size();
    vector<vector<double>> c_matrix(n_cols_b, vector<double>(n_rows_b, 0.0));

    for (int i = 0; i < n_cols_b; i++) {
        for (int j = 0; j < n_rows_b; ++j) {
            c_matrix[i][j] = b_mat[j][i];
        }
    }
    return c_matrix;
}

/**
 * @brief ماتریس D را می‌سازد.
 * این ماتریس معادله مشخصه قطعاتی که جریانشان نامعلوم است (مانند سلف) را تکمیل می‌کند.
 */
vector<vector<double>> Circuit::D() {
    int m_vars = voltageSources.size() + inductors.size();
    if (m_vars == 0) return {};

    vector<vector<double>> d_matrix(m_vars, vector<double>(m_vars, 0.0));

    // اثر سلف‌ها (مدل اویلر پسرو)
    // معادله KVL برای سلف: Vp - Vn - (L/h) * I_L(t) = -(L/h) * I_L(t-h)
    // جمله -(L/h) در این ماتریس قرار می‌گیرد.
    if (delta_t > 0) {
        for (size_t k = 0; k < inductors.size(); ++k) {
            int inductor_var_idx = voltageSources.size() + k;
            d_matrix[inductor_var_idx][inductor_var_idx] = -(inductors[k].inductance / delta_t);
        }
    }
    // در تحلیل DC، سلف اتصال کوتاه است (Vp - Vn = 0).
    // با delta_t بسیار بزرگ، جمله -(L/h) به صفر میل می‌کند و D صفر می‌شود.
    // معادله KVL به Vp - Vn = 0 تبدیل می‌شود که صحیح است.
    return d_matrix;
}

/**
 * @brief بردار J (سمت راست معادلات KCL) را می‌سازد.
 * این بردار شامل منابع جریان مستقل و منابع جریان معادل تاریخی خازن‌ها است.
 */
vector<double> Circuit::J() {
    int num_non_gnd_nodes = countNonGroundNodes();
    vector<double> j_vector(num_non_gnd_nodes, 0.0);

    // منابع جریان مستقل
    for (const auto& cs : currentSources) {
        int p_node_idx = getNodeMatrixIndex(cs.node1); // جریان به این گره وارد می‌شود (مثبت)
        int n_node_idx = getNodeMatrixIndex(cs.node2); // جریان از این گره خارج می‌شود (منفی)
        if (p_node_idx != -1) j_vector[p_node_idx] += cs.value;
        if (n_node_idx != -1) j_vector[n_node_idx] -= cs.value;
    }

    // منبع جریان معادل خازن (مدل اویلر پسرو)
    // I_eq = C/h * V_prev
    if (delta_t > 0) {
        for (const auto& cap : capacitors) {
            double cap_rhs_term = (cap.capacitance / delta_t) * cap.prevVoltage;
            int idx1 = getNodeMatrixIndex(cap.node1);
            int idx2 = getNodeMatrixIndex(cap.node2);
            if (idx1 != -1) j_vector[idx1] += cap_rhs_term;
            if (idx2 != -1) j_vector[idx2] -= cap_rhs_term;
        }
    }
    return j_vector;
}

/**
 * @brief بردار E (سمت راست معادلات KVL) را می‌سازد.
 * شامل منابع ولتاژ مستقل و منابع ولتاژ معادل تاریخی سلف‌ها است.
 */
vector<double> Circuit::E() {
    int m_vars = voltageSources.size() + inductors.size();
    if (m_vars == 0) return {};

    vector<double> e_vector(m_vars, 0.0);

    // منابع ولتاژ مستقل
    for (size_t j = 0; j < voltageSources.size(); ++j) {
        e_vector[j] = voltageSources[j].value;
    }

    // منبع ولتاژ معادل سلف (مدل اویلر پسرو)
    // V_eq = -(L/h) * I_prev
    if (delta_t > 0) {
        for (size_t k = 0; k < inductors.size(); ++k) {
            int inductor_row = voltageSources.size() + k;
            e_vector[inductor_row] = -(inductors[k].inductance / delta_t) * inductors[k].prevCurrent;
        }
    }
    return e_vector;
}

void Circuit::set_MNA_A() {
    vector<vector<double>> g_mat = G();
    vector<vector<double>> b_mat = B();
    vector<vector<double>> c_mat = C();
    vector<vector<double>> d_mat = D();

    int n = g_mat.size();
    int m = b_mat.empty() ? 0 : b_mat[0].size();
    MNA_A.assign(n + m, vector<double>(n + m, 0.0));

    // Place G
    if(n>0) for (int i = 0; i < n; i++) for (int j = 0; j < n; ++j) MNA_A[i][j] = g_mat[i][j];
    // Place B
    if (m > 0 && n > 0) for (int i = 0; i < n; i++) for (int j = 0; j < m; ++j) MNA_A[i][n + j] = b_mat[i][j];
    // Place C
    if (m > 0 && n > 0) for (int i = 0; i < m; i++) for (int j = 0; j < n; ++j) MNA_A[n + i][j] = c_mat[i][j];
    // Place D
    if (m > 0) for (int i = 0; i < m; i++) for (int j = 0; j < m; ++j) MNA_A[n + i][n + j] = d_mat[i][j];
}

void Circuit::set_MNA_RHS() {
    vector<double> j_vec = J();
    vector<double> e_vec = E();
    int n = j_vec.size();
    int m = e_vec.size();
    MNA_RHS.assign(n + m, 0.0);

    for (int i = 0; i < n; i++) MNA_RHS[i] = j_vec[i];
    for (int i = 0; i < m; i++) MNA_RHS[n + i] = e_vec[i];
}

void Circuit::setDeltaT(double dt) {
    this->delta_t = dt;
}

/**
 * @brief حالت‌های قبلی قطعات دینامیک (ولتاژ خازن و جریان سلف) را به‌روز می‌کند.
 * این تابع باید در انتهای هر گام زمانی فراخوانی شود تا مقادیر جدید برای گام بعدی آماده شوند.
 */
void Circuit::updateComponentStates() {
    for (auto& cap : capacitors) {
        cap.update(delta_t);
    }
    for (auto& ind : inductors) {
        ind.update(delta_t);
    }
}

int Circuit::getNodeMatrixIndex(const Node* target_node_ptr) const {
    if (!target_node_ptr || target_node_ptr->isGround) {
        return -1;
    }
    int matrix_idx = 0;
    for (const auto& n_in_list : nodes) {
        if (!n_in_list.isGround) {
            if (n_in_list.num == target_node_ptr->num) {
                return matrix_idx;
            }
            matrix_idx++;
        }
    }
    return -1; // اگر پیدا نشد
}

int Circuit::countNonGroundNodes() const {
    int count = 0;
    for (const auto& node : nodes) {
        if (!node.isGround) {
            count++;
        }
    }
    return count;
}
