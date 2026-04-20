#!/usr/bin/env python
import os
import shutil

# Create directories if they don't exist
os.makedirs('src', exist_ok=True)
os.makedirs('tests', exist_ok=True)
os.makedirs('unity', exist_ok=True)

# Move files to their proper locations
if os.path.exists('token.h'):
    shutil.move('token.h', 'src/token.h')
    print("Moved token.h -> src/token.h")

if os.path.exists('test_lexer.c'):
    shutil.move('test_lexer.c', 'tests/test_lexer.c')
    print("Moved test_lexer.c -> tests/test_lexer.c")

print("\nProject structure created successfully!")
print("Directory structure:")
for root, dirs, files in os.walk('.'):
    level = root.replace('.', '').count(os.sep)
    indent = ' ' * 2 * level
    print(f'{indent}{os.path.basename(root)}/')
    subindent = ' ' * 2 * (level + 1)
    for file in files[:10]:  # Limit to first 10 files per directory
        if not file.endswith(('.pyc', '.bat', '.sh')):
            print(f'{subindent}{file}')
    if len(files) > 10:
        print(f'{subindent}... and {len(files) - 10} more files')

