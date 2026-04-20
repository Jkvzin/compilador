#!/bin/bash
# Test the build

echo "🔨 Building SIMPLES Lexer..."
echo ""

python build.py test

BUILD_RESULT=$?

echo ""
echo "=================================================="
if [ $BUILD_RESULT -eq 0 ]; then
    echo "✅ BUILD SUCCESSFUL!"
    echo "=================================================="
    echo ""
    echo "Next steps:"
    echo "1. Review the lexer implementation"
    echo "2. Run git add and commit"
    echo "3. Start Task 3 (expand tests)"
    echo ""
else
    echo "❌ BUILD FAILED"
    echo "=================================================="
    echo ""
    echo "Check the errors above and fix them."
    echo ""
fi

exit $BUILD_RESULT
