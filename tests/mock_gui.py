import socket
import json

# Connect to the Core on localhost
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(( "127.0.0.1", 19850))

# Get enough data after connecting
data = s.recv(8192)

#print(data.decode("utf-8"))
# Parsing the GuiInit data into JSON and printing
json.loads(data.decode("utf-8"))

data = {}
data['type'] = "GuiCommand"

payload = {}
payload['command'] = "Start"
payload['application_identifier'] = "itunes"
payload['configuration_identifier'] = ""
payload['cluster_identifier'] = "mock"
data['payload'] = payload
s.send(bytearray(json.dumps(data), 'utf-8'))

s.close()
