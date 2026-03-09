import requests
import os

# ---------------- Configuration ----------------
BASE_URL = "http://localhost:8080"  # Adresse de ton serveur Webserv

# Tests CGI
CGI_SCRIPTS = ["/cgi-bin/test_cgi.py", "/cgi-bin/hello.py"]

# Tests fichiers statiques
STATIC_FILES = ["/index.html", "/images/logo.png"]

# Tests dossiers (autoindex)
DIRECTORIES = ["/", "/documents/"]

# Tests DELETE (optionnel)
DELETE_PATHS = ["/cgi-bin/delete_test.py"]

# Données de test POST
POST_DATA = {"name": "Jean", "age": "25"}

# ---------------- Fonctions de test ----------------
def send_request(method, path, data=None):
    url = BASE_URL + path
    try:
        if method == "GET":
            r = requests.get(url, params=data)
        elif method == "POST":
            r = requests.post(url, data=data)
        elif method == "DELETE":
            r = requests.delete(url)
        else:
            return (None, f"Method {method} not supported")
        return (r, None)
    except Exception as e:
        return (None, str(e))

def print_response(title, response, error=None):
    print(f"\n=== {title} ===")
    if error:
        print("Erreur :", error)
        return
    print("Status :", response.status_code)
    print("Headers:", dict(response.headers))
    print("Body:\n", response.text[:500] + ("..." if len(response.text) > 500 else ""))  # Limite affichage

# ---------------- Tests ----------------
def test_cgi():
    for script in CGI_SCRIPTS:
        r, e = send_request("GET", script)
        print_response(f"CGI GET {script}", r, e)
        r, e = send_request("POST", script, data=POST_DATA)
        print_response(f"CGI POST {script}", r, e)

def test_static_files():
    for file in STATIC_FILES:
        r, e = send_request("GET", file)
        print_response(f"Static GET {file}", r, e)

def test_directories():
    for directory in DIRECTORIES:
        r, e = send_request("GET", directory)
        print_response(f"Directory GET {directory}", r, e)

def test_delete():
    for path in DELETE_PATHS:
        r, e = send_request("DELETE", path)
        print_response(f"DELETE {path}", r, e)

# ---------------- Résumé global ----------------
def main():
    print("=== TEST COMPLET WEB SERV ===")
    print("\n--- CGI Tests ---")
    test_cgi()
    
    print("\n--- Static Files Tests ---")
    test_static_files()
    
    print("\n--- Directory Tests (Autoindex) ---")
    test_directories()
    
    print("\n--- DELETE Tests ---")
    test_delete()
    
    print("\n=== FIN DES TESTS ===")

if __name__ == "__main__":
    main()
