#include "ply.hh"

using namespace Misc;

#ifdef UNITTEST
#include "../base/unittest.hh"
#include "../base/mvector.hh"
#include "../base/mdrange.hh"
#include "../base/map.hh"

using namespace System;

Test::Unit PLY_test(
	"1000:PLY",
	"Testing the writing of a correct .ply file. "
	"If this test succeeds, it should write a correct "
	"mesh to a test file called ply_test.ply. ",
	[] ()
{
	PLY ply;
	int N = 50;

	auto grid = map(MdRange<2>(mVector<int,2>(N)),
		[N] (mVector<int,2> const &X) { 
		return mVector<double,2>(X) * (2./N) - mVector<double,2>(1.0);
	});

	auto func =	[] (mVector<double,2> const &X) {
		double v = X.norm();
		return (v == 0 ? 1.0 : sin(v * 4*M_PI) / (v * 4*M_PI));
	};

	ply.add_comment("file written as test to DMT3D .ply writer");
	ply.add_element("vertex", 
		PLY::scalar_type<float>("x"), 
		PLY::scalar_type<float>("y"), 
		PLY::scalar_type<float>("z"),
		PLY::scalar_type<uint8_t>("red"),
		PLY::scalar_type<uint8_t>("green"),
		PLY::scalar_type<uint8_t>("blue")
	);
	for (auto p : grid)
		ply.put_data(
			PLY::scalar<float>(p[0]), 
			PLY::scalar<float>(p[1]), 
			PLY::scalar<float>(func(p)),
			PLY::scalar<uint8_t>(255),
			PLY::scalar<uint8_t>(255),
			PLY::scalar<uint8_t>(0)
		);

	ply.add_element("face",
		PLY::list_type<unsigned>("vertex_index"),
		PLY::scalar_type<uint8_t>("red"),
		PLY::scalar_type<uint8_t>("green"),
		PLY::scalar_type<uint8_t>("blue")
	);
	mVector<int,2> dx({0, 1}), dy({1, 0});
	for (auto p : MdRange<2>(mVector<int,2>(N-1)))
	{
		std::vector<unsigned> v1, v2;
		v1.push_back(p[0] +     N * p[1]);
		v1.push_back(p[0] + 1 + N * p[1]);
		v1.push_back(p[0] + 1 + N * p[1] + N);
		ply.put_data(PLY::list<unsigned>(v1),
			PLY::scalar<uint8_t>(255),
			PLY::scalar<uint8_t>(0),
			PLY::scalar<uint8_t>(0)
		);
		v2.push_back(p[0] + 1 + N * p[1] + N);
		v2.push_back(p[0] +     N * p[1] + N);
		v2.push_back(p[0] +     N * p[1]);
		ply.put_data(PLY::list<unsigned>(v2),
			PLY::scalar<uint8_t>(255),
			PLY::scalar<uint8_t>(0),
			PLY::scalar<uint8_t>(0)
		);
	}

	ply.write("ply_test.ply", PLY::ASCII);

	return true;
});

#endif

template <> std::string PLY::Type<int8_t>::name = "char";
template <> std::string PLY::Type<uint8_t>::name = "uchar";
template <> std::string PLY::Type<int16_t>::name = "short";
template <> std::string PLY::Type<uint16_t>::name = "ushort";	
template <> std::string PLY::Type<int32_t>::name = "int";
template <> std::string PLY::Type<uint32_t>::name = "uint";
template <> std::string PLY::Type<float>::name = "float";
template <> std::string PLY::Type<double>::name = "double";

std::ostream &Misc::operator<<(std::ostream &out, PLY::Header::Property const &property)
{
	return out << "property " << property.type_expression() << " " << property.name() << std::endl;
}

std::ostream &Misc::operator<<(std::ostream &out, PLY::Header::Element const &element)
{
	out << "element " << element.name() << " " << element.count() << std::endl;

	for (auto &property : element.properties())
		out << *property;

	return out;
}

std::ostream &Misc::operator<<(std::ostream &out, PLY::Header const &header)
{
	out << "ply" << std::endl
		<< "format " << (header.format() == PLY::ASCII ? "ascii" : "binary_little_endian") 
		<< " 1.0" << std::endl;

	for (auto &comment : header.comments())
		out << "comment " << comment << std::endl;

	for (auto &element : header.elements())
		out << *element;

	out << "end_header" << std::endl;

	return out;
}

std::ostream &Misc::operator<<(std::ostream &out, PLY::Data const &data)
{
	if (data.format() == PLY::ASCII)
		data.write_ascii(out);
	else
		data.write_binary(out);

	return out;
}

void PLY::write(std::string const &filename, PLY::Format format) const
{
	m_header.set_format(format);
	m_data.set_format(format);
	std::ofstream out(filename);
	out << m_header;
	out << m_data;
	out.close();
}
// vim:ts=4:sw=4
