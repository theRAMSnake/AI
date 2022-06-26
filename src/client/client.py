#!/usr/bin/python3
import socket
import sys
import json
import time

def parseMessageFromParams():
    funcName = sys.argv[1]
    result = {}
    result["operation"] = funcName

    if funcName == "createProject":
        if len(sys.argv) != 5:
            print("Expected createProject <name> <playground> <engine>")
            sys.exit()
        result["name"] = sys.argv[2]
        result["playground"] = sys.argv[3]
        result["engine"] = sys.argv[4]

    if funcName == "loadProject":
        if len(sys.argv) != 3:
            print("Expected loadProject <name>")
            sys.exit()
        result["name"] = sys.argv[2]

    if funcName == "deleteProject":
        if len(sys.argv) != 3:
            print("Expected loadProject <name>")
            sys.exit()
        result["name"] = sys.argv[2]

    if funcName == "run":
        if len(sys.argv) != 4:
            print("Expected run <fitness> <minutes>")
            sys.exit()
        result["fitness"] = sys.argv[2]
        result["minutes"] = sys.argv[3]

    if funcName == "exportIndividual":
        if len(sys.argv) != 3:
            print("Expected exportIndividual <id>")
            sys.exit()
        result["id"] = sys.argv[2]

    if funcName == "perform":
        if len(sys.argv) != 3:
            print("Expected perfrom <id>")
            sys.exit()
        result["id"] = sys.argv[2]

    if funcName == "set":
        if len(sys.argv) != 4:
            print("Expected set <key> <value>")
            sys.exit()
        result["key"] = sys.argv[2]
        result["value"] = sys.argv[3]

    return json.dumps(result)

def sendReceive(message):
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect the socket to the port where the server is listening
    server_address = ('localhost', 38539)

    sock.connect(server_address)

    size = len(message)
    sizes = [size & 0xFF, (size >> 8) & 0xFF]
    sock.send(bytearray(sizes))
    sock.sendall(bytearray(message, 'utf-8'))

    recv_len = sock.recv(2)
    to_receive = recv_len[0] | (recv_len[1] << 8)
    result = sock.recv(to_receive).decode("utf-8")
    sock.close()
    return result

def main():
    if sys.argv[1] == "follow":
        while True:
            message = {"operation": "getLastSnapshot"}
            print(sendReceive(json.dumps(message)))
            time.sleep(5)
    else:
        message = parseMessageFromParams()
        print(sendReceive(message))

if __name__ == "__main__":
    main()
