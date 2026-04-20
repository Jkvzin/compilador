#!/bin/bash
# Task 8 - Statement Parsing: Commit script

cd /c/Users/João\ Victor/Documents/Estudos\ progeamativos/compiladores/compilador_cop

# Check git status
echo "Git status:"
git status

echo ""
echo "Staging changes..."

# Stage all modified files
git add src/token.h src/token.c src/lexer.c src/parser.h src/parser.c tests/test_parser.c

echo ""
echo "Committing Task 8 - Statement Parsing"
git commit -m "Task 8: Implement Statement Parsing (assignments, I/O, control flow)

- Add TOK_ESCREVERLN token type for writeln statements
- Implement parser_parse_read() for 'leia var' statements
- Implement parser_parse_write() for 'escreva expr' statements
- Implement parser_parse_writeln() for 'escreverln expr' statements
- Implement parser_parse_assignment() for 'var <- expr' statements
- Implement parser_parse_if_statement() for if/else control flow
- Implement parser_parse_while_statement() for while loops
- Implement parser_parse_for_statement() for for loops
- Update parser_parse_commands() to recognize all statement terminators
- Update parser_parse_command() to dispatch to statement handlers
- Add 9 comprehensive test functions with 20+ assertions
- All statement types now parse correctly with proper error handling
- Support nested statements (if inside while, etc.)

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"

echo ""
echo "Done!"
