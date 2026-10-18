"""Integration check for the local UI server; requires localhost socket access."""
import importlib.util
import json
from pathlib import Path
import threading
import urllib.request
import urllib.error
from functools import partial
from http.server import ThreadingHTTPServer

path = Path(__file__).resolve().parents[1] / 'demo/serve.py'
spec = importlib.util.spec_from_file_location('demo_server', path)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)
server = ThreadingHTTPServer(('127.0.0.1', 0), partial(module.Handler, directory=str(module.ROOT / 'ui')))
threading.Thread(target=server.serve_forever, daemon=True).start()
base = 'http://127.0.0.1:' + str(server.server_port)
try:
    for asset in ('/', '/style.css', '/app.js'):
        with urllib.request.urlopen(base + asset) as response:
            assert response.status == 200
    for value in (0, 1234, 65535):
        request = urllib.request.Request(base + '/api/transmit', data=json.dumps({'value': value}).encode())
        with urllib.request.urlopen(request) as response:
            result = json.load(response)
            assert result == {'sent': value, 'received': value, 'canId': 291,
                              'bytes': [value & 255, value >> 8], 'confirmations': 1}, result
    for value in (-1, 65536, True, 'bad'):
        request = urllib.request.Request(base + '/api/transmit', data=json.dumps({'value': value}).encode())
        try:
            urllib.request.urlopen(request)
            raise AssertionError('accepted invalid input')
        except urllib.error.HTTPError as error:
            assert error.code == 400
    print('UI assets, live C bridge, boundary values, and invalid-input checks passed')
finally:
    server.shutdown()
    server.server_close()
