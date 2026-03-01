import socket
import time
import struct

BRIDGE_IP = "127.0.0.1"
BRIDGE_PORT = 5000
SIM_PORT = 5001

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((BRIDGE_IP, SIM_PORT))
    sock.settimeout(1.0)

    print(f"Sim tester running on {BRIDGE_IP}:{SIM_PORT}")
    print(f"Sending to bridge on {BRIDGE_IP}:{BRIDGE_PORT}")
    print("-" * 40)

    count = 0
    while True:
        try:
            # Send data to bridge
            message = f"SIM_DATA:{count}".encode()
            sock.sendto(message, (BRIDGE_IP, BRIDGE_PORT))
            print(f"[TX] Sent: {message.decode()}")

            # Try to receive data from bridge
            try:
                data, addr = sock.recvfrom(256)
                print(f"[RX] Received: {data.hex()} ({len(data)} bytes) from {addr}")
            except socket.timeout:
                print("[RX] No data received")

            count += 1
            time.sleep(1.0)

        except KeyboardInterrupt:
            print("\nStopping tester...")
            break

    sock.close()

if __name__ == "__main__":
    main()