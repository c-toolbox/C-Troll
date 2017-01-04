import socket
import json

# Connect to the Core on localhost
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect(( "127.0.0.1", 5000))

# Send TrayCommand
data = {}
data['type'] = "TrayCommand"

payload = {}
payload['identifier'] = "12345"
payload['command'] = "exit"
payload['executable'] = "/Applications/Safari.app/Contents/MacOS/Safari"
payload['baseDirectory'] = ""
payload['currentWorkingDirectory'] = ""
payload['commandlineArguments'] = ""
data['payload'] = payload

jsonData = bytearray(json.dumps(data), 'utf-8')
jsonLen = len(jsonData);

s.send(bytearray(str(jsonLen), 'utf-8'))
s.send(bytearray('#', 'utf-8'));
s.send(jsonData)

s.close()
