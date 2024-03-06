/*
File:   generate.cpp
Author: Taylor Robbins
Date:   05\26\2023
Description: 
	** Holds functions that generate C code from information that has been parsed in parse.cpp
*/

MyStr_t PigGenGenerateFileComment(MemArena_t* memArena, MyStr_t sourceCodeFilePath)
{
	StringBuilder_t builder;
	NewStringBuilder(&builder, memArena);
	builder.newLineStyle = PIGGEN_NEW_LINE;
	StringBuilderAppendPrintLine (&builder, "// This file was generated by PigGen.exe v%u.%02u(%03u)", PIGGEN_VERSION_MAJOR, PIGGEN_VERSION_MINOR, PIGGEN_VERSION_BUILD);
	StringBuilderAppendPrintLine (&builder, "// Source: %.*s", sourceCodeFilePath.length, sourceCodeFilePath.chars);
	StringBuilderAppendPrintLine (&builder, "// Date: %s", FormatRealTimeNt(&pig->realTime, TempArena));
	StringBuilderAppendLine      (&builder, "");
	return TakeString(&builder);
}

// +--------------------------------------------------------------+
// |                     Serializable Struct                      |
// +--------------------------------------------------------------+
MyStr_t PigGenGenerateSerializableStructCode(MemArena_t* memArena, SerializableStruct_t* serializable)
{
	NotNull(serializable);
	StringBuilder_t builder;
	NewStringBuilder(&builder, memArena);
	builder.newLineStyle = PIGGEN_NEW_LINE;
	
	StringBuilderAppendPrintLine (&builder, "struct %.*s", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	VarArrayLoop(&serializable->members, mIndex)
	{
		VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
		StringBuilderAppendPrintLine(&builder, "\t%.*s %.*s;", member->type.length, member->type.chars, member->name.length, member->name.chars);
	}
	StringBuilderAppendLine      (&builder, "};");
	StringBuilderAppendLine      (&builder, "");
	StringBuilderAppendPrintLine (&builder, "#define %.*s_NumMembers %llu", serializable->name.length, serializable->name.chars, serializable->members.length);
	StringBuilderAppendPrintLine (&builder, "#define %.*s_SlzSize ( \\", serializable->name.length, serializable->name.chars);
	VarArrayLoop(&serializable->members, mIndex)
	{
		VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
		MyStr_t alternateTypeOrType = IsEmptyStr(member->alternateType) ? member->type : member->alternateType;
		StringBuilderAppendPrintLine(&builder, "\tSlzMemberTypeSize_%.*s%s", alternateTypeOrType.length, alternateTypeOrType.chars, (mIndex+1 < serializable->members.length) ? " + \\" : ")");
	}
	StringBuilderAppendLine      (&builder, "");
	#if 0
	StringBuilderAppendPrintLine (&builder, "SerializableStructMemberType_t GetSerializableMemberType_%.*s(u64 memberIndex)", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendLine      (&builder, "\tswitch (memberIndex)");
	StringBuilderAppendLine      (&builder, "\t{");
	VarArrayLoop(&serializable->members, mIndex)
	{
		VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
		MyStr_t alternateTypeOrType = IsEmptyStr(member->alternateType) ? member->type : member->alternateType;
		StringBuilderAppendPrintLine(&builder, "\t\tcase %llu: return SerializableStructMemberType_%.*s; //%.*s", mIndex, alternateTypeOrType.length, alternateTypeOrType.chars, member->name.length, member->name.chars);
	}
	StringBuilderAppendLine      (&builder, "\t\tdefault: Assert(false); return SerializableStructMemberType_None;");
	StringBuilderAppendLine      (&builder, "\t}");
	StringBuilderAppendLine      (&builder, "}");
	StringBuilderAppendLine      (&builder, "");
	StringBuilderAppendPrintLine (&builder, "SerializableStructMemberType_t GetSerializableMemberType(const %.*s* structPntr, u64 memberIndex)", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendPrintLine (&builder, "\treturn GetSerializableMemberType_%.*s(memberIndex);", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "}");
	StringBuilderAppendLine      (&builder, "");
	StringBuilderAppendPrintLine (&builder, "const char* GetSerializableMemberName_%.*s(u64 memberIndex)", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendLine      (&builder, "\tswitch(memberIndex)");
	StringBuilderAppendLine      (&builder, "\t{");
	VarArrayLoop(&serializable->members, mIndex)
	{
		VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
		StringBuilderAppendPrintLine (&builder, "\t\tcase %llu: return \"%.*s\"; //%.*s", mIndex, member->name.length, member->name.chars, member->type.length, member->type.chars);
	}
	StringBuilderAppendLine      (&builder, "\t\tdefault: Assert(false); return nullptr;");
	StringBuilderAppendLine      (&builder, "\t}");
	StringBuilderAppendLine      (&builder, "}");
	StringBuilderAppendLine      (&builder, "");
	StringBuilderAppendPrintLine (&builder, "const char* GetSerializableMemberName(const %.*s* structPntr, u64 memberIndex)", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendPrintLine (&builder, "\treturn GetSerializableMemberName_%.*s(memberIndex);", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "}");
	StringBuilderAppendLine      (&builder, "");
	#endif
	StringBuilderAppendPrintLine (&builder, "SlzMemberType_t SlzTypes_%.*s[%.*s_NumMembers] = {", serializable->name.length, serializable->name.chars, serializable->name.length, serializable->name.chars);
	VarArrayLoop(&serializable->members, mIndex)
	{
		VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
		MyStr_t alternateTypeOrType = IsEmptyStr(member->alternateType) ? member->type : member->alternateType;
		StringBuilderAppendPrintLine(&builder, "\tSlzMemberType_%.*s, //%.*s", alternateTypeOrType.length, alternateTypeOrType.chars, member->name.length, member->name.chars);
	}
	StringBuilderAppendLine      (&builder, "};");
	StringBuilderAppendPrintLine (&builder, "const char* SlzNames_%.*s[%.*s_NumMembers] = {", serializable->name.length, serializable->name.chars, serializable->name.length, serializable->name.chars);
	VarArrayLoop(&serializable->members, mIndex)
	{
		VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
		StringBuilderAppendPrintLine(&builder, "\t\"%.*s\", //%.*s", member->name.length, member->name.chars, member->type.length, member->type.chars);
	}
	StringBuilderAppendLine      (&builder, "};");
	StringBuilderAppendLine      (&builder, "");
	StringBuilderAppendPrintLine (&builder, "SlzMemberType_t GetSerializableMemberType(const %.*s* structPntr, u64 memberIndex)", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendLine      (&builder, "\tUNUSED(structPntr);");
	StringBuilderAppendPrintLine (&builder, "\tAssert(memberIndex < %.*s_NumMembers);", serializable->name.length, serializable->name.chars);
	StringBuilderAppendPrintLine (&builder, "\treturn SlzTypes_%.*s[memberIndex];", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "}");
	StringBuilderAppendLine      (&builder, "");
	StringBuilderAppendPrintLine (&builder, "const char* GetSerializableMemberName(const %.*s* structPntr, u64 memberIndex)", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendLine      (&builder, "\tUNUSED(structPntr);");
	StringBuilderAppendPrintLine (&builder, "\tAssert(memberIndex < %.*s_NumMembers);", serializable->name.length, serializable->name.chars);
	StringBuilderAppendPrintLine (&builder, "\treturn SlzNames_%.*s[memberIndex];", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "}");
	StringBuilderAppendLine      (&builder, "");
	StringBuilderAppendPrintLine (&builder, "void* GetSerializableMemberPntr(%.*s* structPntr, u64 memberIndex)", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendLine      (&builder, "\tswitch (memberIndex)");
	StringBuilderAppendLine      (&builder, "\t{");
	VarArrayLoop(&serializable->members, mIndex)
	{
		VarArrayLoopGet(SerializableStructMember_t, member, &serializable->members, mIndex);
		StringBuilderAppendPrintLine (&builder, "\t\tcase %llu: return &structPntr->%.*s;", mIndex, member->name.length, member->name.chars);
	}
	StringBuilderAppendLine      (&builder, "\t\tdefault: Assert(false); return nullptr;");
	StringBuilderAppendLine      (&builder, "\t}");
	StringBuilderAppendLine      (&builder, "}");
	StringBuilderAppendPrintLine (&builder, "const void* GetSerializableMemberPntr(const %.*s* structPntr, u64 memberIndex) //const-variant", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendPrintLine (&builder, "\treturn (const void*)GetSerializableMemberPntr((%.*s*)structPntr, memberIndex);", serializable->name.length, serializable->name.chars);
	StringBuilderAppendLine      (&builder, "}");
	
	return TakeString(&builder);
}

MyStr_t PigGenGenerateAllSerializableStructsCode(MemArena_t* memArena, VarArray_t* allSerializableStructs)
{
	NotNull(allSerializableStructs);
	StringBuilder_t builder;
	NewStringBuilder(&builder, memArena);
	builder.newLineStyle = PIGGEN_NEW_LINE;
	
	StringBuilderAppendLine      (&builder, "enum SlzStruct_t");
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendLine      (&builder, "\tSlzStruct_None = 0,");
	VarArrayLoop(allSerializableStructs, sIndex)
	{
		VarArrayLoopGet(SerializableStruct_t, serializableStruct, allSerializableStructs, sIndex);
		StringBuilderAppendPrintLine (&builder, "\tSlzStruct_%.*s,", serializableStruct->name.length, serializableStruct->name.chars);
	}
	StringBuilderAppendLine      (&builder, "\tSlzStruct_NumStructs,");
	StringBuilderAppendLine      (&builder, "};");
	StringBuilderAppendLine      (&builder, "");
	StringBuilderAppendLine      (&builder, "const char* GetSlzStructStr(SlzStruct_t slzStruct)");
	StringBuilderAppendLine      (&builder, "{");
	StringBuilderAppendLine      (&builder, "\tswitch (slzStruct)");
	StringBuilderAppendLine      (&builder, "\t{");
	VarArrayLoop(allSerializableStructs, sIndex)
	{
		VarArrayLoopGet(SerializableStruct_t, serializableStruct, allSerializableStructs, sIndex);
		StringBuilderAppendPrintLine(&builder, "\t\tcase SlzStruct_%.*s: return \"%.*s\";", StrPrint(serializableStruct->name), StrPrint(serializableStruct->name));
	}
	StringBuilderAppendLine      (&builder, "\t\tdefault: return \"Unknown\";");
	StringBuilderAppendLine      (&builder, "\t}");
	StringBuilderAppendLine      (&builder, "}");
	
	return TakeString(&builder);
}

MyStr_t PigGenGenerateFillGlobalFuncTableImplementationCode(MemArena_t* memArena, VarArray_t* registeredFunctions)
{
	NotNull(registeredFunctions);
	StringBuilder_t builder;
	NewStringBuilder(&builder, memArena);
	builder.newLineStyle = PIGGEN_NEW_LINE;
	
	StringBuilderAppendLine(&builder, "void FillGlobalFuncTable()");
	StringBuilderAppendLine(&builder, "{");
	if (registeredFunctions->length > 0)
	{
		VarArrayLoop(registeredFunctions, fIndex)
		{
			VarArrayLoopGet(RegisteredFunc_t, registeredFunc, registeredFunctions, fIndex);
			StringBuilderAppendPrintLine(&builder, "\tGlobalFuncTable[%llu] = %.*s;", fIndex, StrPrint(registeredFunc->funcName));
		}
	}
	else
	{
		StringBuilderAppendLine(&builder, "\t// No registered functions");
		StringBuilderAppendLine(&builder, "\tGlobalFuncTable[0] = nullptr;");
	}
	StringBuilderAppendLine(&builder, "}");
	
	return TakeString(&builder);
}

MyStr_t PigGenGenerateFuncTableCountCode(MemArena_t* memArena, VarArray_t* registeredFunctions)
{
	NotNull(registeredFunctions);
	StringBuilder_t builder;
	NewStringBuilder(&builder, memArena);
	builder.newLineStyle = PIGGEN_NEW_LINE;
	
	StringBuilderAppendPrintLine(&builder, "#define FuncTable_NumFunctions %llu", MaxU64(1, registeredFunctions->length));
	
	return TakeString(&builder);
}

