#!python3
import subprocess, os, signal, socket
import sys
TCP_IP = '192.168.2.4'
TCP_PORT = 5006
BUFFER_SIZE = 100
#MESSAGE = "Hello, World!"
#s.send(MESSAGE)
#data = s.recv(BUFFER_SIZE)
#s.close()

def send_order(order):

    return True 

def main():
    #status = send_order("H")

    dir_name = sys.argv[1]
    duration = float(sys.argv[2])
    os.mkdir(dir_name)
    

    orders = [b'L', b'H']
    prefixs = ['air', 'send']
    for i in range(2):
        filepath = dir_name + '/' + prefixs[i%2] + str(int((i + 1)/ 2)) + '.dat'
        print("saving", filepath)

        order = orders[i % 2]
        status = send_order(order)
        if not status: 
            print("ERROR")
            break

        process = subprocess.Popen(['./log_to_file', filepath])
        try:
            print("Running process......", "ZigBee status", status)
            process.wait(timeout=duration)
        except subprocess.TimeoutExpired:
            process.send_signal(signal.SIGINT)
            #print("start to kill")
            #process.kill()
            print("process done")
    










if __name__ == "__main__":
    main()
