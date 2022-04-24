#venv\Scripts\activate.bat

import serial
import pandas as pd

import numpy as np
import matplotlib.pyplot as plt

import time
import os

import sys

serialport = serial.Serial('COM3', baudrate=9600, timeout=2)

def datacollection():
    #i = 'r'
    #serialport.write(b'r')

    #time.sleep(5)
    #attempting to communicate with arduino to read new data every time looped
    #arduino data not being collected, void not being triggered by instruction

    openport = False

    print('Serial port open')

    array = []
    array_split = []

    open = True

    while open:
        arduinodata = serialport.readline().decode('ascii').strip()
        array = arduinodata.split(',')
        array_split.append(array)
        
        if not arduinodata or arduinodata == 'exit':
            open = False
    
    np.savetxt('pythonpiezodataarray.csv', array_split, delimiter=',', fmt='%s')

    return array_split

def dataprocessing(array_split):
    FREQ = []
    VREF = []
    VM = []
    VP = []
    COM = []

    length = len(array_split)

    print('Processing data')
    for i in range(1, length-1):
        FREQ.append(float(array_split[i][0]))
        VREF.append(float(array_split[i][1]))
        VM.append(float(array_split[i][2]))
        VP.append(float(array_split[i][3]))
        COM.append(float(array_split[i][4]))

    V_DIFFERENCE = []
    Phase = []

    for i in range(0,length-2):
        V_DIFFERENCE.append(np.abs(VM[i]-VREF[i]))
        Phase.append(VP[i])

    #print(Phase)
    #print(FREQ)
    #print(V_DIFFERENCE)

    phase_peak = max(Phase)
    phase_peak_index = Phase.index(phase_peak)
    driving_freq = FREQ[phase_peak_index]

    print('Values:')
    print('Phase peak =', phase_peak)
    print('Corresponding driving frequency =', driving_freq, 'Hz')
    return FREQ, V_DIFFERENCE, Phase, driving_freq

#index of phase/impedance value gives corresponding driving frequency to send to arduino
#update frequency on arduino
#automate
#csv output every time run

def graph(FREQ, V_DIFFERENCE, Phase):
    print('Creating graph')
    plt.rcParams["font.family"] = "Times New Roman"

    fig, ax1 = plt.subplots()
    ax1.set_xlabel('Frequency (Hz)')
    ax1.set_ylabel('Impedance (Ohms)', color='red')
    ax1.plot(FREQ, V_DIFFERENCE, color='red')
    ax1.tick_params(axis='y')
    ax2 = ax1.twinx()
    ax2.set_ylabel('Phase (Degrees)', color='blue')
    ax2.plot(FREQ, Phase, color='blue')
    ax2.tick_params(axis = 'y')
    fig.tight_layout()
    plt.show()


    #save and exit this graph so code can continue

#manual import of freq input the arduino

def sendfreq(driving_freq):
    #i = 's'
    #serialport.write(b's')

    print('Started open port')
    j = str(driving_freq)
        
    serialport.write(j.encode())
    time.sleep(0.5)
    print(serialport.readline().decode('ascii'))

    #Run it
def programme():
    array_split = datacollection()
    values = dataprocessing(array_split)

    FREQ = values[0]
    V_DIFFERENCE = values[1]
    Phase = values[2]
    driving_freq = values[3]

    graph(FREQ, V_DIFFERENCE, Phase)

    sendfreq(driving_freq)

    return driving_freq

driving_freq = programme()

#table or graph; driving frequency against time as heated etc.
"""
DFarray = []
loop = 0
time_int = 30
time_rel = 0
Tarray = []

#Run code every minute after triggered

while loop < 5:
    driving_freq = programme()
    time.sleep(time_int)
    DFarray.append(driving_freq)
    time_rel += time_int
    Tarray.append(time_rel)
    loop += 1

print(DFarray) 
"""



