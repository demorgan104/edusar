#!/usr/bin/env python3
"""Local UI and C-demo bridge. Run via make -C build ui."""
import argparse
from functools import partial
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parent
BINARY = ROOT.parent / 'build/out/bin/demo'

class Handler(SimpleHTTPRequestHandler):
    def do_POST(self):
        if self.path != '/api/transmit':
            self.send_error(404)
            return
        try:
            size = int(self.headers.get('Content-Length', '0'))
            if not 0 < size <= 256:
                raise ValueError('invalid request size')
            value = json.loads(self.rfile.read(size)).get('value')
            if type(value) is not int or not 0 <= value <= 65535:
                raise ValueError('value must be an integer from 0 to 65535')
            result = subprocess.run([str(BINARY), '--json', str(value)],
                                    capture_output=True, text=True, check=True, timeout=5)
            payload = json.loads(result.stdout)
            status = 200
        except (ValueError, AttributeError) as exc:
            payload, status = {'error': str(exc)}, 400
        except (OSError, subprocess.SubprocessError):
            payload, status = {'error': 'C demo failed. Build with the default demo configuration.'}, 500
        body = json.dumps(payload).encode()
        self.send_response(status)
        self.send_header('Content-Type', 'application/json')
        self.send_header('Cache-Control', 'no-store')
        self.send_header('Content-Length', str(len(body)))
        self.end_headers()
        self.wfile.write(body)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', type=int, default=8000)
    args = parser.parse_args()
    server = ThreadingHTTPServer(('127.0.0.1', args.port), partial(Handler, directory=str(ROOT / 'ui')))
    print(f'Demo UI: http://127.0.0.1:{server.server_port} (Ctrl+C to stop)', flush=True)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()
