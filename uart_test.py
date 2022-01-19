#!/usr/bin/python3
import serial
import time
import sys
class UART():
    def __init__(self):
        self.ser = serial.Serial("/dev/serial0",9600)
        self.ser.flush()
        pass

    def writeCoordinates(self, x, y, extrude=False):
        x = int(x)*10
        y = int(y)*10
        if((0 > x or x > 0xFFFF) or (0 > y or y > 0xFFFF)):
            print("Error")
            return -1
        
        num = x + (y << 16)

        cmd:int = 0b01 if extrude else 0b00

        cmd = (num << 8) + cmd

        cmd = int.to_bytes(cmd,5,"little")

        self.ser.write(cmd);
        return cmd

    def writeWait(self, t):
        t = int(t)
        cmd = (int.to_bytes(2+(t<<8),3,"little"))
        self.ser.write(t)

    def writeDone(self):
        cmd = int.to_bytes(3,1,"little")
        self.ser.write(cmd)

    def sendData(self, file):
#        with file as f:
         lines = file.split("\n")
         for i in range(len(lines)):
             line = lines[i]
             line = line.strip()
             line = line.split()
             print(i)
             print(line)
             if(line[0].strip() == "done"):
                    # continue
                 self.writeDone()
             if(line[0].strip() == "move_dry"):
                    # continue
                 self.writeCoordinates(line[1].strip(),line[2].strip(),True)
             if(line[0].strip() == "move_wet"):
                    # continue
                 self.writeCoordinates(line[1].strip(),line[2].strip(),True)
             if(line[0].strip() == "wait"):
                 continue
                 self.writeWait(line[1].strip())
             time.sleep(.05)
             continue
             data = self.ser.read()
             print(int.from_bytes(data,"little"))
             if(data == b'\x01'):
                 pass
             elif(data == b'\x00'):
                 pass
             elif(data == b'\x69'):
                 #resend
                 i -= 1
             else:
                 print("BUFFER FULL")
                 print(data)
                 i -= 1
                 while(self.ser.read() != b'\0x00'):
                     pass
                 print("SENDING MORE")
             time.sleep(.05)

    def close(self):
        self.ser.close()

x = UART()

if(len(sys.argv)) > 1:
    with open(sys.argv[1],'r') as file:
        c = file.read().strip().split("\n")
        for line in c:
            print(line)
            x.sendData(line)
        print("done")
        x.sendData("done")
    exit(0)

x_ = input("x:")
while len(x_)>0:
    x.sendData(x_)
    x_ = input("x:")
