from ctypes import *
import ctypes
import time

teonet = cdll.LoadLibrary("../../lib/libteonet.so")
teonet.teoAddress.restype = ctypes.c_char_p
teonet.teoCVersion.restype = ctypes.c_char_p

appName    = b"Teonet command client Python sample application"
appShort   = b"teocommand-p"
appVersion = teonet.teoCVersion()

echoComServer = b"WXJfYLDEtg6Rkm1OHm9I9ud9rR6qPlMH6NE"

# reader is a teonet channels callback function
@CFUNCTYPE(None, c_int, c_char_p, c_char_p, c_int, c_ubyte)
def reader(teo, addr, data, dataLen, ev):
    # Check teonet event
    if ev != teonet.teoEvData():
        return 0

    # The safe_printf() function must be call in reader before any printf()
    # function to safe printf() in multithreading application
    teonet.safe_printf()
    print("got data", data, "len:", dataLen, "from:", addr, "\n")
    return 1

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
ok = teonet.teoConnectToCb(teo, echoComServer, reader)
if ok == 0:
    print("can't connect to echo server")
    quit(1)

print("Successfully connected to:", echoComServer, "\n")

# Send command to command server
while True:
    msg = b"Hello from teonet-c!"
    print("send cmd 129", msg, "to", echoComServer)
    teonet.teoSendCmdTo(teo, echoComServer, 129, msg, len(msg))
    time.sleep(3)
