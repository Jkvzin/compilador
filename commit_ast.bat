@echo off
setlocal enabledelayedexpansion

cd /d "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

echo Checking git status...
git status

echo.
echo Staging files...
git add src\ast.h src\ast.c tests\test_ast.c

echo.
echo Creating commit...
git commit -m "feat: implement AST node structures

- Create 16 AST node types for all language constructs
- Implement factory functions for each node type
- Add utility functions (add_child, free, print)
- Create basic AST tests

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"

echo.
echo Commit complete!
git log --oneline -n 3
