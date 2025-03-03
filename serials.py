
import serial
import serial.tools.list_ports
import threading

class Serial:
    def __init__(self):
        self.ser = None
        self.x = {}
        self.y = {}
        self.offsets = [0, 0, 0]
    
    def open(self, isOpen):
        if isOpen:
            connected = self.get_ports()
            if connected:
                self.ser = serial.Serial(connected[0], 9600)
                thread = threading.Thread(target=self.read)
                thread.start()
                return True
            else:
                return False
            
        else:
            if self.ser:
                self.ser.close()
            return False
        
    def get_ports(self):
        comlist = serial.tools.list_ports.comports()
        connected = []
        for element in comlist:
            if "Arduino" in element.description or "USB" in element.description:
                connected.append(element.device)
        return connected

    def write(self, x, y, z):
        x = int(x)
        y = int(y)
        z = int(z)
        self.ser.write(f"w {x} {y} {z}\n".encode())
    
    def offset(self, pos, x):
        self.offsets[pos-1] = x
        self.ser.write(f"o {self.offsets[0]} {self.offsets[1]} {self.offsets[2]}\n".encode())

    def move(self, type, value, min, max):
        type = type[-1].lower()
        print("move ", type, value, min, max)
        self.ser.write(f"{type} {value} {min} {max}\n".encode())
    # a : 0 ~ 180
    def read(self):
        while True:
            line = self.ser.readline().decode().strip()
            print(line)
