//
// Created by ruiy on 18-4-13.
//

#include "SVR.h"

SVR::SVR(std::vector<std::vector<double>> X, std::vector<double> Y, SvmParam param): X(X), Y(Y), param(param) {


    // 生成前半段
    for (int i = 0;i < Y.size(); i++) {
        sign.push_back(1);
        index.push_back(i);
        qd.push_back(kernel_linear(i, i));
    }

    // 生成后半段
    for (int i = 0;i < Y.size(); i++) {
        sign.push_back(-1);
        index.push_back(i);
        qd.push_back(qd[i]);
    }

    buffer.push_back(std::vector<float>(2*Y.size()));
    buffer.push_back(std::vector<float>(2*Y.size()));

    next_buffer = 0;
}

void SVR::train() {

    // 计算a和ro
    std::vector<double> a = calc_a();

    model_l = 0;
    for(int i=0;i<Y.size();i++) {
        if (fabs(a[i]) > 0) {
            sv.push_back(X[i]);
            sv_coef.push_back(a[i]);
            sv_indices.push_back(i + 1);
            model_l++;
        }
    }
}

std::vector<double> SVR::calc_a() {
    std::vector<double> res_a(Y.size(), 0.0);

    // nu_svr 求解
    std::vector<double> t_a;

    // 处理前半段的数据
    double sum = param.C * param.nu * Y.size() / 2;
    for(int i=0;i<Y.size();i++) {
        t_a.push_back(std::min(param.C, sum));
        this->p.push_back(-Y[i]);
        this->y.push_back(1);

        sum -= t_a[i];
    }

    // 处理后半段的数据
    for(int i=0;i<Y.size();i++) {
        t_a.push_back(t_a[i]);
        this->p.push_back(Y[i]);
        this->y.push_back(-1);
    }


    // 求解得到t_a, 进一步得到a

    this->a = t_a;
//    this->Cp = param.C;
//    this->Cn = param.C;
    this->eps = param.eps;

    not_constricted = false;

    int tmp_l =  2 * Y.size();

    status = std::vector<char>(tmp_l);
    for(int i=0;i<tmp_l;i++) update_a_status(i);

    for(int i=0;i<tmp_l;i++) lives.push_back(i);

    lives_size = tmp_l;

    for(double &t: p) {
        g.push_back(t);
        g_bar.push_back(0);
    }

    for(int i=0;i<tmp_l;i++)
        if (a[i] >= 0) {
            const std::vector<float> Q_i = calc_q(i, tmp_l);
            for(int j=0;j<tmp_l;j++)
                g[j] += a[i]*Q_i[j];

            if(a[i] >= param.C)
                for(int j=0;j<tmp_l;j++)
                    g_bar[j] += param.C * Q_i[j];
        }


    // 优化

    int iter = 0;
    int max_iter = std::max(10000000, tmp_l>INT_MAX/100? INT_MAX: 100*tmp_l);
    int counter = std::min(tmp_l,1000)+1;

    while (iter < max_iter) {
        // 松弛
        if (--counter == 0) {
            counter = std::min(tmp_l,1000);
            constriction(tmp_l);
        }

        int i,j;
        if(calc_ws(i,j) != 0) {

            calc_gradient(tmp_l);
            lives_size = tmp_l;

            if(calc_ws(i,j)!=0) break;
            else counter = 1;
        }

        iter++;

        // update a[i] and a[j], handle bounds carefully

        // 更新a[i] 和 a[j]
        std::vector<float> Q_i = calc_q(i,lives_size);
        std::vector<float> Q_j = calc_q(j,lives_size);

        double C_i = param.C;
        double C_j = param.C;

        double old_a_i = a[i];
        double old_a_j = a[j];

        if(y[i]!=y[j]) {
            double quad_coef = calc_qd()[i]+calc_qd()[j]+2*Q_i[j];
            if (quad_coef <= 0) quad_coef = TAU;
            double delta = (-g[i]-g[j]) / quad_coef;
            double diff = a[i] - a[j];
            a[i] += delta;
            a[j] += delta;

            if(diff > 0) {
                if(a[j] < 0) {
                    a[j] = 0;
                    a[i] = diff;
                }
            } else {
                if(a[i] < 0) {
                    a[i] = 0;
                    a[j] = -diff;
                }
            }

            if(diff > C_i - C_j) {
                if(a[i] > C_i) {
                    a[i] = C_i;
                    a[j] = C_i - diff;
                }
            } else {
                if(a[j] > C_j) {
                    a[j] = C_j;
                    a[i] = C_j + diff;
                }
            }
        }
        else {
            double quad_coef = calc_qd()[i]+calc_qd()[j]-2*Q_i[j];
            if (quad_coef <= 0)
                quad_coef = TAU;
            double delta = (g[i]-g[j])/quad_coef;
            double sum = a[i] + a[j];
            a[i] -= delta;
            a[j] += delta;

            if(sum > C_i) {
                if(a[i] > C_i) {
                    a[i] = C_i;
                    a[j] = sum - C_i;
                }
            } else {
                if(a[j] < 0){
                    a[j] = 0;
                    a[i] = sum;
                }
            }

            if(sum > C_j) {
                if(a[j] > C_j) {
                    a[j] = C_j;
                    a[i] = sum - C_j;
                }
            } else {
                if(a[i] < 0) {
                    a[i] = 0;
                    a[j] = sum;
                }
            }
        }

        // 更新 g

        double delta_a_i = a[i] - old_a_i;
        double delta_a_j = a[j] - old_a_j;

        for(int k=0;k<lives_size;k++) {
            g[k] += Q_i[k]*delta_a_i + Q_j[k]*delta_a_j;
        }

        // 更新a的状态和G_bar


        bool ui = is_upper_bound(i);
        bool uj = is_upper_bound(j);
        update_a_status(i);
        update_a_status(j);
        int k;
        if(ui != is_upper_bound(i))
        {
            Q_i = calc_q(i,tmp_l);
            if(ui)
                for(k=0;k<tmp_l;k++)
                    g_bar[k] -= C_i * Q_i[k];
            else
                for(k=0;k<tmp_l;k++)
                    g_bar[k] += C_i * Q_i[k];
        }

        if(uj != is_upper_bound(j))
        {
            Q_j = calc_q(j,tmp_l);
            if(uj)
                for(k=0;k<tmp_l;k++)
                    g_bar[k] -= C_j * Q_j[k];
            else
                for(k=0;k<tmp_l;k++)
                    g_bar[k] += C_j * Q_j[k];
        }

    }

    if(iter >= max_iter)
    {
        if(lives_size < tmp_l)
        {
            // reconstruct the whole calc_gradient to calculate objective value
            calc_gradient(tmp_l);
            lives_size = tmp_l;
        }
    }

    // 计算 rho

    rho = calc_rho();

    // 计算obj
    double v = 0;
    for(int i=0;i<tmp_l;i++) v += a[i] * (g[i] + p[i]);
    obj = v/2;

    // 求解结束

    for(int i=0;i<tmp_l;i++)
        t_a[lives[i]] = a[i];

    for(int i=0;i<Y.size();i++)
        res_a[i] = t_a[i] - t_a[i+Y.size()];

    return res_a;
}

double SVR::predict(const std::vector<double> x) {
    double pred_result = -rho;

    for(int i=0;i<model_l;i++)
        pred_result += sv_coef[i] * dot(x, sv[i]);

    return pred_result;
}

void SVR::calc_gradient(int l) {
    if(lives_size == l) return;

    int nr_free = 0;

    for(int j=lives_size;j<l;j++) g[j] = g_bar[j] + p[j];

    for(int j=0;j<lives_size;j++) if(is_free(j)) nr_free++;

    if (nr_free*l > 2*lives_size*(l-lives_size)) {
        for(int i=lives_size;i<l;i++) {
            const std::vector<float> Q_i = calc_q(i,lives_size);
            for(int j=0;j<lives_size;j++) if(is_free(j))g[i] += a[j] * Q_i[j];
        }
    } else {
        for(int i=0;i<lives_size;i++)
            if(is_free(i)) {
                const std::vector<float> Q_i = calc_q(i,l);
                double a_i = a[i];
                for(int j=lives_size;j<l;j++)
                    g[j] += a_i * Q_i[j];
            }
    }
}

// return 1 if already optimal, return 0 otherwise

/**
 *
 * @param out_i
 * @param out_j
 * @return 返回1表示已经最优, 返回0表示其他
 */
int SVR::calc_ws(int &res_i, int &res_j) {

    double g_map_p = -DBL_MAX;
    double g_map_p2 = -DBL_MAX;
    int g_map_p_index = -1;

    double g_max_n = -DBL_MAX;
    double g_max_n2 = -DBL_MAX;
    int g_max_n_index = -1;

    int g_min_index = -1;
    double obj_diff_min = DBL_MAX;

    for(int t=0;t<lives_size;t++)
        if(y[t] == 1) {
            if(!is_upper_bound(t))
                if(-g[t] >= g_map_p) {
                    g_map_p = -g[t];
                    g_map_p_index = t;
                }

        } else {
            if(!is_lower_bound(t))
                if(g[t] >= g_max_n)
                {
                    g_max_n = g[t];
                    g_max_n_index = t;
                }
        }

    std::vector<float> Q_ip;
    std::vector<float> Q_in;

    // 空Q_ip无法被访问: 如果ip = -1, 则g_max_p = -INF;
    if(g_map_p_index != -1) Q_ip = calc_q(g_map_p_index,lives_size);
    if(g_max_n_index != -1) Q_in = calc_q(g_max_n_index,lives_size);

    for (int j=0;j<lives_size;j++) {
        if(y[j]==+1) {
            if (!is_lower_bound(j)) {
                double grad_diff=g_map_p+g[j];
                if (g[j] >= g_map_p2)
                    g_map_p2 = g[j];
                if (grad_diff > 0)
                {
                    double obj_diff;
                    double quad_coef = calc_qd()[g_map_p_index]+calc_qd()[j]-2*Q_ip[j];
                    if (quad_coef > 0)
                        obj_diff = -(grad_diff*grad_diff)/quad_coef;
                    else
                        obj_diff = -(grad_diff*grad_diff)/TAU;

                    if (obj_diff <= obj_diff_min)
                    {
                        g_min_index=j;
                        obj_diff_min = obj_diff;
                    }
                }
            }
        } else {
            if (!is_upper_bound(j)) {
                double grad_diff=g_max_n-g[j];
                if (-g[j] >= g_max_n2) g_max_n2 = -g[j];
                if (grad_diff > 0) {
                    double obj_diff;
                    double quad_coef = calc_qd()[g_max_n_index]+calc_qd()[j]-2*Q_in[j];
                    if (quad_coef > 0) obj_diff = -(grad_diff*grad_diff)/quad_coef;
                    else obj_diff = -(grad_diff*grad_diff)/TAU;

                    if (obj_diff <= obj_diff_min) {
                        g_min_index=j;
                        obj_diff_min = obj_diff;
                    }
                }
            }
        }
    }

    if(std::max(g_map_p+g_map_p2,g_max_n+g_max_n2) < eps || g_min_index == -1) return 1;

    if (y[g_min_index] == +1) res_i = g_map_p_index;
    else res_i = g_max_n_index;

    res_j = g_min_index;

    return 0;
}

double SVR::calc_rho() {
    int nr_free1 = 0,nr_free2 = 0;
    double ub1 = DBL_MAX, ub2 = DBL_MAX;
    double lb1 = -DBL_MAX, lb2 = -DBL_MAX;
    double sum_free1 = 0, sum_free2 = 0;

    for(int i=0; i<lives_size; i++) {
        if(y[i] == 1) {
            if(is_upper_bound(i)) lb1 = std::max(lb1,g[i]);
            else if(is_lower_bound(i)) ub1 = std::min(ub1,g[i]);
            else {
                nr_free1++;
                sum_free1 += g[i];
            }
        } else {
            if(is_upper_bound(i)) lb2 = std::max(lb2,g[i]);
            else if(is_lower_bound(i)) ub2 = std::min(ub2,g[i]);
            else {
                nr_free2++;
                sum_free2 += g[i];
            }
        }
    }

    double r1,r2;
    if(nr_free1 > 0) r1 = sum_free1/nr_free1;
    else r1 = (ub1+lb1)/2;

    if(nr_free2 > 0) r2 = sum_free2/nr_free2;
    else r2 = (ub2+lb2)/2;

    r = (r1+r2)/2;
    return (r1-r2)/2;
}

bool SVR::constricted(int i, double g_max1, double g_max2, double g_max3, double g_max4)
{
    if (is_upper_bound(i)) {
        if(y[i]==+1) return(-g[i] > g_max1);
        else return(-g[i] > g_max4);
    } else if(is_lower_bound(i)) {
        if(y[i]==+1) return(g[i] > g_max2);
        else return(g[i] > g_max3);
    } else return(false);
}

void SVR::constriction(int l)
{
    double g_max1 = -DBL_MAX;	// max { -y_i * grad(f)_i | y_i = +1, i in I_up(\a) }
    double g_max2 = -DBL_MAX;	// max { y_i * grad(f)_i | y_i = +1, i in I_low(\a) }
    double g_max3 = -DBL_MAX;	// max { -y_i * grad(f)_i | y_i = -1, i in I_up(\a) }
    double g_max4 = -DBL_MAX;	// max { y_i * grad(f)_i | y_i = -1, i in I_low(\a) }


    // 找出最先的错误的一对
    for(int i=0;i<lives_size;i++) {

        if (!is_upper_bound(i)) {
            if(y[i] == 1) {
                if(-g[i] > g_max1) g_max2 = -g[i];
            } else if(-g[i] > g_max4) g_max4 = -g[i];
        }

        if(!is_lower_bound(i)) {
            if(y[i] == 1) {
                if(g[i] > g_max2) g_max2 = g[i];
            }
            else if(g[i] > g_max3) g_max3= g[i];
        }
    }

    if(not_constricted == false && std::max(g_max1 + g_max2, g_max3 + g_max4) <= eps*10) {
        not_constricted = true;
        calc_gradient(l);
        lives_size = l;
    }

    for(int i=0;i<lives_size;i++)
        if (constricted(i, g_max1, g_max2, g_max3, g_max4)) {
            lives_size--;
            while (lives_size > i) {
                if (!constricted(lives_size, g_max1, g_max2, g_max3, g_max4)) {

                    std::swap(sign[i],sign[lives_size]);
                    std::swap(index[i],index[lives_size]);
                    std::swap(qd[i],qd[lives_size]);
                    std::swap(y[i],y[lives_size]);
                    std::swap(g[i],g[lives_size]);
                    std::swap(status[i],status[lives_size]);
                    std::swap(a[i],a[lives_size]);
                    std::swap(p[i],p[lives_size]);
                    std::swap(lives[i],lives[lives_size]);
                    std::swap(g_bar[i],g_bar[lives_size]);
                    break;
                }
                lives_size--;
            }
        }
}

void SVR::update_a_status(int i) {
    if(a[i] >= param.C)
        status[i] = STATUS_UPPER_BOUND;
    else if(a[i] <= 0)
        status[i] = STATUS_LOWER_BOUND;
    else status[i] = STATUS_FREE;
}


std::vector<float> SVR::calc_q(int i, int len) {

    std::vector<float> data;
    int real_i = index[i];

    for(int j=0;j<Y.size();j++) data.push_back((float)kernel_linear(real_i,j));

    std::vector<float> buf = buffer[next_buffer];

    next_buffer = 1 - next_buffer;
    char si = sign[i];
    for(int j=0;j<len;j++) buf[j] = (float) si * (float) sign[j] * data[index[j]];

    return buf;
}


double SVR::dot(const std::vector<double> px, const std::vector<double> py)
{
    double sum = 0;
    int i = 0;
    while(i < px.size()) {
        sum += px[i] * py[i];
        i++;
    }
    return sum;
}
