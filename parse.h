/*
File:   parse.h
Author: Taylor Robbins
Date:   05\11\2023
*/

#ifndef _PARSE_H
#define _PARSE_H

enum PigGenParseError_t
{
	PigGenParseError_None = 0,
	PigGenParseError_ExpectedOpenCurlyBracket,
	PigGenParseError_TooManyTypes,
	PigGenParseError_MissingTypePart,
	PigGenParseError_InvalidIdentifierForStructName,
	PigGenParseError_InvalidIdentifierForMemberName,
	PigGenParseError_InvalidIdentifierForMemberType,
	PigGenParseError_NoMembersInStruct,
	PigGenParseError_FailedToGenerateSerializableStruct,
	PigGenParseError_NumCodes,
};
const char* GetPigGenParseErrorStr(PigGenParseError_t enumValue)
{
	switch (enumValue)
	{
		case PigGenParseError_None:                               return "None";
		case PigGenParseError_ExpectedOpenCurlyBracket:           return "ExpectedOpenCurlyBracket";
		case PigGenParseError_TooManyTypes:                       return "TooManyTypes";
		case PigGenParseError_MissingTypePart:                    return "MissingTypePart";
		case PigGenParseError_InvalidIdentifierForStructName:     return "InvalidIdentifierForStructName";
		case PigGenParseError_InvalidIdentifierForMemberName:     return "InvalidIdentifierForMemberName";
		case PigGenParseError_InvalidIdentifierForMemberType:     return "InvalidIdentifierForMemberType";
		case PigGenParseError_NoMembersInStruct:                  return "NoMembersInStruct";
		case PigGenParseError_FailedToGenerateSerializableStruct: return "FailedToGenerateSerializableStruct";
		default: return "Unknown";
	}
}

struct SerializableStructMember_t
{
	MyStr_t name;
	MyStr_t type;
	MyStr_t alternateType;
	bool isOptional;
};
struct SerializableStruct_t
{
	MemArena_t* allocArena;
	MyStr_t name;
	VarArray_t members; //SerializableStructMember_t
};
struct RegisteredFunc_t
{
	MemArena_t* allocArena;
	MyStr_t funcName;
};

struct GenerationLists_t
{
	MemArena_t* allocArena;
	VarArray_t allSerializableStructs; //SerializableStruct_t
	VarArray_t registeredFunctions; //RegisteredFunc_t
};

// +--------------------------------------------------------------+
// |                    New/Free Serializable                     |
// +--------------------------------------------------------------+
void FreeSerializableStructMember(MemArena_t* allocArena, SerializableStructMember_t* member)
{
	NotNull2(allocArena, member);
	FreeString(allocArena, &member->name);
	FreeString(allocArena, &member->type);
	FreeString(allocArena, &member->alternateType);
	ClearPointer(member);
}
void FreeSerializableStruct(SerializableStruct_t* serializable)
{
	NotNull(serializable);
	if (serializable->members.items != nullptr || serializable->name.chars != nullptr)
	{
		NotNull(serializable->allocArena);
		FreeString(serializable->allocArena, &serializable->name);
		VarArrayLoop(&serializable->members, mIndex)
		{
			VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
			FreeSerializableStructMember(serializable->allocArena, member);
		}
		FreeVarArray(&serializable->members);
	}
	ClearPointer(serializable);
}

SerializableStructMember_t* AddSerializableStructMember(SerializableStruct_t* serializable, MyStr_t name, MyStr_t type, MyStr_t alternateType)
{
	NotNull2(serializable, serializable->allocArena);
	NotEmptyStr(&name);
	NotEmptyStr(&type);
	NotNullStr(&alternateType);
	
	SerializableStructMember_t* result = VarArrayAdd(&serializable->members, SerializableStructMember_t);
	NotNull(result);
	ClearPointer(result);
	result->name = AllocString(serializable->allocArena, &name);
	result->type = AllocString(serializable->allocArena, &type);
	if (!IsEmptyStr(alternateType))
	{
		result->alternateType = AllocString(serializable->allocArena, &alternateType);
	}
	
	return result;
}
void NewSerializableStruct(SerializableStruct_t* serializableOut, MemArena_t* memArena, MyStr_t name)
{
	NotNull(serializableOut);
	ClearPointer(serializableOut);
	serializableOut->allocArena = memArena;
	serializableOut->name = AllocString(serializableOut->allocArena, &name);
	CreateVarArray(&serializableOut->members, serializableOut->allocArena, sizeof(SerializableStructMember_t));
}

SerializableStructMember_t* FindSerializableMemberByName(SerializableStruct_t* serializable, MyStr_t name)
{
	NotNull2(serializable, serializable->allocArena);
	NotNullStr(&name);
	VarArrayLoop(&serializable->members, mIndex)
	{
		VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
		if (StrEquals(member->name, name)) { return member; }
	}
	return nullptr;
}

// +--------------------------------------------------------------+
// |                  New/Free Generation Lists                   |
// +--------------------------------------------------------------+
void FreeGenerationLists(GenerationLists_t* lists)
{
	NotNull(lists);
	VarArrayLoop(&lists->allSerializableStructs, sIndex)
	{
		VarArrayLoopGet(SerializableStruct_t, serializableStruct, &lists->allSerializableStructs, sIndex);
		FreeSerializableStruct(serializableStruct);
	}
	FreeVarArray(&lists->allSerializableStructs);
	VarArrayLoop(&lists->registeredFunctions, fIndex)
	{
		VarArrayLoopGet(RegisteredFunc_t, registeredFunc, &lists->registeredFunctions, fIndex);
		FreeString(registeredFunc->allocArena, &registeredFunc->funcName);
	}
	FreeVarArray(&lists->registeredFunctions);
	ClearPointer(lists);
}

void InitGenerationLists(MemArena_t* memArena, GenerationLists_t* listsOut)
{
	NotNull2(memArena, listsOut);
	ClearPointer(listsOut);
	listsOut->allocArena = memArena;
	CreateVarArray(&listsOut->allSerializableStructs, memArena, sizeof(SerializableStruct_t));
	CreateVarArray(&listsOut->registeredFunctions, memArena, sizeof(RegisteredFunc_t));
}

#endif //  _PARSE_H
