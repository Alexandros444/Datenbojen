#!/usr/bin/env python3
"""
License: MIT License
Copyright (c) 2023 Miel Donkers
Very simple HTTP server in python for logging requests
Usage::
    ./server.py [<port>]
"""
from http.server import BaseHTTPRequestHandler, HTTPServer
import logging
import json
import csv
import os
import datetime

class S(BaseHTTPRequestHandler):
    def _set_response(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        logging.info("GET request,\nPath: %s\nHeaders:\n%s\n", str(self.path), str(self.headers))
        if self.path == "/health":
            self._set_response()
            self.wfile.write(b"OK")
            return
        self._set_response()
        data = {
            "Data Rate / day" : 10
        }
        self.wfile.write(json.dumps(data).encode('utf-8'))

    def do_POST(self):
        try:
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            logging.info("POST request,\nPath: %s\nHeaders:\n%s\n\nBody:\n%s\n",
                    str(self.path), str(self.headers), post_data.decode('utf-8'))
            logging.getLogger().info("Received data: %s", post_data.decode('utf-8'))

            # Write JSON data to CSV file
            try:
                data_dict = json.loads(post_data.decode('utf-8'))
                csv_file = 'received_data.csv'
                write_header = not os.path.exists(csv_file)
                timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
                row = {'timestamp': timestamp}
                row.update(data_dict)
                fieldnames = ['timestamp'] + list(data_dict.keys())
                with open(csv_file, 'a', newline='') as f:
                    writer = csv.DictWriter(f, fieldnames=fieldnames)
                    if write_header:
                        writer.writeheader()
                    writer.writerow(row)
            except Exception as e:
                logging.getLogger().error(f"Failed to write CSV: {e}")

            self._set_response()
            self.wfile.write("POST request for {}".format(self.path).encode('utf-8'))
        except Exception as e:
            logging.getLogger().error(f"Exception in do_POST: {e}")
            self.send_response(500)
            self.end_headers()
            self.wfile.write(b"Internal Server Error")

    def log_message(self, format, *args):
        # Print only the standard HTTP server log line
        print("%s - - [%s] %s" %
              (self.client_address[0],
               self.log_date_time_string(),
               format % args))

def run(server_class=HTTPServer, handler_class=S, port=8080):
    # Remove logging.basicConfig and set up only file handler
    logger = logging.getLogger()
    logger.handlers = []  # Remove any existing handlers
    file_handler = logging.FileHandler('received_data.log')
    file_handler.setLevel(logging.INFO)
    file_handler.setFormatter(logging.Formatter('%(asctime)s %(message)s'))
    logger.addHandler(file_handler)
    logger.setLevel(logging.INFO)
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    logging.info(f'Starting httpd on {server_address=}\n')
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    except Exception as e:
        print(f"Server crashed: {e}")
        logging.getLogger().error(f"Server crashed: {e}")
    httpd.server_close()
    logging.info('Stopping httpd...\n')

if __name__ == '__main__':
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()