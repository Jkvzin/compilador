#!/bin/bash
# Run tests with better output

echo "════════════════════════════════════════════════════"
echo "🧪 Building and Testing SIMPLES Lexer"
echo "════════════════════════════════════════════════════"
echo ""

python build.py test

EXIT_CODE=$?

echo ""
echo "════════════════════════════════════════════════════"
if [ $EXIT_CODE -eq 0 ]; then
    echo "✅ ALL TESTS PASSED!"
    echo "════════════════════════════════════════════════════"
    echo ""
    echo "Next steps:"
    echo "1. Review your code:"
    echo "   cat src/lexer.c | head -50"
    echo ""
    echo "2. Commit your work:"
    echo "   git add src/ tests/"
    echo "   git commit -m \"feat: complete lexer implementation with tests\""
    echo ""
    echo "3. See what comes next:"
    echo "   head -100 plan.md"
    echo ""
else
    echo "❌ TESTS FAILED"
    echo "════════════════════════════════════════════════════"
    echo ""
    echo "Review the output above for test failures."
    echo "Common issues:"
    echo "- Token not recognized"
    echo "- Wrong token count"
    echo "- Integer value not parsed correctly"
    echo ""
fi

exit $EXIT_CODE
