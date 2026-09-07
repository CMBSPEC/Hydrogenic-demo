#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "Atom.h"

double const_HI_A2s_1s = const_HI_A2s_1s_0;
double const_sigT = const_sigT_0;
double const_HeI_A2s_1s = const_HeI_A2s_1s_0;

namespace py = pybind11;

namespace
{
    class HydrogenicAtom
    {
    public:
        HydrogenicAtom(int n_shells,
                       int nuclear_charge,
                       double nucleus_mass_in_mp,
                       bool include_quadrupole_lines,
                       int recombination_mode,
                       int message_level):
            atom_(),
            n_shells_(n_shells),
            nuclear_charge_(nuclear_charge),
            nucleus_mass_in_mp_(nucleus_mass_in_mp),
            include_quadrupole_lines_(include_quadrupole_lines),
            recombination_mode_(recombination_mode),
            message_level_(message_level)
        {
            if(n_shells < 1) throw std::invalid_argument("n_shells must be at least 1");
            if(nuclear_charge < 1) throw std::invalid_argument("nuclear_charge must be at least 1");
            if(nucleus_mass_in_mp <= 0.0) throw std::invalid_argument("nucleus_mass_in_mp must be positive");
            if(recombination_mode < 0 || recombination_mode > 2)
                throw std::invalid_argument("recombination_mode must be 0, 1, or 2");

            atom_.init(n_shells,
                       nuclear_charge,
                       nucleus_mass_in_mp,
                       include_quadrupole_lines,
                       recombination_mode,
                       message_level);
        }

        int n_shells() const { return n_shells_; }
        int nuclear_charge() const { return nuclear_charge_; }
        double nucleus_mass_in_mp() const { return nucleus_mass_in_mp_; }
        bool include_quadrupole_lines() const { return include_quadrupole_lines_; }
        int recombination_mode() const { return recombination_mode_; }
        int message_level() const { return message_level_; }
        unsigned int level_count() const { return atom_.Get_total_number_of_Levels(); }

        py::dict summary() const
        {
            py::dict result;
            result["n_shells"] = n_shells_;
            result["nuclear_charge"] = nuclear_charge_;
            result["nucleus_mass_in_mp"] = nucleus_mass_in_mp_;
            result["include_quadrupole_lines"] = include_quadrupole_lines_;
            result["recombination_mode"] = recombination_mode_;
            result["level_count"] = atom_.Get_total_number_of_Levels();
            return result;
        }

        py::dict level(unsigned int n, unsigned int l) const
        {
            validate_level(n, l);
            return level_to_dict(atom_.Level(n, l));
        }

        py::list levels() const
        {
            py::list result;
            for(unsigned int n = 1; n <= static_cast<unsigned int>(n_shells_); ++n)
            {
                for(unsigned int l = 0; l < n; ++l)
                {
                    result.append(level(n, l));
                }
            }
            return result;
        }

        py::dict transition(unsigned int upper_n,
                            unsigned int upper_l,
                            unsigned int lower_n,
                            unsigned int lower_l) const
        {
            validate_level(upper_n, upper_l);
            validate_level(lower_n, lower_l);
            if(lower_n >= upper_n)
                throw std::invalid_argument("lower_n must be smaller than upper_n");

            const Electron_Level &upper = atom_.Level(upper_n, upper_l);
            const double wavelength_cm = upper.Get_lambda21(lower_n, lower_l);

            py::dict result;
            result["upper_n"] = upper_n;
            result["upper_l"] = upper_l;
            result["lower_n"] = lower_n;
            result["lower_l"] = lower_l;
            result["a21_per_s"] = upper.Get_A21(lower_n, lower_l);
            result["frequency_hz"] = upper.Get_nu21(lower_n, lower_l);
            result["wavelength_cm"] = wavelength_cm;
            result["wavelength_angstrom"] = wavelength_cm*1.0e8;
            return result;
        }

        double photoionization_cross_section(unsigned int n, unsigned int l, double frequency_hz)
        {
            Electron_Level &level_ref = mutable_level_for_photoionization(n, l, frequency_hz);
            return level_ref.sig_phot_ion(frequency_hz);
        }

        py::array_t<double> photoionization_cross_section_array(
            unsigned int n,
            unsigned int l,
            py::array_t<double, py::array::c_style | py::array::forcecast> frequencies_hz)
        {
            validate_photoionization_ready();
            validate_level(n, l);

            Electron_Level &level_ref = atom_.Level(n, l);
            const double threshold = level_ref.Get_nu_ion();
            return map_frequency_array(frequencies_hz, threshold, [&](double frequency) {
                return level_ref.sig_phot_ion(frequency);
            });
        }

        double gaunt_factor(unsigned int n, unsigned int l, double frequency_hz)
        {
            Electron_Level &level_ref = mutable_level_for_photoionization(n, l, frequency_hz);
            return level_ref.g_phot_ion(frequency_hz);
        }

        py::array_t<double> gaunt_factor_array(
            unsigned int n,
            unsigned int l,
            py::array_t<double, py::array::c_style | py::array::forcecast> frequencies_hz)
        {
            validate_photoionization_ready();
            validate_level(n, l);

            Electron_Level &level_ref = atom_.Level(n, l);
            const double threshold = level_ref.Get_nu_ion();
            return map_frequency_array(frequencies_hz, threshold, [&](double frequency) {
                return level_ref.g_phot_ion(frequency);
            });
        }

        py::dict photoionization(
            unsigned int n,
            unsigned int l,
            py::array_t<double, py::array::c_style | py::array::forcecast> frequencies_hz)
        {
            validate_photoionization_ready();
            validate_level(n, l);

            Electron_Level &level_ref = atom_.Level(n, l);
            const double threshold = level_ref.Get_nu_ion();
            py::array_t<double> sigma = map_frequency_array(frequencies_hz, threshold, [&](double frequency) {
                return level_ref.sig_phot_ion(frequency);
            });
            py::array_t<double> gaunt = map_frequency_array(frequencies_hz, threshold, [&](double frequency) {
                return level_ref.g_phot_ion(frequency);
            });

            py::dict result;
            result["frequency_hz"] = py::array(frequencies_hz);
            result["sigma_cm2"] = sigma;
            result["gaunt"] = gaunt;
            result["threshold_hz"] = threshold;
            return result;
        }

        void rescale(double alpha_scale, double electron_mass_scale)
        {
            if(alpha_scale <= 0.0) throw std::invalid_argument("alpha_scale must be positive");
            if(electron_mass_scale <= 0.0) throw std::invalid_argument("electron_mass_scale must be positive");
            atom_.rescale_atom(alpha_scale, electron_mass_scale);
        }

        void reset()
        {
            atom_.reset_atom();
        }

    private:
        Atom atom_;
        int n_shells_;
        int nuclear_charge_;
        double nucleus_mass_in_mp_;
        bool include_quadrupole_lines_;
        int recombination_mode_;
        int message_level_;

        void validate_level(unsigned int n, unsigned int l) const
        {
            if(n == 0 || n > static_cast<unsigned int>(n_shells_))
                throw std::out_of_range("n is outside the initialized shell range");
            if(l >= n)
                throw std::out_of_range("l must satisfy 0 <= l < n");
        }

        void validate_photoionization_ready() const
        {
            if(recombination_mode_ == 0)
            {
                throw std::runtime_error(
                    "photoionization data require recombination_mode 1 or 2");
            }
        }

        void validate_frequency(const Electron_Level &level_ref, double frequency_hz) const
        {
            if(!std::isfinite(frequency_hz))
                throw std::invalid_argument("frequency_hz must be finite");
            if(frequency_hz < level_ref.Get_nu_ion())
                throw std::invalid_argument("frequency_hz must be at or above the ionization threshold");
        }

        Electron_Level& mutable_level_for_photoionization(unsigned int n,
                                                          unsigned int l,
                                                          double frequency_hz)
        {
            validate_photoionization_ready();
            validate_level(n, l);
            Electron_Level &level_ref = atom_.Level(n, l);
            validate_frequency(level_ref, frequency_hz);
            return level_ref;
        }

        py::dict level_to_dict(const Electron_Level &level_ref) const
        {
            py::dict result;
            result["n"] = level_ref.Get_n();
            result["l"] = level_ref.Get_l();
            result["statistical_weight"] = level_ref.Get_gw();
            result["ionization_energy_ev"] = level_ref.Get_E_ion();
            result["ionization_energy_erg"] = level_ref.Get_E_ion_ergs();
            result["ionization_frequency_hz"] = level_ref.Get_nu_ion();
            result["energy_difference_to_1s_ev"] = level_ref.Get_DE_1s();
            result["frequency_to_1s_hz"] = level_ref.Get_Dnu_1s();
            result["a_total_per_s"] = level_ref.Get_A_tot();
            result["a_e1_per_s"] = level_ref.Get_A_E1();
            result["a_e2_per_s"] = level_ref.Get_A_E2();
            result["gamma_per_s"] = level_ref.Get_Gamma();
            result["lifetime_s"] = level_ref.Get_tau();
            return result;
        }

        template <typename Function>
        py::array_t<double> map_frequency_array(
            py::array_t<double, py::array::c_style | py::array::forcecast> frequencies_hz,
            double threshold,
            Function function) const
        {
            py::buffer_info input_info = frequencies_hz.request();
            std::vector<py::ssize_t> shape(input_info.shape.begin(), input_info.shape.end());
            py::array_t<double> output(shape);
            py::buffer_info output_info = output.request();

            const double *input = static_cast<const double *>(input_info.ptr);
            double *values = static_cast<double *>(output_info.ptr);

            for(py::ssize_t i = 0; i < input_info.size; ++i)
            {
                const double frequency = input[i];
                if(!std::isfinite(frequency))
                    throw std::invalid_argument("all frequencies must be finite");
                if(frequency < threshold)
                    throw std::invalid_argument("all frequencies must be at or above the ionization threshold");
                values[i] = function(frequency);
            }

            return output;
        }
    };

    HydrogenicAtom make_hydrogen(int shells,
                                 bool include_quadrupole_lines,
                                 int recombination_mode,
                                 int message_level)
    {
        return HydrogenicAtom(shells,
                              1,
                              1.0,
                              include_quadrupole_lines,
                              recombination_mode,
                              message_level);
    }

    HydrogenicAtom make_oxygen(int shells,
                               bool include_quadrupole_lines,
                               int recombination_mode,
                               int message_level)
    {
        return HydrogenicAtom(shells,
                              8,
                              15.879,
                              include_quadrupole_lines,
                              recombination_mode,
                              message_level);
    }
}

PYBIND11_MODULE(_core, module)
{
    module.doc() = "Python bindings for the Hydrogenic-demo C++ atom routines.";

    py::class_<HydrogenicAtom>(module, "HydrogenicAtom")
        .def(py::init<int, int, double, bool, int, int>(),
             py::arg("n_shells"),
             py::arg("nuclear_charge") = 1,
             py::arg("nucleus_mass_in_mp") = 1.0,
             py::arg("include_quadrupole_lines") = true,
             py::arg("recombination_mode") = 1,
             py::arg("message_level") = -1)
        .def_property_readonly("n_shells", &HydrogenicAtom::n_shells)
        .def_property_readonly("nuclear_charge", &HydrogenicAtom::nuclear_charge)
        .def_property_readonly("nucleus_mass_in_mp", &HydrogenicAtom::nucleus_mass_in_mp)
        .def_property_readonly("include_quadrupole_lines", &HydrogenicAtom::include_quadrupole_lines)
        .def_property_readonly("recombination_mode", &HydrogenicAtom::recombination_mode)
        .def_property_readonly("message_level", &HydrogenicAtom::message_level)
        .def_property_readonly("level_count", &HydrogenicAtom::level_count)
        .def("summary", &HydrogenicAtom::summary)
        .def("level", &HydrogenicAtom::level, py::arg("n"), py::arg("l"))
        .def("levels", &HydrogenicAtom::levels)
        .def("transition",
             &HydrogenicAtom::transition,
             py::arg("upper_n"),
             py::arg("upper_l"),
             py::arg("lower_n"),
             py::arg("lower_l"))
        .def("photoionization_cross_section",
             py::overload_cast<unsigned int, unsigned int, double>(
                 &HydrogenicAtom::photoionization_cross_section),
             py::arg("n"),
             py::arg("l"),
             py::arg("frequency_hz"))
        .def("photoionization_cross_section",
             py::overload_cast<unsigned int,
                               unsigned int,
                               py::array_t<double, py::array::c_style | py::array::forcecast>>(
                 &HydrogenicAtom::photoionization_cross_section_array),
             py::arg("n"),
             py::arg("l"),
             py::arg("frequencies_hz"))
        .def("gaunt_factor",
             py::overload_cast<unsigned int, unsigned int, double>(&HydrogenicAtom::gaunt_factor),
             py::arg("n"),
             py::arg("l"),
             py::arg("frequency_hz"))
        .def("gaunt_factor",
             py::overload_cast<unsigned int,
                               unsigned int,
                               py::array_t<double, py::array::c_style | py::array::forcecast>>(
                 &HydrogenicAtom::gaunt_factor_array),
             py::arg("n"),
             py::arg("l"),
             py::arg("frequencies_hz"))
        .def("photoionization",
             &HydrogenicAtom::photoionization,
             py::arg("n"),
             py::arg("l"),
             py::arg("frequencies_hz"))
        .def("rescale",
             &HydrogenicAtom::rescale,
             py::arg("alpha_scale"),
             py::arg("electron_mass_scale"))
        .def("reset", &HydrogenicAtom::reset);

    module.def("hydrogen",
               &make_hydrogen,
               py::arg("shells") = 5,
               py::arg("include_quadrupole_lines") = true,
               py::arg("recombination_mode") = 1,
               py::arg("message_level") = -1);
    module.def("oxygen",
               &make_oxygen,
               py::arg("shells") = 30,
               py::arg("include_quadrupole_lines") = true,
               py::arg("recombination_mode") = 1,
               py::arg("message_level") = -1);
}
