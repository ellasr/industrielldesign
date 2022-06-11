#!/usr/bin/env python3 
#coding=utf-8 #trengs for bruk av ulike ascii karakterer 

import time
import pygame 
import serial
import os, random
import pyaudio
import wave
from datetime import datetime

print("Starting") #indikasjon på at koden er i gang

#Initierer variabler:
tidsgrense = float(10)
starttid = 10000000000 #et stort tall for at den ikke skal spille av med en gang
differanse = 0 #setter denne til 0 for at den ikke skal spille av med en gang

#definerer funksjoner
def erkjennelse ():
    randomfile = random.choice(os.listdir('/home/pi/Bakgrunn/')) #hent tilfeldig fil fra denne mappen
    file = '/home/pi/Bakgrunn/'+randomfile #setter sammen navnet på filen
    os.system('omxplayer '+file) #spill den filen

def rndmp3 ():
    randomfile = random.choice(os.listdir('/home/pi/mp3/')) #hent tilfeldig fil fra denne mappen
    file = '/home/pi/mp3/'+randomfile #setter sammen navnet på filen
    os.system('omxplayer '+file) #spill den filen
    

#Bakgrunnsmusikk:
pygame.mixer.pre_init()
pygame.mixer.init()
pygame.init()

pygame.mixer.music.load(os.path.join(os.getcwd(), "sound", "Natur.mp3")) #path til filen som skal spilles
pygame.mixer.music.set_volume(0.3)
pygame.mixer.music.play(loops =-1) #looper evig


#Seriell kommunikasjon mellom raspberry pi og arduino 
if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.flush()

while True: #bruker while true som en evig loop
    differanse = time.time() - starttid #tiden vi vil sjekke 
    print(differanse)
    time.sleep(0.1) #unngå delay og kunne følge med på timer i terminal
    
    if ser.in_waiting > 0: #kommunikasjon mellom arduino og raspberry pi
        line = ser.readline().decode('utf-8').rstrip()
        print("in loop") #indikasjon på at scriptet kjører som det skal

        #hvis arduino sier "start" spilles lydfil
        if (line == "start"):
            #redefinerer starttid to ganger for å unngå problemer
            starttid = time.time() 
            rndmp3() #bruker spill tilfeldig lydfil funskjonen
            starttid = time.time()                     

        #hvis arduino sier record tas lydklipp på ti sek opp
        elif (line == "record"):
            #filnavn basert på dato og klokkeslett via datetime:
            now = datetime.now()
            #lagres i mp3-mappa. .. står for "en mappe opp"
            now = now.strftime("../mp3/%d%m%Y%H%M%S.wav")
            print(now) #bruk til feilsøking 

            chunk = 1024  #ta opp i biter på 1024 samples
            sample_format = pyaudio.paInt16  #gjør det til 16bit per sample
            channels = 2
            fs = 44100  #ta opp 44000 samples per sekund
            seconds = 10 #varighet på opptaket
            filename = now #filnavnet blir en unik dato/tid
            p = pyaudio.PyAudio()

            print('Recording')

            stream = p.open(format=sample_format,
                            channels=channels,
                            rate=fs,
                            frames_per_buffer=chunk,
                            input=True)

            frames = [] #Setter opp array

            #lagre data i biter på 3 sekunder
            for i in range(0, int(fs / chunk * seconds)):
                data = stream.read(chunk)
                frames.append(data)

            #stopp og lukk 
            stream.stop_stream()
            stream.close()
            p.terminate()

            print('Finished recording')

            #lagre dataen som en wav-fil
            wf = wave.open(filename, 'wb')
            wf.setnchannels(channels)
            wf.setsampwidth(p.get_sample_size(sample_format))
            wf.setframerate(fs)
            wf.writeframes(b''.join(frames))
            wf.close()
            
            #setter opp starttiden igjen for at erkjennelse ikke skal gå i loop
            starttid = 10000000000
            
    #sjekker om tidsgrensen er nådd, spiller av hvis den er det        
    elif (differanse >= tidsgrense):
        erkjennelse()
        #setter opp starttiden igjen for at erkjennelse ikke skal gå i loop
        starttid = 10000000000
            



