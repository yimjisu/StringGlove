import mimetypes
mimetypes.add_type('application/javascript', '.js')
mimetypes.add_type('text/css', '.css')

from flask import Flask, render_template
from flask_socketio import SocketIO
import logging
import os, json
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

    @socketio.on("xy")
    def xy(x, y):
        print(x, y)
        offset = 50
        a = (2 * y) / 3
        b = -x - y/3
        c = +x + y/3  
        ser.write( b+offset, a+offset, c+offset)
    
    @socketio.on("z")
    def z(z):
        ser.write(z, z, z)
    
    @socketio.on("individual")
    def individual(pos, value):
        if pos == 1:
            ser.write( value, 0, 0)
        elif pos == 2:
            ser.write( 0, value, 0)
        elif pos == 3:
            ser.write( 0, 0, value)

    socketio.run(app, debug=True)