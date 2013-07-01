#include "../base/system.hh"
#include "../base/format.hh"
#include "../base/boxconfig.hh"
#include "../base/fourier.hh"
#include "../base/fft.hh"
#include "../cgal/interpol.hh"

#include "dmt.hh"

using namespace System;

template <typename Point>
class Hessian_3
{
	System::ptr<System::BoxConfig<3>> box;

	static inline unsigned idx(unsigned i, unsigned j)
	{
		return (i >= j  ? (i * (i + 1)) / 2 + j
				: (j * (j + 1)) / 2 + i);
	}

	public:
		std::vector<Array<double>> 		    d_ij_data;
		std::vector<Misc::Interpol::Linear<Array<double>,3>> d_ij;

		Hessian_3(System::ptr<System::BoxConfig<3>> box_, 
			System::Array<double> potential);

		double eigenvalue(unsigned k, mVector<double, 3> const &x) const;
};

template <unsigned R> double symdet(std::vector<double> const &M);

template <>
inline double symdet<2>(std::vector<double> const &M)
{
	return M[0] * M[2] - M[1] * M[1];
}

template <>
inline double symdet<3>(std::vector<double> const &M)
{
	return M[0] * M[2] * M[5]
		- M[0] * M[4] * M[4]
		- M[2] * M[3] * M[3]
		- M[5] * M[1] * M[1]
		+ 2 * M[1] * M[3] * M[4];
}

template <unsigned R> double symsqtr(std::vector<double> const &M);

template <>
inline double symsqtr<2>(std::vector<double> const &M)
{
	return M[0]*M[0] + 2 * M[1]*M[1] + M[2]*M[2];
}

template <>
inline double symsqtr<3>(std::vector<double> const &M)
{
	return M[0]*M[0] + 2 * M[1]*M[1] + M[2]*M[2]
		+ 2 * M[3]*M[3] + 2 * M[4]*M[4] + M[5]*M[5];
}

template <typename Point>
Hessian_3<Point>::Hessian_3(ptr<BoxConfig<3>> box_, Array<double> potential):
	box(box_)
{
	for (unsigned i = 0; i < 6; ++i)
	{
		d_ij_data.push_back(Array<double>(box->size()));
		d_ij.push_back(Misc::Interpol::Linear<Array<double>,3>(box, d_ij_data[i]));
	}

	Array<Fourier::complex64> psi_f(box->size());
	Fourier::Transform fft(std::vector<int>(3, box->N()));
	auto K = Fourier::kspace<3>(box->N(), box->N());
	copy(potential, fft.in); 
	fft.forward();
	copy(fft.out, psi_f);

	unsigned o = 0;
	double s = box->size() / box->scale2();
	for (unsigned i = 0; i < 3; ++i)
	{
		for (unsigned j = 0; j <= i; ++j, ++o)
		{
			auto F = Fourier::Fourier<3>::derivative(i) 
				* Fourier::Fourier<3>::derivative(j);
			transform(psi_f, K, fft.in, Fourier::Fourier<3>::filter(F));
			fft.backward();
			transform(fft.out, d_ij_data[o], Fourier::real_part(s));
		}
	}
}

template <typename Point>
double Hessian_3<Point>::eigenvalue(unsigned k, mVector<double, 3> const &x) const
{
	// copy matrix
	std::vector<double> f(6);
	for (unsigned i = 0; i < 6; ++i)
		f[i] = d_ij[i](x);

	// compute trace and traceless part
	double q = 0;
	for (unsigned i = 0; i < 3; ++i)
		q += f[idx(i,i)] / 3;
	for (unsigned i = 0; i < 3; ++i)
		f[idx(i,i)] -= q;

	// M = p*f + q*Id
	double p = sqrt(symsqtr<3>(f)/6);
	for (unsigned i = 0; i < 6; ++i)
		f[i] /= p;

	double F = acos(symdet<3>(f)/2)/3;
	std::vector<double> lambda(3);
	lambda[0] = q + 2 * p * cos(F);
	lambda[1] = q + 2 * p * cos(F + 2./3 * M_PI);
	lambda[2] = q + 2 * p * cos(F + 4./3 * M_PI);
	std::sort(lambda.begin(), lambda.end());
	return lambda[k];
}

/*
void Hessian_3::compute_eigenvalues()
{
	for (unsigned i = 0; i < 3; ++i)
		lambda.push_back(Array<double>(box->size()));

	std::vector<double> f(6);

	for (size_t x = 0; x < box->size(); ++x)
	{
		// copy matrix
		for (unsigned i = 0; i < 6; ++i)
			f[i] = d_ij[i][x];

		// compute trace and traceless part
		double q = 0;
		for (unsigned i = 0; i < 3; ++i)
			q += f[idx(i,i)] / 3;
		for (unsigned i = 0; i < 3; ++i)
			f[idx(i,i)] -= q;

		// M = p*f + q*Id
		double p = sqrt(symsqtr<3>(f)/6);
		for (unsigned i = 0; i < 6; ++i)
			f[i] /= p;

		double F = acos(symdet<3>(f)/2)/3;
		lambda[0][x] = q + 2 * p * cos(F);
		lambda[1][x] = q + 2 * p * cos(F + 2./3 * M_PI);
		lambda[2][x] = q + 2 * p * cos(F + 4./3 * M_PI);
	}
}

void Hessian_3::sort_eigenvalues()
{
	std::vector<double> l(3);
	for (size_t x = 0; x < box->size(); ++x)
	{
		for (unsigned i = 0; i < 3; ++i)
			l[i] = lambda[i][x];
		std::sort(l.begin(), l.end());
		for (unsigned i = 0; i < 3; ++i)
			lambda[i][x] = l[i];	
	}
}
*/

void command_folds(int argc, char **argv)
{
	Argv args = read_arguments(argc, argv,
		Option(0, "h", "help", "false", "shows this help."),

		Option(Option::VALUED | Option::CHECK, "i", "id", date_string(), 
			"Identifier of the run. This tag is prefixed to all input "
			"and output filenames. By default the current date is used. "),

		Option(Option::VALUED | Option::CHECK, "r", "res", "2",
			"resolution at which to mesh."),
		
		Option(Option::VALUED | Option::CHECK, "b", "time", "1",
			"Comma separated values of eigenvalues. Don't use spaces "
			"between them, or put parentheses around the list."));
		
	if (args.get<bool>("help"))
	{
		std::cerr << "DMT3D - catastrophes at your doorstep.\n"
			"Copyright Johan Hidding, June 2013 - licence: GPL3.\n\n"
			"This command computes the isocontours of all three\n "
			"eigen-values of the potential computed from the initial\n"
			"density, to a set of given values. ";
		args.print(std::cerr);
		exit(0);
	}

	std::ifstream fi(Misc::format(args["id"], ".init.conan"));
	Header H(fi); History I(fi);
	Array<double> potential = load_from_file<double>(fi, "potential");

	auto box = make_ptr<BoxConfig<3>>(
		H.get<unsigned>("mbits"), H.get<double>("size"));
	double b = args.get<double>("time");

	Hessian_3<DMT::Folds::Point> hessian(box, potential);
	//hessian.compute_eigenvalues();
	//hessian.sort_eigenvalues();

	unsigned bits = H.get<unsigned>("mbits");
	typedef Map<Array<mVector<double, 3>>, double> AI;
	auto za_data = load_from_file<mVector<double, 3>>(fi, "displacement");
	Misc::Interpol::Spline<Array<mVector<double,3>>,3> za(box->bits(), za_data);

	DMT::Folds::run(box, [&hessian, b] (DMT::Folds::Point const &q)
	{
		mVector<double, 3> Q; Q[0] = q[0]; Q[1] = q[1]; Q[2] = q[2];
		return hessian.eigenvalue(0, Q) + 1./b;
	}, args.get<double>("res"), [&za, b] (DMT::Folds::Point const &q)
	{
		mVector<double, 3> Q; Q[0] = q[0]; Q[1] = q[1]; Q[2] = q[2];
		mVector<double, 3> X = Q + za(Q) * b;
		return DMT::Folds::Point(X[0], X[1], X[2]);
	}, args["id"]+"-folds.off");

}

Global<Command> _COMMAND_FOLDS("folds", command_folds);


