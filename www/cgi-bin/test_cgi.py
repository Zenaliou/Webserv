#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/html")
print()

# Récupérer les variables d'environnement CGI
request_method = os.environ.get('REQUEST_METHOD', 'UNKNOWN')
content_length = os.environ.get('CONTENT_LENGTH', '0')
query_string = os.environ.get('QUERY_STRING', '')
remote_addr = os.environ.get('REMOTE_ADDR', 'unknown')

print("<h1>Test CGI POST/GET</h1>")
print("<pre>")
print(f"Méthode HTTP: {request_method}")
print(f"Remote Address: {remote_addr}")
print(f"Query String: {query_string}")
print(f"Content-Length: {content_length}")

# Lire les données POST si présentes
if request_method == 'POST' and int(content_length) > 0:
    post_data = sys.stdin.read(int(content_length))
    print(f"\nDonnées POST reçues:\n{post_data}")

print("</pre>")
print("<p><strong>✓ CGI fonctionne!</strong></p>")
