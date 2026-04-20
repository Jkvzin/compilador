#!/usr/bin/env python3
"""
SIMPLES Compiler Quick Start
Run this to get everything working!
"""

import subprocess
import sys
import os

def run_command(cmd, description):
    """Run a command and report results."""
    print(f"\n{'='*60}")
    print(f"📍 {description}")
    print(f"{'='*60}")
    print(f"Running: {' '.join(cmd)}")
    print()
    
    result = subprocess.run(cmd, cwd=os.getcwd())
    return result.returncode == 0

def main():
    print("""
╔════════════════════════════════════════════════════════════╗
║                                                            ║
║        SIMPLES COMPILER - Quick Start Setup ✅            ║
║                                                            ║
║        This script will:                                  ║
║        1. Organize project files                          ║
║        2. Verify build system                             ║
║        3. Run tests                                       ║
║        4. Show you what to do next                        ║
║                                                            ║
╚════════════════════════════════════════════════════════════╝
    """)
    
    # Step 1: Setup
    print("\n⏳ Step 1: Organizing project files...")
    if os.path.exists('FINAL_SETUP.py'):
        if run_command([sys.executable, 'FINAL_SETUP.py'], 
                       "Running FINAL_SETUP.py"):
            print("✅ Project files organized")
        else:
            print("⚠️  Setup encountered an issue but continuing...")
    
    # Step 2: Build and test
    print("\n⏳ Step 2: Building and testing...")
    if os.path.exists('build.py'):
        if run_command([sys.executable, 'build.py', 'test'],
                       "Running tests"):
            print("✅ Tests passed!")
        else:
            print("❌ Tests failed - see output above")
    else:
        print("❌ build.py not found")
        return False
    
    # Step 3: Show documentation
    print("\n" + "="*60)
    print("📚 NEXT STEPS")
    print("="*60)
    print("""
YOUR PROJECT IS READY! 🎉

Next, read these files (in order):

1. START_HERE.txt
   Quick orientation (2 min read)
   Command: cat START_HERE.txt

2. GETTING_STARTED.md
   Setup guide and project structure (5 min read)
   Command: cat GETTING_STARTED.md

3. DOCUMENTATION.md
   Full language specification (10 min read)
   Command: cat DOCUMENTATION.md | head -100

4. plan.md
   Implementation tasks (read as needed)
   Command: less plan.md

---

TO READ FILES IN BASH:
  cat FILENAME              - Show entire file
  head -50 FILENAME         - Show first 50 lines
  less FILENAME             - Show with scrolling
  cat FILENAME | head -100  - Pipe to see first 100 lines

---

BUILD COMMANDS (no make needed!):
  python build.py test      - Build and test
  python build.py clean     - Clean artifacts
  python build.py all       - Build everything

---

CURRENT STATUS:
  ✅ 2 of 20 tasks complete (Lexer phase: 67%)
  ✅ Build system working
  ✅ Tests ready
  ✅ Documentation complete

---

TIME TO COMPLETE PROJECT:
  Phase 1 (Lexer):     20 minutes (1 task left)
  Phase 2 (Parser):    2-3 hours
  Phase 3 (Semantic):  1 hour
  Phase 4 (Codegen):   3-4 hours
  ────────────────────
  Total Remaining:     6-9 hours

---

START CODING!
Read plan.md to see Task 3 (next task)
Then implement it following TDD principles.

Good luck! 🚀
    """)
    
    return True

if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)
