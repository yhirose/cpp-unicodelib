#!/usr/bin/env python3
"""
Update unicodelib.h with UCD.

This script processes unicodelib.h and updates all generated blocks
by executing the commands specified in COMMAND: lines.
"""

import re
import subprocess
import sys
import os


def find_generated_blocks(content):
    """
    Find all generated blocks in the content.
    Returns a list of tuples: (command, start_pos, end_pos)
    where start_pos is the position after BEGIN marker
    and end_pos is the position before END marker.
    """
    blocks = []
    
    # Pattern to match COMMAND line followed by BEGIN/END markers
    command_pattern = re.compile(r'// COMMAND: `([^`]+)`')
    begin_marker = '// -------- [BEGIN GENERATED BLOCK] --------'
    end_marker = '// -------- [END GENERATED BLOCK] --------'
    
    pos = 0
    while True:
        # Find the next COMMAND line
        match = command_pattern.search(content, pos)
        if not match:
            break
        
        command = match.group(1)
        command_end = match.end()
        
        # Find the BEGIN marker after the command
        begin_pos = content.find(begin_marker, command_end)
        if begin_pos == -1:
            print(f"Warning: BEGIN marker not found after command: {command}", file=sys.stderr)
            pos = command_end
            continue
        
        # The content starts after the BEGIN marker line (including newline)
        content_start = content.find('\n', begin_pos) + 1
        
        # Find the END marker
        end_pos = content.find(end_marker, content_start)
        if end_pos == -1:
            print(f"Warning: END marker not found after command: {command}", file=sys.stderr)
            pos = content_start
            continue
        
        blocks.append((command, content_start, end_pos))
        pos = end_pos + len(end_marker)
    
    return blocks


def execute_command(command, working_dir):
    """
    Execute a shell command and return its stdout.
    """
    try:
        result = subprocess.run(
            command,
            shell=True,
            cwd=working_dir,
            capture_output=True,
            text=True,
            check=True
        )
        return result.stdout
    except subprocess.CalledProcessError as e:
        print(f"Error executing command: {command}", file=sys.stderr)
        print(f"stderr: {e.stderr}", file=sys.stderr)
        raise


def update_file(filepath):
    """
    Update the generated blocks in the specified file.
    """
    # Get the project root directory (parent of scripts directory)
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    
    # Read the file content
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Find all generated blocks
    blocks = find_generated_blocks(content)
    
    if not blocks:
        print("No generated blocks found.")
        return
    
    print(f"Found {len(blocks)} generated block(s).")
    
    # Process blocks in reverse order to preserve positions
    for command, start_pos, end_pos in reversed(blocks):
        print(f"Processing: {command}")
        
        # Execute the command
        new_content = execute_command(command, project_root)
        
        # Replace the content
        content = content[:start_pos] + new_content + content[end_pos:]
    
    # Write the updated content back
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
    
    print("Update complete.")


def main():
    # Default to unicodelib.h in the project root
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
    else:
        filepath = os.path.join(project_root, 'unicodelib.h')
    
    if not os.path.exists(filepath):
        print(f"Error: File not found: {filepath}", file=sys.stderr)
        sys.exit(1)
    
    update_file(filepath)


if __name__ == '__main__':
    main()
