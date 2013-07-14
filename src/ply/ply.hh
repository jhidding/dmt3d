#pragma once
#include <string>
#include <iostream>

namespace Misc
{
	class PLY
	{
		using System::ptr;

		class Header;
		class Element;
		class Property
		{};

		template <typename T>
		class Scalar: public Property
		{
			public:
				using Property::Property;
		}

		template <typename T, typename length_type = uint8_t>
		class List: public Property
		{
			public:
				using Property::Property;
		};

		Header 						m_header;
		std::vector<ptr<Element>>	m_elements;

		public:
			template <typename ...Args>
			void add_element(std::string const &name, Args &&...properties)
			{
				m_header.add_element(name);
			}

			template <typename T, typename ...Args>
			void add_properties(
	};

	// PLY::Header {{{1
	// PLY::Header {{{2
	class PLY::Header
	{
		public:
			class Element;

			enum Format { ASCII, BINARY };
			typedef std::string Comment;

		private:
			Format 								m_format;
			std::vector<Comment> 				m_comment;
			std::vector<ptr<Element>>			m_elements;

		public:
			Format format() const 
				{ return m_format; }
			std::vector<Comment> const &comments() const 
				{ return m_comments; }
			std::vector<ptr<Element>> const &elements() const 
				{ return m_elements; }

			void add_element(std::string const &name)
			{
				m_elements.push_back(new Element(name));
			}

			template <typename T>
			void add_scalar(std::string const &name)
			{
				m_elements.back()->add_scalar<T>(name);
			}

			template <typename T, typename length_type = uint8_t>
			void add_list(std::string const &name)
			{
				m_elements.back()->add_list<T, length_type>(name);
			}

			void add_item()
			{
				m_elements.back()->add_item();
			}
	};
	// }}}2 PLY::Header
	
	// PLY::Header::Element {{{2
	class Element
	{
		template <typename T>
		struct type { static std::string name; }

		class Property
		{
			std::string m_name;

			public:
				Property(std::string const &name_): m_name(name_) {}
				std::string const &name() const { return m_name; }
				std::string type_expression() const = 0;
		};

		template <typename T>
		class Scalar: public Property
		{
			public:
				using Property::Property;

				std::string type_expression() const
				{ return type<T>::name; }
		};

		template <typename T, typename length_type = uint8_t>
		class List: public Property
		{
			public:
				using Property::Property;

				std::string type_expression() const
				{ return "list " + type<length_type>::name + " " 
					+ type<T>::name; }
		};

		std::string 						m_name;
		size_t								m_count;
		std::vector<ptr<Property>> 			m_properties;

		public:
			Element(std::string const &name_): m_name(name_), m_count(0) {}
			std::string const &name() const { return m_name; }
			size_t count() const { return m_count; }

			template <typename T>
			void add_scalar(std::string const &name)
			{
				m_properties.push_back(new Scalar<T>(name));
			}

			template <typename T, typename length_type = uint8_t>
			void add_list(std::string const &name)
			{
				m_properties.push_back(new List<T, length_type>(name));
			}

			void add_item()
			{
				++m_count;
			}
	};
	// }}}2 PLY::Header::Element
	// }}}1 PLY::Header
}

// vim:ts=4:sw=4:fdm=marker
