#include "hessian.hh"
#include "../base/fourier.hh"

using namespace System;
using namespace DMT;

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

Hessian_3::Hessian_3(ptr<BoxConfig<3>> box_, Array<double> potential):
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

Vector_3


void Hessian_3::compute_eigenvalues()
{
	for (unsigned k = 0; k < 3; ++k)
	{
		d_lambda_data.push_back(Array<double>(box->size()));
		d_ev1.push_back(Array<Vector_3>(box->size()));
		d_ev2.push_back(Array<Vector_3>(box->size()));
		d_ev3.push_back(Array<Vector_3>(box->size()));
		d_lambda.push_back(Misc::Interpol::Linear<Array<double>,3>(box, d_lambda_data[k]));
	}

	for (size_t x = 0; x < box->size(); ++x)
	{
		// copy matrix
		std::vector<double> f(6), d(6);
		for (unsigned i = 0; i < 6; ++i)
		{
			f[i] = d_ij_data[i][x];
			d[i] = f[i];
		}

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
		std::vector<double> l(3);
		l[0] = q + 2 * p * cos(F);
		l[1] = q + 2 * p * cos(F + 2./3 * M_PI);
		l[2] = q + 2 * p * cos(F + 4./3 * M_PI);
		std::sort(l.begin(), l.end());
		
		for (unsigned k = 0; k < 3; ++k)
		{
			Vector_3 e1(
				(l[k] - d[2])*d[3] + d[4]*d[1],
				(l[k] - d[0])*d[4] + d[3]*d[1],
				(l[k] - d[0])*(l[k] - d[1]) - d[0]*d[0]);
			Vector_3 e2(
				(l[k] - d[2])*(l[k] - d[4]) - d[2]*d[2],
				(l[k] - d[5])*d[1] + d[3]*d[4],
				(l[k] - d[2])*d[3] + d[1]*d[4]);
			Vector_3 e3(
				(l[k] - d[5])*d[1] + d[4]*d[3],
				(l[k] - d[5])*(l[k] - d[3]) - d[5]*d[5],
				(l[k] - d[0])*d[4] + d[1]*d[3]);

			d_ev1[k][x] = e1;
			d_ev2[k][x] = e2;
			d_ev3[k][x] = e3;
			d_lambda_data[k][x] = l[k];
		}
	}
}

double Hessian_3::eigenvalue(unsigned k, mVector<double, 3> const &x) const
{
	// copy matrix
	std::vector<double> f(6);
	for (unsigned i = 0; i < 6; ++i)
	{
		f[i] = d_ij[i](x);
	}

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

