#!/bin/bash
# Task 5 Symbol Table Implementation - Git Commit Script

# Navigate to project directory
cd "C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop"

# Add the three new files
git add src/symbol.h src/symbol.c tests/test_symbol.c

# Create the commit with the specified message
git commit -m "feat: implement symbol table

- Track variable declarations and types
- Support nested scopes (foundation for procedures)
- Detect duplicate declarations at current scope
- Provide lookup by name with scope awareness

Co-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>"

# Verify the commit
git log -1 --oneline

# Show the commit details
git show --stat

echo "✓ Commit complete!"
