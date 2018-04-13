//
// Created by ruiy on 18-4-13.
//

#include "Solver.h"


void Solver::swap_index(int i, int j)
{
    Q->swap_index(i, j);
    std::swap(y[i],y[j]);
    std::swap(G[i],G[j]);
    std::swap(alpha_status[i],alpha_status[j]);
    std::swap(alpha[i],alpha[j]);
    std::swap(p[i],p[j]);
    std::swap(active_set[i],active_set[j]);
    std::swap(G_bar[i],G_bar[j]);
}

void Solver::reconstruct_gradient()
{
    // reconstruct inactive elements of G from G_bar and free variables

    if(active_size == l) return;

    int i,j;
    int nr_free = 0;

    for(j=active_size;j<l;j++)
        G[j] = G_bar[j] + p[j];

    for(j=0;j<active_size;j++)
        if(is_free(j))
            nr_free++;

//    if(2*nr_free < active_size)
//        info("\nWARNING: using -h 0 may be faster\n");

    if (nr_free*l > 2*active_size*(l-active_size))
    {
        for(i=active_size;i<l;i++)
        {
            const float *Q_i = Q->get_Q(i,active_size);
            for(j=0;j<active_size;j++)
                if(is_free(j))
                    G[i] += alpha[j] * Q_i[j];
        }
    }
    else
    {
        for(i=0;i<active_size;i++)
            if(is_free(i))
            {
                const float *Q_i = Q->get_Q(i,l);
                double alpha_i = alpha[i];
                for(j=active_size;j<l;j++)
                    G[j] += alpha_i * Q_i[j];
            }
    }
}

void Solver::Solve(int l, SVR_Q& Q, const std::vector<double> &p_, const std::vector<char> &y_,
                   std::vector<double> &alpha_, double Cp, double Cn, double eps,
                   SolutionInfo &si, int shrinking)
{

    this->si = si;

    this->l = l;
    this->Q = &Q;
    QD=Q.get_QD();


//	clone(p, p_,l);
    p = p_;
    y = y_;
    alpha = alpha_;
    this->Cp = Cp;
    this->Cn = Cn;
    this->eps = eps;
    unshrink = false;

    // initialize alpha_status
    {
        alpha_status = new char[l];
        for(int i=0;i<l;i++)
            update_alpha_status(i);
    }

    // initialize active set (for shrinking)
    {
        active_set = std::vector<int>(l);
        for(int i=0;i<l;i++)
            active_set[i] = i;
        active_size = l;
    }

    // initialize gradient
    {
        G = std::vector<double>(l, 0.0);
        G_bar = std::vector<double>(l, 0.0);
        int i;
        for(i=0;i<l;i++)
        {
            G[i] = p[i];
            G_bar[i] = 0;
        }
        for(i=0;i<l;i++)
            if(!is_lower_bound(i))
            {
                const float *Q_i = Q.get_Q(i,l);
                double alpha_i = alpha[i];
                int j;
                for(j=0;j<l;j++)
                    G[j] += alpha_i*Q_i[j];
                if(is_upper_bound(i))
                    for(j=0;j<l;j++)
                        G_bar[j] += get_C(i) * Q_i[j];
            }
    }

    // optimization step

    int iter = 0;
    int max_iter = std::max(10000000, l>INT_MAX/100 ? INT_MAX : 100*l);
    int counter = std::min(l,1000)+1;

    while(iter < max_iter)
    {
        // show progress and do shrinking

        if(--counter == 0)
        {
            counter = std::min(l,1000);
            if(shrinking) do_shrinking();
//            info(".");
        }

        int i,j;
        if(select_working_set(i,j)!=0)
        {
            // reconstruct the whole gradient
            reconstruct_gradient();
            // reset active set size and check
            active_size = l;
//            info("*");
            if(select_working_set(i,j)!=0)
                break;
            else
                counter = 1;	// do shrinking next iteration
        }

        ++iter;

        // update alpha[i] and alpha[j], handle bounds carefully

        const float *Q_i = Q.get_Q(i,active_size);
        const float *Q_j = Q.get_Q(j,active_size);

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
                Q_i = Q.get_Q(i,l);
                if(ui)
                    for(k=0;k<l;k++)
                        G_bar[k] -= C_i * Q_i[k];
                else
                    for(k=0;k<l;k++)
                        G_bar[k] += C_i * Q_i[k];
            }

            if(uj != is_upper_bound(j))
            {
                Q_j = Q.get_Q(j,l);
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
            reconstruct_gradient();
            active_size = l;
//            info("*");
        }
        fprintf(stderr,"\nWARNING: reaching max number of iterations\n");
    }

    // calculate rho

    si.rho = calculate_rho();

    // calculate objective value
    {
        double v = 0;
        int i;
        for(i=0;i<l;i++)
            v += alpha[i] * (G[i] + p[i]);

        si.obj = v/2;
    }

    // put back the solution
    {
        for(int i=0;i<l;i++)
            alpha_[active_set[i]] = alpha[i];
    }

    // juggle everything back
    /*{
        for(int i=0;i<l;i++)
            while(active_set[i] != i)
                swap_index(i,active_set[i]);
                // or Q.swap_index(i,active_set[i]);
    }*/

    si.upper_bound_p = Cp;
    si.upper_bound_n = Cn;

//    info("\noptimization finished, #iter = %d\n",iter);

//	delete[] p;
//	delete[] y;
//	delete[] alpha;
    delete[] alpha_status;
//	delete[] active_set;
//	delete[] G;
//	delete[] G_bar;
}

// return 1 if already optimal, return 0 otherwise
int Solver::select_working_set(int &out_i, int &out_j)
{
    // return i,j such that y_i = y_j and
    // i: maximizes -y_i * grad(f)_i, i in I_up(\alpha)
    // j: minimizes the decrease of obj value
    //    (if quadratic coefficeint <= 0, replace it with tau)
    //    -y_j*grad(f)_j < -y_i*grad(f)_i, j in I_low(\alpha)

    double Gmaxp = -INF;
    double Gmaxp2 = -INF;
    int Gmaxp_idx = -1;

    double Gmaxn = -INF;
    double Gmaxn2 = -INF;
    int Gmaxn_idx = -1;

    int Gmin_idx = -1;
    double obj_diff_min = INF;

    for(int t=0;t<active_size;t++)
        if(y[t]==+1)
        {
            if(!is_upper_bound(t))
                if(-G[t] >= Gmaxp)
                {
                    Gmaxp = -G[t];
                    Gmaxp_idx = t;
                }
        }
        else
        {
            if(!is_lower_bound(t))
                if(G[t] >= Gmaxn)
                {
                    Gmaxn = G[t];
                    Gmaxn_idx = t;
                }
        }

    int ip = Gmaxp_idx;
    int in = Gmaxn_idx;
    const float *Q_ip = NULL;
    const float *Q_in = NULL;
    if(ip != -1) // NULL Q_ip not accessed: Gmaxp=-INF if ip=-1
        Q_ip = Q->get_Q(ip,active_size);
    if(in != -1)
        Q_in = Q->get_Q(in,active_size);

    for(int j=0;j<active_size;j++)
    {
        if(y[j]==+1)
        {
            if (!is_lower_bound(j))
            {
                double grad_diff=Gmaxp+G[j];
                if (G[j] >= Gmaxp2)
                    Gmaxp2 = G[j];
                if (grad_diff > 0)
                {
                    double obj_diff;
                    double quad_coef = QD[ip]+QD[j]-2*Q_ip[j];
                    if (quad_coef > 0)
                        obj_diff = -(grad_diff*grad_diff)/quad_coef;
                    else
                        obj_diff = -(grad_diff*grad_diff)/TAU;

                    if (obj_diff <= obj_diff_min)
                    {
                        Gmin_idx=j;
                        obj_diff_min = obj_diff;
                    }
                }
            }
        }
        else
        {
            if (!is_upper_bound(j))
            {
                double grad_diff=Gmaxn-G[j];
                if (-G[j] >= Gmaxn2)
                    Gmaxn2 = -G[j];
                if (grad_diff > 0)
                {
                    double obj_diff;
                    double quad_coef = QD[in]+QD[j]-2*Q_in[j];
                    if (quad_coef > 0)
                        obj_diff = -(grad_diff*grad_diff)/quad_coef;
                    else
                        obj_diff = -(grad_diff*grad_diff)/TAU;

                    if (obj_diff <= obj_diff_min)
                    {
                        Gmin_idx=j;
                        obj_diff_min = obj_diff;
                    }
                }
            }
        }
    }

    if(std::max(Gmaxp+Gmaxp2,Gmaxn+Gmaxn2) < eps || Gmin_idx == -1)
        return 1;

    if (y[Gmin_idx] == +1)
        out_i = Gmaxp_idx;
    else
        out_i = Gmaxn_idx;
    out_j = Gmin_idx;

    return 0;
}

bool Solver::be_shrunk(int i, double Gmax1, double Gmax2)
{
    if(is_upper_bound(i))
    {
        if(y[i]==+1)
            return(-G[i] > Gmax1);
        else
            return(-G[i] > Gmax2);
    }
    else if(is_lower_bound(i))
    {
        if(y[i]==+1)
            return(G[i] > Gmax2);
        else
            return(G[i] > Gmax1);
    }
    else
        return(false);
}

double Solver::calculate_rho()
{
    int nr_free1 = 0,nr_free2 = 0;
    double ub1 = INF, ub2 = INF;
    double lb1 = -INF, lb2 = -INF;
    double sum_free1 = 0, sum_free2 = 0;

    for(int i=0;i<active_size;i++)
    {
        if(y[i]==+1)
        {
            if(is_upper_bound(i))
                lb1 = std::max(lb1,G[i]);
            else if(is_lower_bound(i))
                ub1 = std::min(ub1,G[i]);
            else
            {
                ++nr_free1;
                sum_free1 += G[i];
            }
        }
        else
        {
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



bool Solver::be_shrunk(int i, double Gmax1, double Gmax2, double Gmax3, double Gmax4)
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

void Solver::do_shrinking()
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

    if(unshrink == false && std::max(Gmax1+Gmax2,Gmax3+Gmax4) <= eps*10)
    {
        unshrink = true;
        reconstruct_gradient();
        active_size = l;
    }

    for(i=0;i<active_size;i++)
        if (be_shrunk(i, Gmax1, Gmax2, Gmax3, Gmax4))
        {
            active_size--;
            while (active_size > i)
            {
                if (!be_shrunk(active_size, Gmax1, Gmax2, Gmax3, Gmax4))
                {
                    swap_index(i,active_size);
                    break;
                }
                active_size--;
            }
        }
}
