#!/usr/bin/env python3

import os
import json

script_dir = os.path.dirname(os.path.abspath(__file__))
uploads_dir = os.path.join(script_dir, "..", "uploads")

try:
    files = []
    if os.path.exists(uploads_dir):
        for item in os.listdir(uploads_dir):
            item_path = os.path.join(uploads_dir, item)
            if os.path.isfile(item_path):
                size = os.path.getsize(item_path)
                files.append({
                    "name": item,
                    "size": size
                })
    
    print(json.dumps({"success": True, "files": files}))
except Exception as e:
    print(json.dumps({"success": False, "error": str(e)}))
