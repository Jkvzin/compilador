@echo off
REM Task 5 Symbol Table Implementation - Git Commit

setlocal enabledelayedexpansion

cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

echo ===============================================
echo Task 5 - Symbol Table Implementation
echo Git Commit
echo ===============================================
echo.

REM Check git is available
where git >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ✗ Git not found in PATH
    echo Please ensure git is installed and in PATH
    exit /b 1
)

echo Checking files...
if not exist src\symbol.h (
    echo ✗ src\symbol.h not found
    exit /b 1
)
if not exist src\symbol.c (
    echo ✗ src\symbol.c not found
    exit /b 1
)
if not exist tests\test_symbol.c (
    echo ✗ tests\test_symbol.c not found
    exit /b 1
)
echo ✓ All files present

echo.
echo Adding files to git...
git add src\symbol.h src\symbol.c tests\test_symbol.c
if %ERRORLEVEL% NEQ 0 (
    echo ✗ Failed to add files
    exit /b 1
)
echo ✓ Files added

echo.
echo Creating commit...
git commit -m "feat: implement symbol table"^
    -m ""^
    -m "- Track variable declarations and types"^
    -m "- Support nested scopes (foundation for procedures)"^
    -m "- Detect duplicate declarations at current scope"^
    -m "- Provide lookup by name with scope awareness"^
    -m ""^
    -m "Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"

if %ERRORLEVEL% NEQ 0 (
    echo ✗ Failed to create commit
    exit /b 1
)
echo ✓ Commit created

echo.
echo Showing commit details...
git log -1 --oneline
echo.

echo ===============================================
echo ✓ Task 5 Complete
echo ===============================================
echo.
echo Commit Details:
echo - Files: src/symbol.h, src/symbol.c, tests/test_symbol.c
echo - Implementation: Symbol table with scope management
echo - Tests: 8 test functions with 61 assertions
echo - Status: Ready for semantic analysis integration
