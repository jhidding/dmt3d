#include "../base/system.hh"
#include "../base/format.hh"
#include "../base/boxconfig.hh"
#include "../base/fourier.hh"
#include "../base/fft.hh"
#include "../misc/interpol.hh"


#include "dmt.hh"
#include "../dmt/hessian.hh"

using namespace System;

#include <iomanip>
inline std::string time_string(double b)
{
	std::ostringstream ss;
	ss << std::setfill('0') << std::setw(5) << (int)round(b * 10000);
	return ss.str();
}

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

	DMT::Hessian_3<> hessian(box, potential);

	//unsigned bits = H.get<unsigned>("mbits");
	//typedef Map<Array<mVector<double, 3>>, double> AI;
	auto za_data = load_from_file<mVector<double, 3>>(fi, "displacement");
	Misc::Interpol::Spline<Array<mVector<double,3>>,3> za(box->bits(), za_data);
	std::string tstr = time_string(b);

	for (unsigned l = 0; l < 3; ++l)
	{
		DMT::Folds::run(box, [&hessian, b, l] (DMT::Folds::Point const &q)
		{
			mVector<double, 3> Q; Q[0] = q[0]; Q[1] = q[1]; Q[2] = q[2];
			return hessian.eigenvalue(l, Q) + 1./b;
		}, args.get<double>("res"), [&za, b] (DMT::Folds::Point const &q)
		{
			mVector<double, 3> Q; Q[0] = q[0]; Q[1] = q[1]; Q[2] = q[2];
			mVector<double, 3> X = Q + za(Q) * b;
			return DMT::Folds::Point(X[0], X[1], X[2]);
		}, Misc::format(args["id"], ".l", l, ".folds.", tstr, ".off"));
	}

}

Global<Command> _COMMAND_FOLDS("folds", command_folds);


