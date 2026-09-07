#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Atom.h"

double const_HI_A2s_1s = const_HI_A2s_1s_0;
double const_sigT = const_sigT_0;
double const_HeI_A2s_1s = const_HeI_A2s_1s_0;

using namespace std;

namespace
{
    void print_level_summary(const Atom &atom, unsigned int n, unsigned int l)
    {
        const Electron_Level &level = atom.Level(n, l);

        cout << "  (" << n << ", " << l << ")"
             << "  gw=" << level.Get_gw()
             << "  Eion=" << level.Get_E_ion() << " eV"
             << "  nu_ion=" << level.Get_nu_ion() << " Hz"
             << "  A_tot=" << level.Get_A_tot() << " 1/s"
             << "  tau=" << level.Get_tau() << " s"
             << '\n';
    }

    void print_transition(const Atom &atom,
                          unsigned int upper_n,
                          unsigned int upper_l,
                          unsigned int lower_n,
                          unsigned int lower_l,
                          const string &label)
    {
        const Electron_Level &upper = atom.Level(upper_n, upper_l);
        const double A21 = upper.Get_A21(lower_n, lower_l);
        const double nu21 = upper.Get_nu21(lower_n, lower_l);
        const double lambda21 = upper.Get_lambda21(lower_n, lower_l);

        cout << "  " << label
             << ": A21=" << A21 << " 1/s"
             << "  nu21=" << nu21 << " Hz"
             << "  lambda21=" << lambda21 << " cm"
             << '\n';
    }

    void print_photoionization_table(Atom &atom,
                                     unsigned int n,
                                     unsigned int l,
                                     const vector<double> &threshold_factors)
    {
        Electron_Level &level = atom.Level(n, l);
        const double nu_ion = level.Get_nu_ion();

        cout << "  Level (" << n << ", " << l << "), threshold nu=" << nu_ion << " Hz" << '\n';
        for(const double factor : threshold_factors)
        {
            const double nu = factor*nu_ion;
            cout << "    nu/nu_ion=" << factor
                 << "  sigma=" << level.sig_phot_ion(nu) << " cm^2"
                 << "  gaunt=" << level.g_phot_ion(nu)
                 << '\n';
        }
    }
}

int main()
{
    constexpr int n_shells = 5;
    constexpr int nuclear_charge = 1;
    constexpr double nucleus_mass_in_mp = 1.0;
    constexpr bool include_quadrupole_lines = true;
    constexpr int recombination_mode = 1;
    constexpr int message_level = -1;

    Atom hydrogen(n_shells,
                  nuclear_charge,
                  nucleus_mass_in_mp,
                  include_quadrupole_lines,
                  recombination_mode,
                  message_level);

    cout << scientific << setprecision(6);

    cout << "Hydrogenic Atom demo using Hydrogenic.vX" << '\n';
    cout << "  Z=" << hydrogen.Get_Z()
         << "  Np=" << hydrogen.Get_Np()
         << "  shells=" << hydrogen.Get_nShells()
         << "  levels=" << hydrogen.Get_total_number_of_Levels()
         << '\n';

    cout << "\nSelected levels" << '\n';
    print_level_summary(hydrogen, 1, 0);
    print_level_summary(hydrogen, 2, 0);
    print_level_summary(hydrogen, 2, 1);
    print_level_summary(hydrogen, 3, 2);

    cout << "\nSelected bound-bound transitions" << '\n';
    print_transition(hydrogen, 2, 1, 1, 0, "Ly-alpha 2p -> 1s E1");
    print_transition(hydrogen, 3, 0, 2, 1, "Balmer-alpha 3s -> 2p E1");
    print_transition(hydrogen, 3, 2, 2, 1, "Balmer-alpha 3d -> 2p E1");
    print_transition(hydrogen, 3, 2, 1, 0, "3d -> 1s E2");

    cout << "\nPhotoionization cross sections" << '\n';
    print_photoionization_table(hydrogen, 1, 0, {1.001, 1.1, 2.0, 5.0});
    print_photoionization_table(hydrogen, 2, 1, {1.001, 1.5, 3.0});

    const double ly_alpha_nu = hydrogen.Level(2, 1).Get_nu21(1, 0);
    hydrogen.rescale_atom(1.01, 1.0);
    const double scaled_ly_alpha_nu = hydrogen.Level(2, 1).Get_nu21(1, 0);

    cout << "\nRescaling example" << '\n';
    cout << "  alpha_scale=1.01, me_scale=1.0" << '\n';
    cout << "  Ly-alpha frequency ratio=" << scaled_ly_alpha_nu/ly_alpha_nu
         << "  original=" << ly_alpha_nu << " Hz"
         << "  scaled=" << scaled_ly_alpha_nu << " Hz"
         << '\n';

    hydrogen.reset_atom();

    return 0;
}
