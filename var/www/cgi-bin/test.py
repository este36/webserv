#!/usr/bin/env python3
import sys

# Imprimimos el header Content-Type y la línea en blanco obligatoria (\r\n\r\n)
sys.stdout.write("Content-Type: text/html\r\n\r\n")
# Imprimimos el body (el HTML que verá el navegador)
sys.stdout.write("<html><body><h1>¡Hola desde Python CGI!</h1></body></html>\n")