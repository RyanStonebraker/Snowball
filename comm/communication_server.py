import http.server


class ServerMain(http.server.SimpleHTTPRequestHandler):
    def do_HEAD(self):
        self.send_response(200)
        self.send_header('Allow', 'GET, POST, OPTIONS')
        self.send_header("Content-Type", "text/html")
        self.send_header("charset", "utf-8")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Headers", "*")
        self.send_header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE')
        self.end_headers()

    def do_GET(self):
        self.send_response(200)
        self.send_header('Allow', 'GET, POST, OPTIONS')
        self.send_header('')
        self.send_header("Content-Type", "text/html")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Headers", "*")
        self.send_header('Access-Control-Allow-Methods', 'GET,PUT,POST,DELETE')
        self.end_headers()

        if self.path == "/boardstate":
            with open("boardstate.txt", "r") as board:
                out = """<DOCTYPE html><html><head><meta http-equiv="Content-Type" content="text/html; charset=utf-8"></head>""" + board.readline() + """</html>"""
                self.wfile.write(out.encode('utf-8'))
        if "?" in self.path:
            post_board = str(self.path[self.path.find("?") + 1:])
            if len(post_board) >= 32:
                with open("boardstate.txt", "w") as wOut:
                    wOut.write(post_board)
                    self.wfile.write(post_board.encode('utf-8'))


if __name__ == '__main__':
    server_class = http.server.HTTPServer
    httpd = server_class(("localhost", 3000), ServerMain)
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    httpd.server_close()
