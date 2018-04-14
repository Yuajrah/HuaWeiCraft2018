//
// Created by ruiy on 18-4-13.
//

#include "SVR.h"

SVR::SVR(std::vector<std::vector<double>> X, std::vector<double> Y, svm_parameter param): X(X), Y(Y), param(param){}

void SVR::train() {

    model.param = param;

    // 计算alpha和ro
    std::pair<std::vector<double>, double> alpha_rho = train_one();

    model.rho = alpha_rho.second;

    model.l = 0;
    for(int i=0;i<Y.size();i++) {
        if (fabs(alpha_rho.first[i]) > 0) {
            model.SV.push_back(X[i]);
            model.sv_coef.push_back(alpha_rho.first[i]);
            model.sv_indices.push_back(i + 1);
            model.l++;
        }
    }
}

std::pair<std::vector<double>, double> SVR::train_one() {
    std::vector<double> res_alpha(Y.size(), 0.0);

    // nu_svr 求解
    std::vector<double> t_alpha;

    // 处理前半段的数据
    double sum = param.C * param.nu * Y.size() / 2;
    for(int i=0;i<Y.size();i++) {
        t_alpha.push_back(std::min(param.C, sum));
        this->p.push_back(-Y[i]);
        this->y.push_back(1);

        sum -= t_alpha[i];
    }

    // 处理后半段的数据
    for(int i=0;i<Y.size();i++) {
        t_alpha.push_back(t_alpha[i]);
        this->p.push_back(Y[i]);
        this->y.push_back(-1);
    }


    // 求解得到t_alpha, 进一步得到alpha

    this->Q = new SVR_Q(X, Y, param);

    this->alpha = t_alpha;
    this->QD=Q->get_QD();

    this->Cp = param.C;
    this->Cn = param.C;
    this->eps = param.eps;

    unshrink = false;

    int l =  2 * Y.size();

    alpha_status = std::vector<char>(l);
    for(int i=0;i<l;i++) update_alpha_status(i);

    for(int i=0;i<l;i++) active_set.push_back(i);

    active_size = l;

    for(double &t: p) {
        G.push_back(t);
        G_bar.push_back(0);
    }

    for(int i=0;i<l;i++)
        if (alpha[i] >= 0) {
//            const float *Q_i = Q->get_Q(i,l);
            const std::vector<float> Q_i = Q->get_Q(i, l);
            for(int j=0;j<l;j++)
                G[j] += alpha[i]*Q_i[j];

            if(alpha[i] >= get_C(i))
                for(int j=0;j<l;j++)
                    G_bar[j] += get_C(i) * Q_i[j];
        }


    // 优化

    int iter = 0;
    int max_iter = std::max(10000000, l>INT_MAX/100? INT_MAX: 100*l);
    int counter = std::min(l,1000)+1;

    while (iter < max_iter) {
        // 松弛
        if (--counter == 0) {
            counter = std::min(l,1000);
            shrink(l);
        }

        int i,j;
        if(select_workset(i,j) != 0) {

            gradient(l);
            active_size = l;

            if(select_workset(i,j)!=0)
                break;
            else
                counter = 1;
        }

        ++iter;

        // update alpha[i] and alpha[j], handle bounds carefully

        std::vector<float> Q_i = Q->get_Q(i,active_size);
        std::vector<float> Q_j = Q->get_Q(j,active_size);

        double C_i = get_C(i);
        double C_j = get_C(j);

        double old_alpha_i = alpha[i];
        double old_alpha_j = alpha[j];

        if(y[i]!=y[j])
        {
            double quad_coef = QD[i]+QD[j]+2*Q_i[j];
            if (quad_coef <= 0)
                quad_coef = TAU;
            double delta = (-G[i]-G[j])/quad_coef;
            double diff = alpha[i] - alpha[j];
            alpha[i] += delta;
            alpha[j] += delta;

            if(diff > 0)
            {
                if(alpha[j] < 0)
                {
                    alpha[j] = 0;
                    alpha[i] = diff;
                }
            }
            else
            {
                if(alpha[i] < 0)
                {
                    alpha[i] = 0;
                    alpha[j] = -diff;
                }
            }
            if(diff > C_i - C_j)
            {
                if(alpha[i] > C_i)
                {
                    alpha[i] = C_i;
                    alpha[j] = C_i - diff;
                }
            }
            else
            {
                if(alpha[j] > C_j)
                {
                    alpha[j] = C_j;
                    alpha[i] = C_j + diff;
                }
            }
        }
        else
        {
            double quad_coef = QD[i]+QD[j]-2*Q_i[j];
            if (quad_coef <= 0)
                quad_coef = TAU;
            double delta = (G[i]-G[j])/quad_coef;
            double sum = alpha[i] + alpha[j];
            alpha[i] -= delta;
            alpha[j] += delta;

            if(sum > C_i)
            {
                if(alpha[i] > C_i)
                {
                    alpha[i] = C_i;
                    alpha[j] = sum - C_i;
                }
            }
            else
            {
                if(alpha[j] < 0)
                {
                    alpha[j] = 0;
                    alpha[i] = sum;
                }
            }
            if(sum > C_j)
            {
                if(alpha[j] > C_j)
                {
                    alpha[j] = C_j;
                    alpha[i] = sum - C_j;
                }
            }
            else
            {
                if(alpha[i] < 0)
                {
                    alpha[i] = 0;
                    alpha[j] = sum;
                }
            }
        }

        // update G

        double delta_alpha_i = alpha[i] - old_alpha_i;
        double delta_alpha_j = alpha[j] - old_alpha_j;

        for(int k=0;k<active_size;k++)
        {
            G[k] += Q_i[k]*delta_alpha_i + Q_j[k]*delta_alpha_j;
        }

        // update alpha_status and G_bar

        {
            bool ui = is_upper_bound(i);
            bool uj = is_upper_bound(j);
            update_alpha_status(i);
            update_alpha_status(j);
            int k;
            if(ui != is_upper_bound(i))
            {
                Q_i = Q->get_Q(i,l);
                if(ui)
                    for(k=0;k<l;k++)
                        G_bar[k] -= C_i * Q_i[k];
                else
                    for(k=0;k<l;k++)
                        G_bar[k] += C_i * Q_i[k];
            }

            if(uj != is_upper_bound(j))
            {
                Q_j = Q->get_Q(j,l);
                if(uj)
                    for(k=0;k<l;k++)
                        G_bar[k] -= C_j * Q_j[k];
                else
                    for(k=0;k<l;k++)
                        G_bar[k] += C_j * Q_j[k];
            }
        }
    }

    if(iter >= max_iter)
    {
        if(active_size < l)
        {
            // reconstruct the whole gradient to calculate objective value
            gradient(l);
            active_size = l;
        }
    }

    // calculate rho

    si.rho = calc_rho();

    // calculate objective value
    {
        double v = 0;
        int i;
        for(i=0;i<l;i++)
            v += alpha[i] * (G[i] + p[i]);
        si.obj = v/2;
    }

    si.upper_bound_p = Cp;
    si.upper_bound_n = Cn;

    // 求解结束

    for(int i=0;i<2 * Y.size();i++)
        t_alpha[active_set[i]] = alpha[i];

    for(int i=0;i<Y.size();i++)
        res_alpha[i] = t_alpha[i] - t_alpha[i+Y.size()];

    return {res_alpha, si.rho};
}

double SVR::predict(const std::vector<double> x) {
    double pred_result = -model.rho;

    for(int i=0;i<model.l;i++)
        pred_result += model.sv_coef[i] * SVR_Q::dot(x, model.SV[i]);

    return pred_result;
}

void SVR::gradient(int l) {
    if(active_size == l) return;

    int nr_free = 0;

    for(int j=active_size;j<l;j++) G[j] = G_bar[j] + p[j];

    for(int j=0;j<active_size;j++) if(is_free(j)) nr_free++;

    if (nr_free*l > 2*active_size*(l-active_size)) {
        for(int i=active_size;i<l;i++) {
            const std::vector<float> Q_i = Q->get_Q(i,active_size);
            for(int j=0;j<active_size;j++) if(is_free(j))G[i] += alpha[j] * Q_i[j];
        }
    } else {
        for(int i=0;i<active_size;i++)
            if(is_free(i)) {
                const std::vector<float> Q_i = Q->get_Q(i,l);
                double alpha_i = alpha[i];
                for(int j=active_size;j<l;j++)
                    G[j] += alpha_i * Q_i[j];
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
int SVR::select_workset(int &res_i, int &res_j) {

    double g_map_p = -DBL_MAX;
    double g_map_p2 = -DBL_MAX;
    int g_map_p_index = -1;

    double g_max_n = -DBL_MAX;
    double g_max_n2 = -DBL_MAX;
    int g_max_n_index = -1;

    int g_min_index = -1;
    double obj_diff_min = DBL_MAX;

    for(int t=0;t<active_size;t++)
        if(y[t] == 1) {
            if(!is_upper_bound(t))
                if(-G[t] >= g_map_p) {
                    g_map_p = -G[t];
                    g_map_p_index = t;
                }

        } else {
            if(!is_lower_bound(t))
                if(G[t] >= g_max_n)
                {
                    g_max_n = G[t];
                    g_max_n_index = t;
                }
        }

    std::vector<float> Q_ip;
    std::vector<float> Q_in;

    // 空Q_ip无法被访问: 如果ip = -1, 则g_max_p = -INF;
    if(g_map_p_index != -1) Q_ip = Q->get_Q(g_map_p_index,active_size);
    if(g_max_n_index != -1) Q_in = Q->get_Q(g_max_n_index,active_size);

    for (int j=0;j<active_size;j++) {
        if(y[j]==+1) {
            if (!is_lower_bound(j)) {
                double grad_diff=g_map_p+G[j];
                if (G[j] >= g_map_p2)
                    g_map_p2 = G[j];
                if (grad_diff > 0)
                {
                    double obj_diff;
                    double quad_coef = QD[g_map_p_index]+QD[j]-2*Q_ip[j];
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
                double grad_diff=g_max_n-G[j];
                if (-G[j] >= g_max_n2) g_max_n2 = -G[j];
                if (grad_diff > 0) {
                    double obj_diff;
                    double quad_coef = QD[g_max_n_index]+QD[j]-2*Q_in[j];
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
    double ub1 = INF, ub2 = INF;
    double lb1 = -INF, lb2 = -INF;
    double sum_free1 = 0, sum_free2 = 0;

    for(int i=0;i<active_size;i++) {
        if(y[i]==+1) {
            if(is_upper_bound(i))
                lb1 = std::max(lb1,G[i]);
            else if(is_lower_bound(i))
                ub1 = std::min(ub1,G[i]);
            else
            {
                ++nr_free1;
                sum_free1 += G[i];
            }
        } else {
            if(is_upper_bound(i))
                lb2 = std::max(lb2,G[i]);
            else if(is_lower_bound(i))
                ub2 = std::min(ub2,G[i]);
            else
            {
                ++nr_free2;
                sum_free2 += G[i];
            }
        }
    }

    double r1,r2;
    if(nr_free1 > 0)
        r1 = sum_free1/nr_free1;
    else
        r1 = (ub1+lb1)/2;

    if(nr_free2 > 0)
        r2 = sum_free2/nr_free2;
    else
        r2 = (ub2+lb2)/2;

    si.r = (r1+r2)/2;
    return (r1-r2)/2;
}

bool SVR::shrunk(int i, double Gmax1, double Gmax2, double Gmax3, double Gmax4)
{
    if(is_upper_bound(i))
    {
        if(y[i]==+1)
            return(-G[i] > Gmax1);
        else
            return(-G[i] > Gmax4);
    }
    else if(is_lower_bound(i))
    {
        if(y[i]==+1)
            return(G[i] > Gmax2);
        else
            return(G[i] > Gmax3);
    }
    else
        return(false);
}

void SVR::shrink(int l)
{
    double Gmax1 = -INF;	// max { -y_i * grad(f)_i | y_i = +1, i in I_up(\alpha) }
    double Gmax2 = -INF;	// max { y_i * grad(f)_i | y_i = +1, i in I_low(\alpha) }
    double Gmax3 = -INF;	// max { -y_i * grad(f)_i | y_i = -1, i in I_up(\alpha) }
    double Gmax4 = -INF;	// max { y_i * grad(f)_i | y_i = -1, i in I_low(\alpha) }

    // find maximal violating pair first
    int i;
    for(i=0;i<active_size;i++)
    {
        if(!is_upper_bound(i))
        {
            if(y[i]==+1)
            {
                if(-G[i] > Gmax1) Gmax1 = -G[i];
            }
            else	if(-G[i] > Gmax4) Gmax4 = -G[i];
        }
        if(!is_lower_bound(i))
        {
            if(y[i]==+1)
            {
                if(G[i] > Gmax2) Gmax2 = G[i];
            }
            else	if(G[i] > Gmax3) Gmax3 = G[i];
        }
    }

    if(unshrink == false && std::max(Gmax1+Gmax2,Gmax3+Gmax4) <= eps*10) {
        unshrink = true;
        gradient(l);
        active_size = l;
    }

    for(i=0;i<active_size;i++)
        if (shrunk(i, Gmax1, Gmax2, Gmax3, Gmax4))
        {
            active_size--;
            while (active_size > i)
            {
                if (!shrunk(active_size, Gmax1, Gmax2, Gmax3, Gmax4))
                {

                    Q->swap_index(i, active_size);
                    std::swap(y[i],y[active_size]);
                    std::swap(G[i],G[active_size]);
                    std::swap(alpha_status[i],alpha_status[active_size]);
                    std::swap(alpha[i],alpha[active_size]);
                    std::swap(p[i],p[active_size]);
                    std::swap(active_set[i],active_set[active_size]);
                    std::swap(G_bar[i],G_bar[active_size]);
                    break;
                }
                active_size--;
            }
        }
}

void SVR::update_alpha_status(int i) {
    if(alpha[i] >= get_C(i))
        alpha_status[i] = UPPER_BOUND;
    else if(alpha[i] <= 0)
        alpha_status[i] = LOWER_BOUND;
    else alpha_status[i] = FREE;
}