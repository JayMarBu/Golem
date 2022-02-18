#pragma once
#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>
#include <cstddef>
#include <iterator>
#include <utility>

namespace golem
{

	// TYPE DESCRIPTOR STRUCT *************************************************************************
	struct TypeDescriptor
	{
		// **** Members ****
		const char* name;
		size_t size;

		// **** Methods ****
		TypeDescriptor(const char* name, size_t size)
			: name{ name }, size{ size } {}

		virtual ~TypeDescriptor() {}

		virtual std::string GetName() const { return name; }
		virtual void Dump(const void* obj, int indentLevel = 0) const = 0;
	};

	// TYPE DESCRIPTOR FIND & CREATE UTILITIES ********************************************************
	template <typename T>
	TypeDescriptor* GetPrimitiveDescriptor();

	struct DefaultResolver
	{
		template <typename T> static char func(decltype(&T::Reflection));
		template <typename T> static int func(...);

		template <typename T>
		struct IsReflected
		{
			enum { value = (sizeof(func<T>(nullptr)) == sizeof(char)) };
		};

		// This version is called if T has a static member named "Reflection":
		template <typename T, typename std::enable_if<IsReflected<T>::value, int>::type = 0>
		static TypeDescriptor* Get()
		{
			return &T::Reflection;
		}

		// This version is called otherwise:
		template <typename T, typename std::enable_if<!IsReflected<T>::value, int>::type = 0>
		static TypeDescriptor* Get()
		{
			return GetPrimitiveDescriptor<T>();
		}
	};


	template <typename T>
	struct TypeResolver
	{
		static TypeDescriptor* Get()
		{
			return DefaultResolver::Get<T>();
		}
	};

	// TYPE DESCRIPTOR FOR USER DEFINED STRUCTS/ CLASSES **********************************************
	struct TypeDescriptor_Struct : TypeDescriptor
	{
		struct Member
		{
			const char* name;
			size_t offset;
			TypeDescriptor* type;
		};

		std::vector<Member> members;

		std::vector<TypeDescriptor*> childClases;
		TypeDescriptor* parentClass = nullptr;

		TypeDescriptor_Struct(void (*init)(TypeDescriptor_Struct*)) : TypeDescriptor{ nullptr, 0 }
		{
			init(this);
		}

		TypeDescriptor_Struct(const char* name, size_t size, const std::initializer_list<Member>& init)
			: TypeDescriptor{ nullptr, 0 }, members{ init }
		{}

		virtual void Dump(const void* obj, int indentLevel) const override
		{
			/*gef::DebugOut((std::string(4 * indentLevel, ' ') + GetName()).c_str());
			gef::DebugOut("\n");
			gef::DebugOut((std::string(4 * indentLevel, ' ') + "{\n").c_str());
			for (const Member& member : members)
			{
				gef::DebugOut(std::string(std::string(4 * (indentLevel + 1), ' ') + std::string(member.name) + "=").c_str());
				member.type->Dump((char*)obj + member.offset, indentLevel + 1);
				gef::DebugOut("\n");
			}
			gef::DebugOut((std::string(4 * indentLevel, ' ') + "}").c_str());*/
		}

		virtual int GetMember(const char* memberName) const
		{
			for (int i = 0; i < members.size(); i++)
			{
				if (members[i].name == memberName)
					return i;
			}

			return -1;
		}
	};


#define TYPE_TO_STRING(T) #T

	// REFLECT MACRO **********************************************************************************
	// - creates Init reflect function definition inside class
	// - creates static TypeDescriptor inside class
	// - creates friend link with type resolver class
#define REFLECT() \
friend struct golem::DefaultResolver; \
static golem::TypeDescriptor_Struct Reflection; \
static void InitReflection(golem::TypeDescriptor_Struct*);

// REFLECT BEGIN MACRO ****************************************************************************
// - initializes static TypeDescriptor
// - InitReflect function implementation
#define REFLECT_STRUCT_BEGIN(type) \
golem::TypeDescriptor_Struct type::Reflection{type::InitReflection}; \
void type::InitReflection(golem::TypeDescriptor_Struct* typeDesc) \
{ \
    using T = type; \
    typeDesc->name = #type; \
    typeDesc->size = sizeof(T); \
    typeDesc->members = \
	{

// REFLECT BEGIN TEMPLATED MACRO ******************************************************************
// - initializes static TypeDescriptor
// - InitReflect function implementation
#define REFLECT_TEMPLATED_STRUCT_BEGIN(type, ...) \
template <>\
void type<__VA_ARGS__>::InitReflection(golem::TypeDescriptor_Struct* typeDesc)\
{\
	using T = type<__VA_ARGS__>;\
	typeDesc->name = TYPE_TO_STRING(type<__VA_ARGS__>);\
	typeDesc->size = sizeof(T);\
	typeDesc->members =\
	{

// REFLECT MEMBER CREATE MACRO ********************************************************************
// - adds member variables to members list
#define REFLECT_STRUCT_MEMBER(name) \
        {#name, offsetof(T, name), golem::TypeResolver<decltype(T::name)>::Get()},

// REFLECT END MACRO ******************************************************************************
#define REFLECT_STRUCT_END() \
    }; \
}

// REFLECT TEMPLATE MACRO STATIC MEMBER ***********************************************************
// - Must come after struct begin macro
#define REFLECT_TEMPLATED_STRUCT_STATIC_MEMBER(type, ...)\
template <>\
golem::TypeDescriptor_Struct type<__VA_ARGS__>::Reflection{ type<__VA_ARGS__>::InitReflection };

// STD::VECTOR REFLECTION UTILITIES ***************************************************************

	struct TypeDescriptor_StdVector : TypeDescriptor
	{
		// **** Members **** 
		TypeDescriptor* itemType;
		bool itemIsPtr;
		size_t(*GetSize)(const void*, const bool&);
		const void* (*GetItem)(const void*, size_t);
		const void* (*GetItemDeRef)(const void*, size_t);

		// **** Methods ****
		template <typename ItemType>
		TypeDescriptor_StdVector(ItemType*, const bool& isPtr)
			: TypeDescriptor{ "std::vector<>", (isPtr) ? sizeof(std::vector<ItemType*>) : sizeof(std::vector<ItemType>) },
			itemType{ TypeResolver<ItemType>::Get() },
			itemIsPtr{ isPtr }
		{
			GetSize = [](const void* vecPtr, const bool& isPtrType) -> size_t
			{
				if (isPtrType)
				{
					const auto& vec = *(const std::vector<ItemType*>*) vecPtr;
					return vec.size();
				}
				else
				{
					const auto& vec = *(const std::vector<ItemType>*) vecPtr;
					return vec.size();
				}
			};

			GetItem = [](const void* vecPtr, size_t index) -> const void*
			{
				const auto& vec = *(const std::vector<ItemType>*) vecPtr;
				return &vec[index];
			};

			GetItemDeRef = [](const void* vecPtr, size_t index) -> const void*
			{
				const auto& vec = *(const std::vector<ItemType*>*) vecPtr;
				const auto& itemPtr = vec[index];
				if (itemPtr == nullptr)
					return nullptr;

				const auto& data = *itemPtr;
				return vec[index];
			};
		}

		virtual std::string GetName() const override
		{
			return std::string("std::vector<") + itemType->GetName() + ((itemIsPtr) ? "*>" : ">");
		}

		virtual void Dump(const void* obj, int indentLevel) const override
		{
			/*size_t numItems = GetSize(obj, itemIsPtr);
			//std::cout << GetName();
			gef::DebugOut((std::string(4 * indentLevel, ' ') + GetName()).c_str());
			gef::DebugOut("\n");

			if (numItems == 0)
			{
				gef::DebugOut((std::string(4 * indentLevel, ' ') + "{}\n").c_str());
			}
			else
			{
				gef::DebugOut((std::string(4 * indentLevel, ' ') + "{\n").c_str());
				for (size_t index = 0; index < numItems; index++)
				{
					gef::DebugOut((std::string(4 * (indentLevel + 1), ' ') + "[" + std::to_string(index) + "]").c_str());
					if (itemIsPtr)
					{
						gef::DebugOut("->");
						itemType->Dump(GetItemDeRef(obj, index), indentLevel + 1);
					}
					else
						itemType->Dump(GetItem(obj, index), indentLevel + 1);
					gef::DebugOut("\n");
				}
				gef::DebugOut((std::string(4 * indentLevel, ' ') + "}").c_str());
			}*/
		}
	};

	//template<typename T>
	//struct is_pointer { static const bool value = false; };

	//template<typename T>
	//struct is_pointer<T*> { static const bool value = true; };

	// Partially specialize TypeResolver<> for std::vectors:
	template <typename T>
	class TypeResolver<std::vector<T>>
	{
	public:
		static TypeDescriptor* Get()
		{
			gef::DebugOut("regular get\n");
			static TypeDescriptor_StdVector typeDesc{ (T*) nullptr, false };
			return &typeDesc;
		}
	};

	template <typename T>
	class TypeResolver<std::vector<T*>>
	{
	public:
		static TypeDescriptor* Get()
		{
			gef::DebugOut("pointer get\n");
			static TypeDescriptor_StdVector typeDesc{ (T*) nullptr, true };
			return &typeDesc;
		}
	};


	// STD::PAIR REFLECTION UTILITIES *****************************************************************
	struct TypeDescriptor_StdPair : TypeDescriptor
	{
		// **** Members **** 
		TypeDescriptor* keyType;
		TypeDescriptor* valueType;
		const void* (*GetKey)(const void*);
		const void* (*GetValue)(const void*);


		// **** Methods ****
		template <typename KeyType, typename ValueType>
		TypeDescriptor_StdPair(KeyType*, ValueType*)
			: TypeDescriptor{ "std::pair<>", sizeof(std::pair<KeyType,ValueType>) },
			keyType{ TypeResolver<KeyType>::Get() },
			valueType{ TypeResolver<ValueType>::Get() }
		{
			GetKey = [](const void* pairPtr) -> const void*
			{
				const auto& p = *(const std::pair<KeyType, ValueType>*) pairPtr;
				return &p.first;
			};

			GetValue = [](const void* pairPtr) -> const void*
			{
				const auto& p = *(const std::pair<KeyType, ValueType>*) pairPtr;
				return &p.second;
			};
		}

		virtual std::string GetName() const override
		{
			return std::string("std::pair< ") + keyType->GetName() + ", " + valueType->GetName() + ">";
		}

		virtual void Dump(const void* obj, int indentLevel) const override
		{
			/*gef::DebugOut((std::string(4 * indentLevel, ' ') + GetName()).c_str());
			gef::DebugOut("\n");
			gef::DebugOut((std::string(4 * indentLevel, ' ') + "{\n").c_str());

			gef::DebugOut((std::string(4 * (indentLevel + 1), ' ') + "first: \n").c_str());
			gef::DebugOut(std::string(4 * (indentLevel + 2), ' ').c_str());
			keyType->Dump(GetKey(obj), indentLevel + 2);
			gef::DebugOut("\n\n");

			gef::DebugOut((std::string(4 * (indentLevel + 1), ' ') + "second: \n").c_str());
			valueType->Dump(GetValue(obj), indentLevel + 2);
			gef::DebugOut("\n");

			gef::DebugOut((std::string(4 * indentLevel, ' ') + "}").c_str());*/
		}
	};

	// Partially specialize TypeResolver<> for std::vectors:
	template <typename T1, typename T2>
	class TypeResolver<std::pair<T1, T2>>
	{
	public:
		static TypeDescriptor* Get()
		{
			static TypeDescriptor_StdPair typeDesc{ (T1*) nullptr, (T2*) nullptr };
			return &typeDesc;
		}
	};

}
