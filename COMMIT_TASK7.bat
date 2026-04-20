@echo off
REM Git commit for Task 7: Expression Parsing

cd /d "%~dp0"

echo ========================================
echo Task 7: Expression Parsing
echo Git Commit Script
echo ========================================

REM Stage all changes
echo.
echo Staging changes...
git add -A

REM Show what will be committed
echo.
echo Changes to be committed:
git status

REM Commit with message
echo.
echo Creating commit...
git commit -m "Task 7: Implement expression parsing with operator precedence

- Add parser_parse_expression() entry point for expression parsing
- Implement parser_parse_comparison() for comparison operators (=, <>, <, >, <=, >=)
- Implement parser_parse_term() for addition/subtraction operators (+, -)
- Implement parser_parse_factor() for multiplication/division operators (*, /)
- Implement parser_parse_unary() for unary operators (-, nao)
- Implement parser_parse_primary() for primary expressions (numbers, identifiers, parentheses)
- Update parser_parse_command() to handle assignment statements (var <- expression;)
- Add 11 comprehensive test functions with 80+ assertions
- Verify operator precedence and left-associativity through AST structure
- Implement left-associative operator parsing
- Support parenthesized expressions
- All tests passing, memory-safe, no compiler warnings

Implementation details:
- Recursive descent parser with proper precedence climbing
- Operator mapping: TOK_* to AST operator characters
- Multi-char operators encoded: ^<^> ^-> '!', ^<^= ^-> '[', ^>^= ^-> ']'
- Error messages include line/column information
- Complete memory management with proper allocation/deallocation

Implements Task 7 of SIMPLES compiler specification.

Co-authored-by: Copilot ^<223556219+Copilot@users.noreply.github.com^>"

echo.
echo ========================================
echo Commit complete!
echo ========================================
