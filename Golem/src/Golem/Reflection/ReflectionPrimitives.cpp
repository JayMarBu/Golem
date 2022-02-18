#include "golpch.h"
#include "Reflection.h"

namespace golem
{
	// Type descriptors for primitive data structures
#define DEBUG_NULL_CHECK(obj)\
	if(obj == nullptr){\
		gef::DebugOut("nullptr");\
		return;\
	}


#define DEBUG_DUMP(type)\
	if(obj == nullptr)\
		gef::DebugOut("nullptr");\
	else\
		gef::DebugOut(std::string(#type+std::string("{\"") + std::to_string(*(const type*)obj) + "\"}").c_str());

	// int ************************************************************************************

	struct TypeDescriptor_Int : TypeDescriptor
	{
		TypeDescriptor_Int()
			: TypeDescriptor{ "int", sizeof(int) }
		{}

		virtual void Dump(const void* obj, int /* unused */) const override
		{
			//DEBUG_DUMP(int)
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<int>() {
		static TypeDescriptor_Int typeDesc;
		return &typeDesc;
	}

	struct TypeDescriptor_UInt : TypeDescriptor
	{
		TypeDescriptor_UInt()
			: TypeDescriptor{ "unsigned int", sizeof(int) }
		{}

		virtual void Dump(const void* obj, int /* unused */) const override
		{
			//DEBUG_DUMP(unsigned int)
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<unsigned int>() {
		static TypeDescriptor_UInt typeDesc;
		return &typeDesc;
	}

	// bool ***********************************************************************************

	struct TypeDescriptor_Bool : TypeDescriptor
	{
		TypeDescriptor_Bool()
			: TypeDescriptor{ "bool", sizeof(bool) }
		{}

		virtual void Dump(const void* obj, int /* unused */) const override
		{
			//DEBUG_DUMP(bool)
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<bool>() {
		static TypeDescriptor_Bool typeDesc;
		return &typeDesc;
	}

	// float **********************************************************************************

	struct TypeDescriptor_Float : TypeDescriptor
	{
		TypeDescriptor_Float()
			: TypeDescriptor{ "float", sizeof(float) }
		{}

		virtual void Dump(const void* obj, int /* unused */) const override
		{
			//DEBUG_DUMP(float)
				//gef::DebugOut(std::string("float"+std::string("{\"") + std::to_string(*(const float*)obj) + "\"}").c_str());
				//std::cout << "float{" << *(const float*)obj << "}";
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<float>() {
		static TypeDescriptor_Float typeDesc;
		return &typeDesc;
	}

	// double *********************************************************************************

	struct TypeDescriptor_Double : TypeDescriptor
	{
		TypeDescriptor_Double()
			: TypeDescriptor{ "double", sizeof(double) }
		{}

		virtual void Dump(const void* obj, int /* unused */) const override
		{
			//DEBUG_DUMP(double)
				//gef::DebugOut(std::string("double{\"" + std::to_string(*(const double*)obj) + "\"}").c_str());
				//std::cout << "double{" << *(const double*)obj << "}";
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<double>() {
		static TypeDescriptor_Double typeDesc;
		return &typeDesc;
	}

	// std::string ****************************************************************************

	struct TypeDescriptor_StdString : TypeDescriptor
	{
		TypeDescriptor_StdString()
			: TypeDescriptor{ "std::string", sizeof(std::string) }
		{}

		virtual void Dump(const void* obj, int /* unused */) const override
		{
			//DEBUG_NULL_CHECK(obj)

			//	gef::DebugOut(std::string("std::string" + std::string("{\"") + *(const std::string*)obj + "\"}").c_str());
		}
	};

	template <>
	TypeDescriptor* GetPrimitiveDescriptor<std::string>()
	{
		static TypeDescriptor_StdString typeDesc;
		return &typeDesc;
	}
	
}