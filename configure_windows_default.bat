@ECHO off
SETLOCAL EnableDelayedExpansion

SET build_conf=%1

WHERE python
IF %ERRORLEVEL% NEQ 0 ( 
	ECHO "python could not be found"
	EXIT /b %ERRORLEVEL%
)

IF NOT "%build_conf%"=="Debug" IF NOT "%build_conf%"=="Release" IF NOT "%build_conf%"=="RelWithDebInfo" (
	SET build_conf=Debug
	ECHO "Using default build type: !build_conf!"
)

ECHO %build_conf%

MKDIR build

python -m venv build_env
CALL ./build_env/Scripts/activate.bat

pip3 install conan
conan profile detect

conan install . -of="build/"%build_conf% -s build_type=%build_conf% --build=missing