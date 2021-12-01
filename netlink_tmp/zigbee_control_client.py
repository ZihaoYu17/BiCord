#!python3
import subprocess, os, signal, socket
import sys
TCP_IP = '192.168.2.4'
TCP_PORT = 5005
BUFFER_SIZE = 100
#MESSAGE = "Hello, World!"
#s.send(MESSAGE)
#data = s.recv(BUFFER_SIZE)
#s.close()

def send_order(order):

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))
    #order = order.encode("utf-8")
    s.sendall(order)
    print("send order", order)

    ack = s.recv(BUFFER_SIZE)
    print("receive", ack)
    s.close()

    return ack == b'1'

def main():
    #status = send_order("H")

    dir_name = sys.argv[1]
    duration = float(sys.argv[2])
    #os.mkdir(dir_name)
    

    orders = [b'L', b'H']
    prefixs = ['air', 'send']
    for i in range(20):
        filepath = dir_name + '/' + prefixs[i%2] + str(int((i + 1)/ 2)) + '.dat'
        print("saving", filepath)

        order = orders[i % 2]
        status = send_order(order)
        if not status: 
            print("ERROR")
            break

        #process = subprocess.Popen(['./log_to_file', filepath])
        process = subprocess.Popen(['./log_to_file', "serv_port_ip.conf"])
        try:
            print("Running process......", "ZigBee status", status)
            process.wait(timeout=duration)
        except subprocess.TimeoutExpired:
            #process.send_signal(signal.SIGINT)
            process.kill()
            print("process done")
    










if __name__ == "__main__":
    main()
