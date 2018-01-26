if "%PLATFORM%" == "x64" GOTO x64

echo 32bit Platform
C:\Python27\python -m pip install pytest
C:\Python27\python -m pytest -v -s "%~dp0/test_cnvme.py" || GOTO FAIL

GOTO END

:X64

echo 64bit Platform
C:\Python27-x64\python -m pip install pytest
C:\Python27-x64\python -m pytest -v -s "%~dp0/test_cnvme.py" || GOTO FAIL

GOTO END

FAIL:
exit /b 1

:END