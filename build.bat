@echo off

set ExeBaseName=PigGen
set ExeName=%ExeBaseName%.exe

set DebugBuild=0
set AssertionsEnabled=1

set CopyToBuildDirectory=1

set SourceDirectory=..
set LibDirectory=..\..
set ProjectDirectory=..\..\..\..
set BuildDirectory=%ProjectDirectory%\build
set GeneratedCodeDirectory=%BuildDirectory%\gen
set MainCodePath=%SourceDirectory%\main.cpp
set VersionFilePath=%SourceDirectory%\version.h
set IncVersNumScriptPath=%SourceDirectory%\IncrementVersionNumber.py

echo Running on %ComputerName%

python --version 2>NUL
if errorlevel 1 (
	echo WARNING: Python isn't installed on this computer. The build number will not be incremented
	set PythonInstalled=0
) else (
	set PythonInstalled=1
)

rem call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
rem call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 -no_logo
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64

set CompilerFlags=-DWINDOWS_COMPILATION -DDEBUG_BUILD=%DebugBuild% -DASSERTIONS_ENABLED=%AssertionsEnabled%
rem /FC = Full path for error messages
rem /EHsc = Exception Handling Model: Standard C++ Stack Unwinding. Functions declared as extern "C" can't throw exceptions
rem /EHa- = TODO: Do we really need this?? It seems like this option should be off if we specify s and c earlier
rem /nologo = Suppress the startup banner
rem /GS- = Buffer overrun protection is turned off
rem /Gm- = Minimal rebuild is enabled [deprecated]
rem /GR- = Run-Time Type Information is Disabled [_CPPRTTI macro doesn't work]
rem /Oi = Enable intrinsic generation
rem /WX = Treat warnings as errors
rem /W4 = Warning level 4 [just below /Wall]
set CompilerFlags=%CompilerFlags% /FC /EHsc /EHa- /nologo /GS- /Gm- /GR- /Oi /WX /W4
rem /wd4130 = Logical operation on address of string constant [W4] TODO: Should we re-enable this one? Don't know any scenarios where I want to do this
rem /wd4201 = Nonstandard extension used: nameless struct/union [W4] TODO: Should we re-enable this restriction for ANSI compatibility?
rem /wd4324 = Structure was padded due to __declspec[align[]] [W4]
rem /wd4458 = Declaration of 'identifier' hides class member [W4]
rem /wd4505 = Unreferenced local function has been removed [W4]
rem /wd4996 = Usage of deprecated function, class member, variable, or typedef [W3]
rem /wd4127 = Conditional expression is constant [W4]
rem /wd4706 = assignment within conditional expression [W?]
set CompilerFlags=%CompilerFlags% /wd4130 /wd4201 /wd4324 /wd4458 /wd4505 /wd4996 /wd4127 /wd4706
set LinkerFlags=-incremental:no
rem TODO: Do we really need all of these? Maybe go through and document what functions we use from them?
rem gdi32.lib    = ?
rem User32.lib   = ?
rem Shell32.lib  = Shlobj.h ? 
rem kernel32.lib = ?
rem winmm.lib    = ?
rem Winhttp.lib  = ?
rem Shlwapi.lib  = ?
rem Ole32.lib    = Combaseapi.h, CoCreateInstance
set Libraries=User32.lib Shell32.lib kernel32.lib
set IncludeDirectories=/I"%LibDirectory%"

if "%DebugBuild%"=="1" (
	rem /Od = Optimization level: Debug
	rem /Zi = Generate complete debugging information
	rem /MTd = Statically link the standard library [not as a DLL, Debug version]
	rem /wd4065 = Switch statement contains 'default' but no 'case' labels
	rem /wd4100 = Unreferenced formal parameter [W4]
	rem /wd4101 = Unreferenced local variable [W3]
	rem /wd4127 = Conditional expression is constant [W4]
	rem /wd4189 = Local variable is initialized but not referenced [W4]
	rem /wd4702 = Unreachable code [W4]
	set CompilerFlags=%CompilerFlags% /Od /Zi /MTd /wd4065 /wd4100 /wd4101 /wd4127 /wd4189 /wd4702
	rem Dbghelp.lib = ?
	set Libraries=%Libraries% Dbghelp.lib
) else (
	rem /Ot = Favors fast code over small code
	rem /Oy = Omit frame pointer [x86 only]
	rem /O2 = Optimization level 2: Creates fast code
	rem /MT = Statically link the standard library [not as a DLL]
	rem /Zi = Generate complete debugging information [optional]
	rem /fsanitize=address = Enable Address Sanitizer [optional]
	set CompilerFlags=%CompilerFlags% /Ot /Oy /O2 /MT
	rem set Libraries=%Libraries%
)

set OutputFolder=release
if "%DebugBuild%"=="1" (
	set OutputFolder=debug
)
mkdir %OutputFolder% 2> NUL > NUL
cd %OutputFolder%

echo[

if "%PythonInstalled%"=="1" (
	python %IncVersNumScriptPath% %VersionFilePath%
)

cl /Fe%ExeName% %CompilerFlags% %IncludeDirectories% "%MainCodePath%" /link %LinkerFlags% %Libraries%

if "%CopyToBuildDirectory%"=="1" (
	echo [Copying %ExeName% to project build directory]
	XCOPY ".\%ExeName%" "%BuildDirectory%\" /Y > NUL
	if "%DebugBuild%"=="1" (
		XCOPY ".\%ExeBaseName%.pdb" "%BuildDirectory%\" /Y > NUL
	)
) else (
	echo [PigGen Build Finished!]
)

