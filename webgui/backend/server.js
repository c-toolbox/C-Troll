/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
 *                                                                                       *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY   *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT   *
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,        *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR    *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    *
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                                               *
 *                                                                                       *
 ****************************************************************************************/

const net = require('net');
const JsonSocket = require('json-socket');
const sockjs = require('sockjs');
const http = require('http');
const pluralize = require('pluralize');
const colors = require('colors');

pluralize.addIrregularRule('is', 'are');

let nConnections = 0;
let nextId = 0;

let guiSocketServer =
    sockjs.createServer({
        sockjs_url: 'http://cdn.jsdelivr.net/sockjs/1.0.1/sockjs.min.js'
    });

function logNConnections() {
    console.log(('There ' + pluralize('is', nConnections) + ' now ' +
        pluralize('connection', nConnections, true) + '.').green);
}

guiSocketServer.on('connection', (guiSocket) => {
    const connectionId = nextId++;
    const rawSocket = new net.Socket();

    let closedCore = false;
    let closedGui = false;

    console.log(('New incoming gui connection (#' + connectionId + ').').green);

    const coreSocket = new JsonSocket(rawSocket);
    coreSocket.connect(19850, '127.0.0.1');

    coreSocket.on('connect', () => {
        console.log(('Connection established to core for connection #' +
            connectionId + '.').green);
        nConnections++;
        logNConnections();
    });

    coreSocket.on('message', (message) => {
        guiSocket.write(JSON.stringify(message));
        console.log('Connection #' + connectionId +
            ': Forwarding message from core to gui:\n', message);
    });

    coreSocket.on('end', () => {
        closedCore = true;
        if (closedGui) {
            console.log(('Connection #' + connectionId +
                ': Disconnected from core.').green);
            logNConnections();
        } else {
            console.log(('Connection #' + connectionId +
                ': Unexpected disconnection from core.').red);
            nConnections--;
            guiSocket.close();
        }
    });

    guiSocket.on('data', (message) => {
        coreSocket.sendMessage(JSON.parse(message));
        console.log('Connection #' + connectionId +
            ': Forwarding message from gui to core:\n', message);
    });

    guiSocket.on('close', () => {
        console.log(('Connection #' + connectionId + ': Disconnected from gui.').green);
        closedGui = true;
        if (closedCore) {
            logNConnections();
        } else {
            nConnections--;
            coreSocket.end();
        }
    });
});

let httpServer = http.createServer();
guiSocketServer.installHandlers(httpServer, {prefix:'/ws'});
httpServer.listen(3001, '127.0.0.1');

logNConnections();
