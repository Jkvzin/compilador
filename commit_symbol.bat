@echo off
cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

echo Adding files to git...
git add src\symbol.h src\symbol.c tests\test_symbol.c

echo Committing...
git commit -m "feat: implement symbol table

- Track variable declarations and types
- Support nested scopes (foundation for procedures)
- Detect duplicate declarations at current scope
- Provide lookup by name with scope awareness

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ✓ Commit successful
) else (
    echo.
    echo ✗ Commit failed
)
