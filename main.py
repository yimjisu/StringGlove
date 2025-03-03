import mimetypes
mimetypes.add_type('application/javascript', '.js')
mimetypes.add_type('text/css', '.css')

from flask import Flask, render_template
from flask_socketio import SocketIO
import logging
import os, json , math
from serials import Serial


log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)



CODEMIRROR_LANGUAGES = ['python', 'yaml', 'htmlembedded']
WTF_CSRF_ENABLED = True
SECRET_KEY = 'secret'
# optional
CODEMIRROR_THEME = '3024-day'
CODEMIRROR_ADDONS = (
        ('ADDON_DIR','ADDON_NAME'),
)
app = Flask(__name__)
app.config.from_object(__name__)

socketio = SocketIO(app)

@app.route("/")
def index():
    return render_template('index.html')

if __name__ == "__main__":
    ser = Serial()
    
    @socketio.on("load")
    def load():
        socketio.emit("ports", ser.get_ports())

    @socketio.on("port")
    def port(isOpen):
        ser.open(isOpen)
        print("Connected", isOpen)

    MAX = 80
    @socketio.on("xy")
    def xy(x, y): # x, y are in range -1 to 1
        print("x, y", x, y)
        a = (1 + 2 * y) / 3
        b = (1 - y + math.sqrt(3) * x) / 3
        c = (1 - y - math.sqrt(3) * x) / 3

        a = min(1, max(0, a))
        b = min(1, max(0, b))
        c = min(1, max(0, c))

        a = int(a * MAX)
        b = int(b * MAX)
        c = int(c * MAX)

        print("a, b, c", a, b, c)
        ser.write( c, a, b)
    
    @socketio.on("z")
    def z(z):
        ser.write(z, z, z)
    
    @socketio.on("individual")
    def individual(pos, value):
        ser.offset(pos, value)
        

    @socketio.on("move")
    def individual(type, value, min, max):
        ser.move(type, value, min, max)

    socketio.run(app, debug=True)