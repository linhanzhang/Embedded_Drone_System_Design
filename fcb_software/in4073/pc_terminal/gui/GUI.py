# Siyuan Fang: gui: main.py
# Two threads: one for gu, one for socket communication between c and python

import tkinter as tk
import tkinter.ttk as ttk
import threading
import time
import socket
import sys
from ctypes import *
from PIL import ImageTk, Image

MAX_MOTOR_VALUE = 700

""" This class defines a C-like struct """
class Payload(Structure):
    _fields_ = [("mode", c_uint16),
                ("ae_0", c_int16),
                ("ae_1", c_int16),
                ("ae_2", c_int16),
                ("ae_3", c_int16),
                ("phi", c_int16),
                ("theta", c_int16),
                ("psi", c_int16),
                ("sp", c_int16),
                ("sq", c_int16),
                ("sr", c_int16),
                ("sax", c_int16),
                ("say", c_int16),
                ("saz", c_int16),
                ("P1", c_uint16),
                ("P2", c_uint16),
                ("p_yaw", c_uint16),
                ("yaw_setpoint", c_int16),
                ("bat_volt", c_int16)]

mutex = threading.Lock()
payload_in = Payload()

def socket_rx(lock):
    global temp_rx
    global payload_in
    PORT = 2300
    server_addr = ('localhost', PORT)
    ssock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ssock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    print("Socket created")
    
       
    try:
        # bind the server socket and listen
        ssock.bind(server_addr)
        print("Bind done")
        ssock.listen(3)
        print("Server listening on port {:d}".format(PORT))

        while True:
            csock, client_address = ssock.accept()
            print("Accepted connection from {:s}".format(client_address[0]))

            buff = csock.recv(512)
            while buff:
                # print("\nReceived {:d} bytes".format(len(buff)))
                # time.sleep(1);
                lock.acquire()
                payload_in = Payload.from_buffer_copy(buff)
                # print("Received contents id={:6d}, counter={:6d}, temp={:6d}".format(payload_in.sp,
                #                                             payload_in.sq,
                #                             payload_in.sr))
                                  
                # print("Sending it back.. ", end='')
                nsent = csock.send(payload_in)
                # print("Sent {:d} bytes".format(nsent))
                buff = csock.recv(512)
                lock.release() 

            print("Closing connection to client")
            print("----------------------------")
            csock.close()

    except AttributeError as ae:
        print("Error creating the socket: {}".format(ae))
    except socket.error as se:
        print("Exception on socket: {}".format(se))
    except KeyboardInterrupt:
        ssock.close()
    finally:
        print("Closing socket")
        ssock.close()

def gui_rx(lock):
    global motor_0, motor_1, motor_2, motor_3
    global mode
    global ae_0, ae_1, ae_2, ae_3
    global text_phi, text_theta, text_psi
    global text_sp, text_sq, text_sr
    global text_sax, text_say, text_saz
    global text_P1, text_P2, text_P_YAW
    global text_yaw_setpoint
    global mode_rx
    global battery
    global window
    global payload_in

    mode_rx = ["FAIL", "SAFE", "PANIC", "MANUAL", "CALIBRATE", "YAW", "FULL", "RAW"]

    window = tk.Tk()
    window.title("Embedded System Lab __Group 16 Demo")
    # window.wm_attributes('-transparentcolor', '#ab23ff')
    window.wm_attributes("-alpha", 0.4)
    window.wait_visibility(window)
    # style = darkstyle(window)
    s = ttk.Style()
    s.theme_use("default")
    s.configure("TProgressbar", thickness=35, foreground='gray', background='#00a6d6')
    

    window.columnconfigure([0, 1, 2, 3, 4, 5, 6], minsize=170)
    window.rowconfigure([0, 1, 2, 3, 4, 5, 6, 7], minsize=65)

    bg = ImageTk.PhotoImage(file = "gui_background.jpg")
    label_bg = tk.Label( window, image = bg)
    label_bg.place(x = 0, y = 0)

    label_Title = tk.Label( 
        window, text = "Drone Control Interface",
        font = ("Times New Roman", 22, "bold"),
        bg= '#84a5d8',
    )
    label_Title.place(x = 400, y = 30)

    mode = tk.StringVar()
    mode.set("Mode:")
    label_mode= tk.Label(
        window, textvariable=mode,
        width=3,height=2,
        bg= '#84a5d8',
        font = ("Times New Roman", 22, "bold")
    )   
    label_mode.grid(row=1, column=0, sticky="nesw", padx=10, pady=10)

    name = tk.StringVar()
    name.set("Group 16 Member" +"\n"
                + "koen.wosten" +"\n"
                + "siyuan Fang" +"\n"
                + "hanzhang Lin" +"\n"
                + "tiarui Mao")

    label_name= tk.Label(
        window, textvariable=name,
        width=3,height=2,
        font = ("Times New Roman", 14, "italic"),
        bg= '#84a5d8',
    )
    label_name.grid(row=1, column=5, sticky="nesw", padx=10, pady=10)

    motor_0 = ttk.Progressbar(
        window,
        style="TProgressbar",
        orient='vertical', 
        mode='determinate',
        length=130
    )

    motor_0.grid(column=1, row=1, sticky="nesw", padx=20, pady=40)
    motor_0["value"] = 0
    motor_0["maximum"] = MAX_MOTOR_VALUE

    motor_1 = ttk.Progressbar(
        window,
        style="TProgressbar",
        orient='vertical', 
        mode='determinate',
        length=130
    )

    motor_1.grid(column=2, row=1, sticky="nesw", padx=20, pady=40)
    motor_1["value"] = 0
    motor_1["maximum"] = MAX_MOTOR_VALUE

    motor_2 = ttk.Progressbar(
        window,
        style="TProgressbar",
        orient='vertical', 
        mode='determinate',
        length=110
    )

    motor_2.grid(column=3, row=1, sticky="nesw", padx=20, pady=40)

    motor_2["value"] = 0
    motor_2["maximum"] = MAX_MOTOR_VALUE

    motor_3 = ttk.Progressbar(
        window,
        style="TProgressbar",
        orient='vertical', 
        # vertical
        mode='determinate',
        length=110
    )
    motor_3.grid(column=4, row=1, sticky="nesw", padx=20, pady=40)
    motor_3["value"] = 0
    motor_3["maximum"] = MAX_MOTOR_VALUE

    ae_0 = tk.StringVar()
    ae_0.set("ae[0]")
    label_ae_0 = tk.Label(
        window, textvariable= ae_0,
        width=3,height=2,
        # font=("Arial", 20)
        font = ("Times New Roman", 21, "bold"),
    )   
    label_ae_0.grid(row=2, column=1, sticky="nesw", padx=10, pady=10)

    ae_1 = tk.StringVar()
    ae_1.set("ae[1]")
    label_ae_1 = tk.Label(
        window, textvariable=ae_1,
        width=3,height=2,
        font = ("Times New Roman", 21, "bold")
    )   
    label_ae_1.grid(row=2, column=2, sticky="nesw", padx=10, pady=10)

    ae_2 = tk.StringVar()
    ae_2.set("ae[2]")
    label_ae_2 = tk.Label(
        window, textvariable=ae_2,
        width=3,height=2,
        font = ("Times New Roman", 21, "bold")
    )   
    label_ae_2.grid(row=2, column=3, sticky="nesw", padx=10, pady=10)

    ae_3 = tk.StringVar()
    ae_3.set("ae[3]")
    label_ae_3 = tk.Label(
        window, textvariable=ae_3,
        width=3,height=2,
        font = ("Times New Roman", 21, "bold")
    )   
    label_ae_3.grid(row=2, column=4, sticky="nesw", padx=10, pady=10)

    # frame1 = ttk.LabelFrame(window, text='Angle datas')

    text_phi = tk.StringVar()
    text_phi.set("phi:")
    label_phi = tk.Label(
        window, textvariable=text_phi,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_phi.grid(row=3, column=2, sticky="nesw", padx=10, pady=10)

    text_theta = tk.StringVar()
    text_theta.set("theta:")
    label_theta = tk.Label(
        window, textvariable=text_theta,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_theta.grid(row=3, column=3, sticky="nesw", padx=10, pady=10)

    text_psi = tk.StringVar()
    
    text_psi.set("psi:")
    label_psi = tk.Label(
        window, textvariable=text_psi,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_psi.grid(row=3, column=4, sticky="nesw", padx=10, pady=10)

    text_sp = tk.StringVar()
    text_sp.set("sp:")
    label_sp = tk.Label(
        window, textvariable=text_sp,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_sp.grid(row=4, column=2, sticky="nesw",padx=10, pady=10)

    text_sq = tk.StringVar()
    text_sq.set("sq:")
    label_sq = tk.Label(
        window, textvariable=text_sq,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_sq.grid(row=4, column=3, sticky="nesw", padx=10, pady=10)

    text_sr = tk.StringVar()
    text_sr.set("sr:")
    label_sr = tk.Label(
        window, textvariable=text_sr,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_sr.grid(row=4, column=4, sticky="nesw",padx=10, pady=10)

    text_sax = tk.StringVar()
    text_sax.set("sax:")
    label_sax = tk.Label(
        window, textvariable=text_sax,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_sax.grid(row=5, column=2, sticky="nesw", padx=10, pady=10)

    text_say = tk.StringVar()
    text_say.set("say:")
    label_say = tk.Label(
        window, textvariable=text_say,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_say.grid(row=5, column=3, sticky="nesw", padx=10, pady=10)

    text_saz = tk.StringVar()
    text_saz.set("saz:")
    label_saz = tk.Label(
        window, textvariable=text_saz,
        width=3,height=2,
        font = ("Times New Roman", 17, "bold")
    )   
    label_saz.grid(row=5, column=4, sticky="nesw", padx=10, pady=10)

    text_P1 = tk.StringVar()
    text_P1.set("P1:")
    label_P1 = tk.Label(
        window, textvariable=text_P1,
        width=3,height=2,
        font=("Times New Roman", 16, "bold")
    )   
    label_P1.grid(row=2, column=5, sticky="nesw",padx=10, pady=10)

    text_P2 = tk.StringVar()
    text_P2.set("P2:")
    label_P2 = tk.Label(
        window, textvariable=text_P2,
        width=3,height=2,
        font=("Times New Roman", 16, "bold")
    )   
    label_P2.grid(row=3, column=5, sticky="nesw", padx=10, pady=10)

    text_P_YAW = tk.StringVar()
    text_P_YAW.set("P_YAW:")
    label_P_YAW = tk.Label(
        window, textvariable=text_P_YAW,
        width=3,height=2,
        font=("Times New Roman", 16, "bold")
    )   
    label_P_YAW.grid(row=4, column=5, sticky="nesw", padx=10, pady=10)

    text_yaw_setpoint = tk.StringVar()
    text_yaw_setpoint.set("Yaw_setpoint:")
    label_yaw_setpoint = tk.Label(
        window, textvariable=text_yaw_setpoint,
        width=3,height=2,
        font=("Times New Roman", 14, "bold")
    )   
    label_yaw_setpoint.grid(row=5, column=5, sticky="nesw", padx=10, pady=10)

    battery = ttk.Progressbar(
        window,
        style="TProgressbar",
        orient='horizontal', 
        mode='determinate',
        length= 80
    )

    battery.grid(column=1, row=5, sticky="nesw", padx=10, pady=10)
    battery["value"] = 0
    battery["maximum"] = 1260

    # require lock to access payload_in
    lock.acquire()
    updateDisplay()
    lock.release()
    # GUI main loop
    window.mainloop()


# siyuan: update sensor data
def updateDisplay():
    global motor_0, motor_1, motor_2, motor_3
    global mode
    global mode_rx
    global ae_0, ae_1, ae_2, ae_3
    global text_phi, text_theta, text_psi
    global text_sp, text_sq, text_sr
    global text_sax, text_say, text_saz
    global text_P1, text_P2, text_P_YAW
    global text_yaw_setpoint
    global battery
    
    global window
    global payload_in
    mode.set(mode_rx[payload_in.mode])
    motor_0["value"] = payload_in.ae_0
    motor_1["value"] = payload_in.ae_1
    motor_2["value"] = payload_in.ae_2
    motor_3["value"] = payload_in.ae_3
    ae_0.set("ae_0: " + str(payload_in.ae_0))
    ae_1.set("ae_1: " + str(payload_in.ae_1))
    ae_2.set("ae_2: " + str(payload_in.ae_2))
    ae_3.set("ae_3: " + str(payload_in.ae_3))
    text_phi.set("phi: " + str(payload_in.phi//182))
    text_theta.set("theta: " + str(payload_in.theta//182))
    text_psi.set("psi: " + str(payload_in.psi//182))
    text_sp.set("sp: " + str(payload_in.sp))
    text_sq.set("sq: " + str(payload_in.sq))
    text_sr.set("sr: " + str(payload_in.sr))
    text_sax.set("sax: " + str(payload_in.sax))
    text_say.set("say: " + str(payload_in.say))
    text_saz.set("saz: " + str(payload_in.saz))
    
    text_P1.set("P1: " + str(payload_in.P1))
    text_P2.set("P2: " + str(payload_in.P2))
    text_P_YAW.set("P_YAW: " + str(payload_in.p_yaw))
    text_yaw_setpoint.set("yaw_setpoint: " + str(payload_in.yaw_setpoint))

    battery["value"] = payload_in.bat_volt

    window.after(15, updateDisplay)


# def darkstyle(root):
#     ''' Return a dark style to the window'''
    
#     style = ttk.Style(root)
#     root.tk.call('source', 'azure dark/azure_dark.tcl')
#     style.theme_use('azure')
#     style.configure("Accentbutton", foreground='white')
#     style.configure("Togglebutton", foreground='white')
#     return style

if __name__ == "__main__":

    t1 = threading.Thread(target= gui_rx, args = (mutex,), name = "Thread 1")
    t1.setDaemon(True)
    t2 = threading.Thread(target= socket_rx ,args = (mutex,), name = "Thread 2")
    # t2.setDaemon(True)
  
    t1.start()
    t2.start()
  
    t1.join()
    t2.join()
  
    print("Finsh Demo!")