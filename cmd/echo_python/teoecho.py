from ctypes import *
import ctypes
import time

teonet = cdll.LoadLibrary("../../lib/libteonet.so")
teonet.teoAddress.restype = ctypes.c_char_p

appName    = b"Teonet echo client Python sample application"
appShort   = b"teoecho-c"
appVersion = b"0.2.1"

echoServer = b"dBTgSEHoZ3XXsOqjSkOTINMARqGxHaXIDxl"

# reader is a teonet channels callback function 
def reader(teo, addr, data, dataLen, ev):
    print("reader")
    # Check teonet event
    if ev != teonet.teoEvData():
        return 0
    
    # // The safe_printf() function must be call in reader before any printf() 
    # // function to safe printf() in multithreading application
    # safe_printf();
    # printf("got data: '%s', data len: %d, from: %s\n\n", data, dataLen, addr);
    # return 1;


# Application logo
teonet.teoLogo(appName, appVersion)

# Create teonet connector
teo = teonet.teoNew(appShort)
if teo == 0:
    print("can't initialize teonet")
    quit(1)

# Connect to teonet
ok = teonet.teoConnect(teo)
if ok == 0:
    print("can't initialize teonet")
    quit(1)

# Get and print your teonet address
address = teonet.teoAddress(teo)
print("Teonet address:", address)

# Connect to teonet echo server
# ok = teonet.teoConnectToCb(teo, echoServer, reader)
ok = teonet.teoConnectTo(teo, echoServer)
if ok == 0:
    print("can't connect to echo server")
    quit(1)

print("Successfully connected to:", echoServer, "\n")

# Send messages to echo server
while True:
    msg = b"Hello from teonet-c!"
    print("send message", msg, "to", echoServer)
    teonet.teoSendTo(teo, echoServer, msg, len(msg))
    time.sleep(5)