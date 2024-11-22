
import serial
import serial.tools.list_ports

class Serial:
    def __init__(self):
        self.ser = None
    
    def open(self, isOpen):
        if isOpen:
            connected = self.get_ports()
            if connected:
                self.ser = serial.Serial(connected[0], 9600)
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
        x = int(x) + 100
        y = int(y) + 100
        z = int(z)+ 100
        print(x-100, y-100, z-100)
        self.ser.write(f"{x} {y} {z}\n".encode())