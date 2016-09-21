import socket
import json

# Connect to the Core on localhost
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(( "127.0.0.1", 5000))

# Get enough data after connecting
data = s.recv(8192)

# Parsing the GuiInit data into JSON and printing
print(json.loads(data.decode("utf-8")))

# data = {}
# data['type'] = 'GuiInit'
# json_data = json.dumps(data)
# s.send(bytearray(json_data, 'UTF-8'))


s.close()

