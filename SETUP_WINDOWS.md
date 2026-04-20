# Windows Setup Guide for SIMPLES Compiler

## Problem: Make Not Installed ❌

You're using MinGW64 bash but `make` is not installed.

## Solution 

### Option 1: Install Make via MinGW (Recommended)

**Step 1: Install MinGW with Make**

Go to: https://www.mingw-w64.org/

Or if you already have MinGW, use the installer:
```bash
mingw-get install mingw32-make
```

**Step 2: Rename or Create Alias**

MinGW64 installs it as `mingw32-make`. Either:

**Option A: Use directly**
```bash
mingw32-make clean test
```

**Option B: Create alias** (add to ~/.bashrc)
```bash
alias make=mingw32-make
```

Then reload:
```bash
source ~/.bashrc
```

**Option C: Rename** (one-time)
```bash
cp /mingw64/bin/mingw32-make.exe /mingw64/bin/make.exe
```

---

### Option 2: Quick Alternative - Manual Compile

If you can't install make, compile manually:

```bash
# Compile token tests
gcc -Wall -Wextra -std=c99 -g -o test_lexer tests/test_lexer.c src/lexer.c -I src/

# Run tests
./test_lexer
```

---

## Verify Installation

After installing make, verify:

```bash
make --version
gcc --version
```

Should show versions for both.

---

## Then Run Setup

Once make is working:

```bash
python FINAL_SETUP.py
make clean test
```

---

## Windows Command Alternative

If bash is giving you trouble, use **Windows Command Prompt** instead:

```cmd
cd C:\Users\João Victor\Documents\Estudos progeamativos\compiladores\compilador_cop
python FINAL_SETUP.py
mingw32-make clean test
```

---

## Need Help?

1. **Check if MinGW is installed:**
   ```bash
   gcc --version
   ```

2. **Check if make exists:**
   ```bash
   mingw32-make --version
   ```
   or
   ```bash
   which mingw32-make
   ```

3. **If not found, install via MSYS2:**
   - Download: https://www.msys2.org/
   - Then in MSYS2: `pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make`

---

## Quick Test

Once set up, verify everything works:

```bash
make --version      # Should show GNU Make
gcc --version       # Should show GCC
python --version    # Should show Python 3
```

All three commands should succeed.

---

## Next Steps

1. Fix the make issue using one of the options above
2. Then run: `python FINAL_SETUP.py`
3. Then run: `make clean test`
4. Then read: `GETTING_STARTED.md` (use `cat` or open in editor)

---

Good luck! Let me know which option works for you.
