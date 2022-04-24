#cd /Users/zakwatts/Coding/MImiZakLocal/2ndTerm/040222/TestPALED
#source venv/bin/activate

#This is the python code to be used with the Adruino code to operate an acoustic deformation chamber
#For notes and use please vist - GITHUB "all rights reserved"

#By Zak Watts and Mimi Houlihan

#Import required plugins
#XXXX If you have the package serial installed, pip unistall it, then pip install pyserial to avoid errors
import time
import os
import sys

import serial
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt



#Establish serial connection
'''serialport = serial.Serial('COM4', baudrate=9600, timeout=2)'''
serialport = serial.Serial('/dev/cu.usbmodem1201', baudrate=9600, timeout=2)

#start of the program
def main():
    print()
    print("************Welcome to the Acoustic Chamber Software (ACS)**************")
    print()
    menu()



def menu():
    print("Please select how you would like to operate the device (type letter)")

    choice = input("""
                      A: Manually frequency selection
                      B: Auto find anti-resonance frequency selection (No DATA EXPORT)
                      C: The above with CSV + Graph
                      D: Time repeat of B
                      Q: Quit

                      Please enter your choice: """)

    if choice == "A" or choice =="a":
        manualfreq()

    elif choice == "B" or choice =="b":
        autofreq()

    elif choice == "C" or choice =="c":
        autofreqEXPORT()

    elif choice == "D" or choice =="d":
        repeatautofreq()

    elif choice=="Q" or choice=="q":
        sys.exit

    else:
        print("You must only select either A or B")
        print("Please try again")
        menu()




def manualfreq():
    print("You have selected to manually enter a frequency")

    manualfreqchoice = input("Input the frequency you would like to drive the piezo at (in Hz): ")
    print("You have chosen to drive the piezo at ", manualfreqchoice," Hz")

    timeoperated = input("Input the amount of time you would like to operate the piezo for (milliseconds: ")
    print("You have chosen to operate the piezo for ", timeoperated, " mS")

    sendfreq(manualfreqchoice, timeoperated)

    endmenu()
    





def autofreq():
    print("You have selected to automatically find the anti-resonance frequency")

    timeoperated = input("Input the amount of time you would like to operate the piezo for (milliseconds: ")
    print("You have chosen to operate the piezo for ", timeoperated, " mS")

    array_split = freqscan()
    values = dataprocessing(array_split)

    FREQ = values[0]
    V_DIFFERENCE = values[1]
    Phase = values[2]
    driving_freq = values[3]


    sendfreq(driving_freq, timeoperated)

    endmenu()



def autofreqEXPORT():
    print("You have selected to automically find the anti-resoance frew and export and csv and graph")

    array_split = freqscan()
    values = dataprocessing(array_split)

    FREQ = values[0]
    V_DIFFERENCE = values[1]
    Phase = values[2]
    driving_freq = values[3]

    savedata(array_split)
    makegraph(FREQ, V_DIFFERENCE, Phase)

    sendfreq(driving_freq)





def repeatautofreq():
    print("You have selected to repeatedly find the anti-resonance frequency")



#Code to calcuate the driving frequency once data has been recieved
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



#The code to find anti-resonant freq
def freqscan():

    #send the byte "F" to tell the arduino to scan the freqs and send back the data
    serialport.write(b'F')

    
    array = []
    array_split = []
    open = True

    while open:
        arduinodata = serialport.readline().decode('ascii').strip()
        array = arduinodata.split(',')
        array_split.append(array)
        
        if not arduinodata or arduinodata == 'exit':
            open = False

    return array_split




#The code to send the desired frequency back to the arduino
def sendfreq(driving_freq, timeoperated):

    #Establish serial connection
    #serialport = serial.Serial('COM4', baudrate=9600, timeout=2)

    #Send the byte "S" to tell the arudino to listen to what to what to drive the frequncy at
    serialport.write(b'S')

    print("written byte")

    j = str(driving_freq)
        
    serialport.write(j.encode())

    print("freq sent")
    #listen to hear for a confirmation that the frequency has been sent

    i = str(timeoperated)

    serialport.write(i.encode())

    print("time on sent")

    time.sleep(0.5)
    print(serialport.readline().decode('ascii'))
    print("recieved handshake")





def savedata(array_split):
    np.savetxt('pythonpiezodataarray.csv', array_split, delimiter=',', fmt='%s')


#Be sure to close down the graph to allow the code ot continue
def makegraph(FREQ, V_DIFFERENCE, Phase):
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


def programme():
    array_split = freqscan()
    values = dataprocessing(array_split)

    FREQ = values[0]
    V_DIFFERENCE = values[1]
    Phase = values[2]
    driving_freq = values[3]

    makegraph(FREQ, V_DIFFERENCE, Phase)

    sendfreq(driving_freq)



def opmenu():
    print("The code is currently running")


def endmenu():
    print("Thanks for using the Acoustic Chamber Software")

    endchoice = input("""
                      A: Go back to the start menu
                      Q: Quit

                      Please enter your choice: """)
            
    if endchoice== "A" or endchoice== "a":
        menu()
    
    elif endchoice== "Q" or endchoice== 'q':
        quit()



#This is where the programme is initiated
main()




"""
    savedatachoice = input("Do you wish to save this data (Y/N)")

    if savedatachoice =="Y" or savedatachoice=="y":
        savedata(array_split)

    elif savedatachoice =="N" or savedatachoice=="n":
        return

    else:
        print("You must only select either Y or N")
        print("Please try again")


    makegraphchoice = input("Do you wish to have an impedance vs frequency graph?")

    if makegraphchoice =="Y" or makegraphchoice =="y":
        makegraph(FREQ, V_DIFFERENCE, Phase)

    elif makegraphchoice =="N" or makegraphchoice=="n":
        return

    else:
        print("You must only select either Y or N")
        print("Please try again")
"""